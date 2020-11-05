/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Giulia Meuli
*------------------------------------------------------------------------------------------------*/

#pragma once
#include <easy/utils/dynamic_bitset.hpp>
#include "mapping_strategy.hpp"
#include <caterpillar/solvers/solver_manager.hpp>
#include <caterpillar/structures/stg_gate.hpp>
#include <caterpillar/structures/pebbling_view.hpp>
#include <caterpillar/structures/abstract_network.hpp>
#include <caterpillar/synthesis/strategies/pebbling_mapping_strategy.hpp>
#include <caterpillar/solvers/z3_solver.hpp>
#include <mockturtle/networks/abstract_xag.hpp>
#include <mockturtle/views/topo_view.hpp>
#include <mockturtle/views/depth_view.hpp>
#include <tweedledum/networks/netlist.hpp>
#include <caterpillar/details/depth_costs.hpp>
#include <algorithm>
#include <chrono>
#include <fmt/format.h>
using namespace std::chrono;


namespace caterpillar
{
using abstract_xag_network = mockturtle::abstract_xag_network;
using abs_node_t = abstract_xag_network::node;
using abs_steps_xag_t = std::vector<std::pair<abs_node_t, mapping_strategy_action>>;


inline std::vector<cone_t> get_cones( abs_node_t node, abstract_xag_network const& xag, std::vector<uint32_t> const& drivers )
{
  assert(xag.is_and(node));
  // I can assume that AND inputs are not complemented
  // AND I can assume that include cases have been taken care of
  // collect the cones
  std::vector<cone_t> cones;
  xag.foreach_fanin(node, [&] (auto f)
  {
    std::vector<uint32_t> lsets;
    auto fi = xag.get_node(f);
    if(xag.is_nary_xor(fi) && std::find(drivers.begin(), drivers.end(), fi) == drivers.end())
    {
      xag.foreach_fanin(fi, [&] (auto s)
      {
        auto l = xag.get_node(s);
        lsets.push_back(l);
      });
    }
    else 
    {
      lsets = {fi};
    }
    assert(!lsets.empty());
    if(lsets.size() == 1) assert(lsets[0] == fi);
    cones.push_back({fi, lsets});
  });

  assert(cones.size() == 2);

  auto left = cones[0].leaves;
  auto right = cones[1].leaves;


  /* search a target for first */
  std::set_difference(left.begin(), left.end(), 
    right.begin(), right.end(), std::inserter(cones[0].target, cones[0].target.begin()));

  /* set difference */
  std::set_difference(right.begin(), right.end(), 
    left.begin(), left.end(), std::inserter(cones[1].target, cones[1].target.begin()));
  
  return cones;
}

static inline abs_steps_xag_t gen_steps( abs_node_t node, bool compute, abstract_xag_network const& xag, std::vector<uint32_t> const& drivers)
{
  abs_steps_xag_t parity_steps;
  abs_steps_xag_t comp_steps;

  if(xag.is_and(node))
  {
    auto cones = get_cones(node, xag, drivers);
    // compute inplace xors
    for(auto cone : cones)
    {
      if(xag.is_and(cone.root) || xag.is_pi(cone.root) || cone.leaves.size() == 1) continue;
      if ( cone.target.empty() )
      {
        parity_steps.push_back( {cone.root, compute_action{ cone.leaves, std::nullopt}} );
      }
      else 
      {
        parity_steps.push_back( {cone.root, compute_inplace_action{static_cast<uint32_t>( cone.target[0] ), cone.leaves}} );
      } 
    }

    comp_steps.insert(comp_steps.begin(), parity_steps.begin(), parity_steps.end());

  }

  if(compute)
    comp_steps.push_back( {node, compute_action{}} );
  else
    comp_steps.push_back( {node, uncompute_action{}} );

  if (xag.is_and(node)) 
    comp_steps.insert(comp_steps.end(), parity_steps.begin(), parity_steps.end());
  
  return comp_steps;
}

/* does not clean up possibly empty levels */
static inline std::vector<std::vector<abs_node_t>> get_levels_asap( abstract_xag_network const& xag_t, std::vector<abs_node_t> const& drivers)
{
  mockturtle::depth_view<abstract_xag_network, caterpillar::xag_depth_cost<abstract_xag_network>> xag {xag_t};

  /* AND nodes per level */
  std::vector<std::vector<uint32_t>> nodes_per_level( xag.depth() );
  xag.foreach_gate( [&]( auto const& n ) {
    if( xag.is_and(n) || std::find( drivers.begin(), drivers.end(), n ) != drivers.end() ) 
    {
      auto l = xag.level(n);
      nodes_per_level[l - 1u].push_back( n );
    }
  });
  return nodes_per_level;
}

/* get nodes per level (ALAP) */
static inline std::vector<std::vector<abs_node_t>> get_levels_alap( abstract_xag_network const& xag_t, std::vector<abs_node_t> const& drivers)
{
  mockturtle::depth_view<abstract_xag_network, xag_depth_cost<abstract_xag_network>> xag {xag_t};

  /* AND nodes per level */
  std::vector<std::vector<uint32_t>> nodes_per_level( xag.depth() );

  /* compute parents */
  mockturtle::node_map<std::vector<uint32_t>, abstract_xag_network> parents( xag );
  xag.foreach_node( [&]( auto const& n ) {
    if ( !( xag.is_and( n ) || (std::find(drivers.begin(), drivers.end(), n) != drivers.end()) ) ) return;

    xag.foreach_fanin( n, [&]( auto const& f ) {
      const auto child = xag.get_node( f );
      if ( xag.is_and( child ) || (std::find(drivers.begin(), drivers.end(), child) != drivers.end()) )
      {
        parents[child].push_back( n );
      }
      else if ( xag.is_nary_xor( child ) )
      {
        xag.foreach_fanin( child, [&]( auto const& gf ) {
          const auto gchild = xag.get_node( gf );
          if ( xag.is_and( gchild ) )
          {
            parents[gchild].push_back( n );
          }
        });
      }
    });
  } );

  /* reverse TOPO */
  mockturtle::node_map<uint32_t, abstract_xag_network> level( xag );
  for ( auto n = xag.size() - 1u; n > 0u; --n )
  {
    if ( !( xag.is_and( n ) || (std::find(drivers.begin(), drivers.end(), n) != drivers.end()) ) ) continue;

    if ( parents[n].empty() )
    {
      const auto l = xag.depth() - 1u;
      nodes_per_level[l].push_back( n );
      level[n] = l;
    }
    else
    {
      const auto l = level[*std::min_element( parents[n].begin(), parents[n].end(), [&]( auto n1, auto n2 ) { return level[n1] < level[n2]; } )] - 1u;
      nodes_per_level[l].push_back( n );
      level[n] = l;
    }
  }
  xag.foreach_gate([&] (auto gate)
  {
    auto alap_level = level[gate];
    xag.foreach_fanin(gate, [&] (auto f)
    {
      auto fgate = xag.get_node(f);
      assert( level[fgate] < alap_level);
    });
  });

  return nodes_per_level;
}

/*!
  \verbatim embed:rst
    This strategy is dedicated to XAG graphs and fault tolerant quantum computing.
    It exploits two main facts:

    1.  XORs are relatively cheap to be implemented in fault tolerant quantum computing
    2.  Toffoli gates visited to implement AND nodes can be uncomputed using 0 T gates

    Details can be found in :cite:`MSC19`.
  \endverbatim
*/
class abstract_xag_mapping_strategy : public mapping_strategy<abstract_xag_network>
{

public:
  bool compute_steps( abstract_xag_network const& ntk ) override
  {
    mockturtle::topo_view xag {ntk};

    auto drivers = detail::get_outputs(xag);                                                     
    //auto fi  = get_fi(xag, drivers);
    auto it = steps().begin();

    xag.foreach_gate( [&]( auto node ) {
      
      if ( xag.is_and( node ) || std::find( drivers.begin(), drivers.end(), node ) != drivers.end() )
      {

        auto cc = gen_steps( node , /* compute */ true, xag, drivers);
        it = steps().insert( it, cc.begin(), cc.end() );
        it = it + cc.size();

        if ( std::find( drivers.begin(), drivers.end(), node ) == drivers.end()  )
        { 
          auto uc = gen_steps( node , false, xag, drivers);
          it = steps().insert( it, uc.begin(), uc.end() );
        }

      }

    } );

    return true;
  }
};

class abstract_xag_fast_lowt_mapping_strategy : public mapping_strategy<abstract_xag_network>
{

public:
  bool compute_steps( abstract_xag_network const& ntk ) override
  {
    mockturtle::topo_view xag {ntk};

    auto drivers = detail::get_outputs(xag);                                                     
    auto levels = get_levels_asap(xag, drivers);

    auto it = steps().begin();

    for(auto lvl : levels)
    { 
      assert( lvl.size() > 0 );
      /* store two action sets for each node in the level */
      std::vector<std::pair<uint32_t, std::vector<cone_t>>> node_and_action;      
      std::vector<std::pair<uint32_t, std::vector<cone_t>>> to_be_uncomputed;

      for(auto n : lvl)
      {
        /* nary xor outs have empty cones */
        if(xag.is_and(n))
        {
          auto cones = get_cones(n, xag, drivers);
          node_and_action.push_back({n, cones });
        }
        else
        {
          node_and_action.push_back({n, {}});
        }
      }

      it = steps().insert(it, {lvl[0], compute_level_action{node_and_action}});
      it = it + 1;
      
      for(auto node : node_and_action)
      {
        if(std::find(drivers.begin(), drivers.end(), node.first) == drivers.end())
        to_be_uncomputed.push_back(node);
      }
      it = steps().insert(it, {lvl[0], uncompute_level_action{to_be_uncomputed}});
    }

    return true;
  }
};
/*!
  \verbatim embed:rst
    This strategy is dedicated to XAG graphs and fault tolerant quantum computing.
    It finds a strategy that aim to minimize the number of T-stages or T-depth.
    Every level uses some extra-qubits to copy AND inputs.
  \endverbatim
*/
class abstract_xag_low_depth_mapping_strategy : public mapping_strategy<abstract_xag_network>
{

  void eval_copies(std::vector<cone_t>& cones, easy::utils::dynamic_bitset<>& visited)
  {
    //problem gere as leaves are empty when there is a direct ling
    for(auto& cone : cones) 
    {
      for (auto l : cone.leaves )
      {
        assert( l < visited.num_bits());
        if( visited[l] == true)
        {
          cone.copies.push_back(l);
        }
      }
    }
    for(auto cone : cones)
    {
      for(auto l : cone.leaves )
      {
        visited.set_bit(l);
      }
    }
  }

  bool _alap;

public: 
  
  abstract_xag_low_depth_mapping_strategy (bool use_alap = false)
  : _alap(use_alap){}

  bool compute_steps( abstract_xag_network const& ntk ) override
  {
    // the strategy proceeds in topological order and level by level
    mockturtle::topo_view xag {ntk};

    auto drivers = detail::get_outputs(xag);
    auto levels = _alap ? get_levels_alap(xag, drivers) : get_levels_asap(xag, drivers);

    auto it = steps().begin();

    for(auto lvl : levels) if(lvl.size() > 0 ) {
    {
      /* store two action sets for each node in the level */
      std::vector<std::pair<uint32_t, std::vector<cone_t>>> node_and_action;      
      std::vector<std::pair<uint32_t, std::vector<cone_t>>> to_be_uncomputed;

      easy::utils::dynamic_bitset<> visited;
      visited.resize(xag.size());

      for(auto n : lvl)
      {
        if(xag.is_and(n))
        {
          auto cones = get_cones(n, xag, drivers);
          if(xag.is_and(n))
          {
            /* modifies cones.leaves and cones.copies according to visited */
            eval_copies(cones, visited);
          }
          node_and_action.push_back({n, cones});
        }
        else
        {
          node_and_action.push_back({n, {}});
        }
      }

      it = steps().insert(it, {lvl[0], compute_level_action{node_and_action}});
      it = it + 1;
      
      for(auto node : node_and_action)
      {
        if(std::find(drivers.begin(), drivers.end(), node.first) == drivers.end())
        to_be_uncomputed.push_back(node);
      }
      it = steps().insert(it, {lvl[0], uncompute_level_action{to_be_uncomputed}});
      
    }
    }
    return true;
  }
};

} // namespace caterpillar