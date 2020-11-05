/*------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Giulia Meuli
*-----------------------------------------------------------------------------*/
#pragma once
#include "../structures/stg_gate.hpp"
#include "strategies/mapping_strategy.hpp"
#include "strategies/xag_mapping_strategy.hpp"

#include <array>
#include <cstdint>
#include <fmt/format.h>
#include <mockturtle/algorithms/cut_enumeration/spectr_cut.hpp>

#include <mockturtle/networks/xag.hpp>
#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/views/topo_view.hpp>
#include <stack>
#include <type_traits>
#include <fmt/format.h>
#include <variant>
#include <vector>

using Qubit = tweedledum::qubit_id;
using SetQubits = std::vector<Qubit>;

namespace caterpillar
{

struct xag_tracer_params
{
  /*! \brief Be verbose. */
  bool verbose{false};

  bool low_tdepth_AND{false};

};

struct xag_tracer_stats
{
  /*! \brief Number of CNOT gates. */
  uint32_t CNOT_count{0};

  /*! \brief Number of T gates. */
  uint32_t T_count{0};

  /*! \brief Number of T stages. */
  uint32_t T_depth{0};

  /*! \brief Number of qubits. */
  uint32_t qubit_count{0};

  /*! \brief Total runtime. */
  mockturtle::stopwatch<>::duration time_total{0};

  /*! \brief Required number of ancilla. */
  uint32_t required_ancillae{0u};

  /*! \brief output qubits. */
  std::vector<uint32_t> o_indexes;

  /*! \brief input qubits. */
  std::vector<uint32_t> i_indexes;

  void report() const
  {
    std::cout << fmt::format( "[i] total time = {:>5.2f} secs\n", mockturtle::to_seconds( time_total ) );
  }
};

namespace detail
{

template<class Ntk>
class xag_tracer_impl
{
  using node_t = typename Ntk::node;
  using signal_t = typename Ntk::signal;
public:
  xag_tracer_impl( Ntk const& ntk,
                                mapping_strategy<Ntk>& strategy,
                                xag_tracer_params const& ps,
                                xag_tracer_stats& st )
      : ntk( ntk ), strategy( strategy ), ps( ps ), st( st )
  {
  }

  bool run()
  {
    mockturtle::stopwatch t( st.time_total );
    prepare_inputs();
    prepare_constant( false );
    if ( ntk.get_node( ntk.get_constant( false ) ) != ntk.get_node( ntk.get_constant( true ) ) )
      prepare_constant( true );

    if ( const auto result = strategy.compute_steps( ntk ); !result )
    {
    
      std::cout << "[i] strategy could not be computed\n";
      return false;
    }

    strategy.foreach_step( [&]( auto node, auto action ) {
      std::visit(
          overloaded{
              []( auto ) {},
              [&]( compute_action const& action ) {
                const auto t = request_ancilla();
                node_to_qubit[node].push(t);
                if ( ps.verbose )
                {
                  fmt::print("[i] compute {} in qubit {}\n", node, t);
                  if ( action.leaves )   
                    fmt::print(" with leaves: {} \n", fmt::join(*action.leaves, " ")); 
                }
                if (action.leaves)
                {
                  compute_big_xor( t, *action.leaves );
                }
                else
                {
                  compute_node( node, t ); 
                }
              },
              [&]( uncompute_action const& action ) {
                const auto t = node_to_qubit[node].top();
                if ( ps.verbose )
                {  
                  fmt::print("[i] uncompute {} from qubit {}\n", node, t);
                  if ( action.leaves )   
                    fmt::print(" with leaves: {} \n", fmt::join(*action.leaves, " ")); 
                }
                if (action.leaves)
                {
                  compute_big_xor(  t, *action.leaves );
                }
                else
                {
                  compute_node( node, t );
                }
                release_ancilla( t );
              },
              [&]( compute_inplace_action const& action ) {
                const auto t = node_to_qubit[ntk.index_to_node( action.target_index )].top() ;
                node_to_qubit[node].push(t);

                if ( ps.verbose )
                {  
                  fmt::print("[i] compute {} inplace onto node {}\n", node , action.target_index);
                  if ( action.leaves )   
                    fmt::print(" with leaves: {} \n", fmt::join(*action.leaves, " ")); 
                }
                if (action.leaves)
                {
                  compute_big_xor(t, *action.leaves);
                }
                else
                {  
                  compute_node_inplace( node, t );
                }
              },
              [&]( uncompute_inplace_action const& action ) {
                const auto t = node_to_qubit[node].top();
                if ( ps.verbose )
                {
                  fmt::print("[i] uncompute {} inplace to {}\n", node , action.target_index);
                  if ( action.leaves )   
                    fmt::print(" with leaves: {} \n", fmt::join(*action.leaves, " ")); 
                }
                if (action.leaves)
                {
                  compute_big_xor(  t, *action.leaves );
                }
                else 
                {  
                  compute_node_inplace( node, t );
                }
              },
              [&] (buffer_action const& action){
                if(ps.verbose)
                {
                  fmt::print("[i] compute buffer from node {} to node {}\n", action.leaf, action.target);
                }
                node_to_qubit[action.target].push( node_to_qubit[action.leaf].top() );
              },
              [&] (compute_level_action& action){
                if(ps.verbose)
                {
                  fmt::print("[i] compute level with node {}\n", action.level[0].first);
                }

                compute_level_with_copies(action.level);
              },
              [&] (uncompute_level_action& action){
                if(!action.level.empty())
                {
                  if(ps.verbose)
                  {
                    fmt::print("[i] uncompute level with node {}\n", action.level[0].first);
                  }
                  uncompute_level(action.level);
                }
              }},
          action );
    } );

    prepare_outputs();
    st.T_depth = depths[std::max_element(depths.begin(), depths.end()) - depths.begin()];
    st.qubit_count = num_qubits;
    return true;
  }

private:

  void add_qubit()
  {
    num_qubits++;
    mask.push_back(0);
    depths.push_back(0);
  }

  void add_gate(tweedledum::gate_base op, Qubit t)
	{
		(void)op;
    (void)t;
	}

	void add_gate(const tweedledum::gate_base op, Qubit control, Qubit target)
  {
    assert(op.operation() == tweedledum::gate_set::cx);
    (void)op;

    auto const c = control.index();
    auto const t = target.index();
    assert(c < depths.size()); 
    assert(t < depths.size());

    st.CNOT_count++;
    depths[t] = std::max(depths[c], depths[t]);
  }

  void add_gate(const tweedledum::gate_base op, SetQubits controls, SetQubits target)
  {
    assert(op.operation() == tweedledum::gate_set::mcx);
    (void)op;
    assert(controls.size() == 2);

    auto const c1 = controls[0].index();
    auto const c2 = controls[1].index();
    auto const t = target[0].index();

    assert(c1 < depths.size());
    assert(c2 < depths.size());
    assert(t < depths.size());
    assert(t < mask.size());


    if (!mask[t])
    {
      depths[t] = ps.low_tdepth_AND ? 
                        std::max(std::max(depths[t] + 1, depths[c1]+1), depths[c2]+1) : 
                        std::max(std::max(depths[t] + 2, depths[c1]+1), depths[c2]+1);

      depths[c1] = depths[c1] + 1;
      depths[c2] = depths[c2] + 1;

      st.T_count = st.T_count + 4;

    }
    mask[t] = !(mask[t]);
  }

  void eval_tdepth()
  {
    
  }

  void prepare_inputs()
  {
    /* prepare primary inputs of logic network */
    ntk.foreach_pi( [&]( auto n ) {
      std::stack<uint32_t> sta;
      sta.push( num_qubits );
      node_to_qubit[n] = sta;
      st.i_indexes.push_back( node_to_qubit[n].top() );
      add_qubit();
    } );
    ntk.foreach_gate( [&]( auto n ) {
      std::stack<uint32_t> sta;
      node_to_qubit[n] = sta;
    } );
  }

  void prepare_constant( bool value )
  {
    const auto f = ntk.get_constant( value );
    const auto n = ntk.get_node( f );
    if ( ntk.fanout_size( n ) == 0 )
      return;
    const auto v = ntk.constant_value( n ) ^ ntk.is_complemented( f );
    node_to_qubit[n].push( num_qubits );
    add_qubit();
    if ( v )
      add_gate( tweedledum::gate::pauli_x, node_to_qubit[n].top() );
  }

  uint32_t request_ancilla()
  {
    if ( free_ancillae.empty() )
    {
      auto r = num_qubits;
      st.required_ancillae++;
      add_qubit();
      return r;
    }
    else
    {
      const auto r = free_ancillae.top();
      free_ancillae.pop();
      return r;
    }
  }

  void prepare_outputs()
  {
    std::unordered_map<node_t, signal_t> node_to_signals;
    ntk.foreach_po( [&]( auto s ) {
      auto node = ntk.get_node( s );

      if ( const auto it = node_to_signals.find( node ); it != node_to_signals.end() ) //node previously referred
      {
        auto new_i = request_ancilla();

        add_gate( tweedledum::gate::cx, node_to_qubit[ntk.node_to_index( node )].top(), new_i );
        if ( ntk.is_complemented( s ) != ntk.is_complemented( node_to_signals[node] ) )
        {
          add_gate( tweedledum::gate::pauli_x, new_i );
        }
        st.o_indexes.push_back( new_i );
      }
      else //node never referred
      {
        if ( ntk.is_complemented( s ) )
        {
          add_gate( tweedledum::gate::pauli_x, node_to_qubit[ntk.node_to_index( node )].top() );
        }
        node_to_signals[node] = s;
        st.o_indexes.push_back( node_to_qubit[ntk.node_to_index( node )].top() );
      }
    } );
  }

  void release_ancilla( uint32_t q )
  {
    free_ancillae.push( q );
  }

  template<int Fanin>
  std::array<uint32_t, Fanin> get_fanin_as_literals( node_t const& n )
  {
    std::array<uint32_t, Fanin> controls;
    ntk.foreach_fanin( n, [&]( auto const& f, auto i ) {
      controls[i] = ( ntk.node_to_index( ntk.get_node( f ) ) << 1 ) | ntk.is_complemented( f );
    } );
    return controls;
  }

  SetQubits get_fanin_as_qubits( node_t const& n )
  {
    SetQubits controls;
    ntk.foreach_fanin( n, [&]( auto const& f ) {
      assert( !ntk.is_complemented( f ) );
      controls.push_back( tweedledum::qubit_id( node_to_qubit[ntk.node_to_index( ntk.get_node( f ) )].top() ) );
    } );
    return controls;
  }

  void compute_big_xor( uint32_t const t, std::vector<uint32_t> const leaves)
  {
    for ( auto control : leaves )
    {
      auto c =  node_to_qubit[control].top();
      if (c != t)
      {     
        add_gate( tweedledum::gate::cx, tweedledum::qubit_id( c ), tweedledum::qubit_id( t ) );
      }
    }
  }


  void compute_node( node_t const& node, uint32_t t)
  {
    if ( ntk.is_and( node ) )
    {
      auto controls = get_fanin_as_literals<2>( node );
      auto node0 = ntk.index_to_node( controls[0] >> 1 );
      auto node1 = ntk.index_to_node( controls[1] >> 1 );

      SetQubits pol_controls;     
      pol_controls.emplace_back( node_to_qubit[node0].top(), controls[0] & 1 );
      pol_controls.emplace_back( node_to_qubit[node1].top(), controls[1] & 1 );
      
      compute_and( pol_controls, t );
      return;
    }
    if ( ntk.is_xor( node ) )
    {
      
      auto controls = get_fanin_as_literals<2>( node );
      compute_xor( node_to_qubit[ntk.index_to_node( controls[0] >> 1 )].top(),
                    node_to_qubit[ntk.index_to_node( controls[1] >> 1 )].top(),
                    ( controls[0] & 1 ) != ( controls[1] & 1 ), t );
      return;
    }
    if ( ntk.is_nary_xor( node ) )
    {
      
      std::vector<uint32_t> controls;
      controls.reserve( ntk.fanin_size(node) );
      ntk.foreach_fanin(node, [&] (auto f)
      {
        controls.push_back(ntk.get_node(f));
      });
      compute_big_xor( t, controls);
      return;
    }
  }


  void compute_node_inplace( node_t const& node, uint32_t t )
  {
 
    if ( ntk.is_xor( node ) )
    {
      auto controls = get_fanin_as_literals<2>( node );
      compute_xor_inplace( node_to_qubit[ntk.index_to_node( controls[0] >> 1 )].top(),
                            node_to_qubit[ntk.index_to_node( controls[1] >> 1 )].top(),
                            ( controls[0] & 1 ) != ( controls[1] & 1 ), t );
      return;
    }
    
  }

  void compute_and( SetQubits controls, uint32_t t )
  {
    add_gate( tweedledum::gate::mcx, controls, SetQubits{{t}} );
  }

  void compute_xor( uint32_t c1, uint32_t c2, bool inv, uint32_t t)
  {
    add_gate( tweedledum::gate::cx, tweedledum::qubit_id( c1 ), tweedledum::qubit_id( t ) );
    add_gate( tweedledum::gate::cx, tweedledum::qubit_id( c2 ), tweedledum::qubit_id( t ) );
    if ( inv )
      add_gate( tweedledum::gate::pauli_x, tweedledum::qubit_id( t ) );
  }

  void compute_xor_inplace( uint32_t c1, uint32_t c2, bool inv, uint32_t t )
  {

    if ( c1 == t && c2 != t)
    {
      add_gate( tweedledum::gate::cx, tweedledum::qubit_id( c2 ), c1 );
    }
    else if ( c2 == t && c1 !=t)
    {
      add_gate( tweedledum::gate::cx, tweedledum::qubit_id( c1 ), c2 );
    }
    else if (c1 != t && c2!= t && c1 != c2)
    {
      //std::cerr << "[e] target does not match any control in in-place\n";
      add_gate( tweedledum::gate::cx, tweedledum::qubit_id( c1 ), t );
      add_gate( tweedledum::gate::cx, tweedledum::qubit_id( c2 ), t );
    }
    if ( inv )
      add_gate( tweedledum::gate::pauli_x, t );
  }

  /*  
      For each node in the level inserts copies and returns the two 
      qubits where the roots of each node are stored.             
  */
  void compute_copies ( std::map<node_t, std::vector<uint32_t>>& id_to_tcp,  caterpillar::level_info_t const& level)
  {
    for(auto node : level)
    { 
      auto &id = node.first;
      if(ntk.is_nary_xor(id))
      {
        std::vector<uint32_t> fanins;
        ntk.foreach_fanin(id, [&] (auto f)
        {
          fanins.push_back(ntk.get_node(f));
        });
        id_to_tcp[id] = fanins;
        continue;
      }     
      std::vector<uint32_t> roots_targets (2);
      for(auto i = 0; i < 2 ; i++)
      {
        auto &cone = node.second[i];
        if(cone.copies.empty())
        {
          if(cone.leaves.size() == 1)
          {
            roots_targets[i] = node_to_qubit[cone.leaves[0]].top();
            continue;
          }
          else
          {
            roots_targets[i] = cone.target.empty() ? request_ancilla() : node_to_qubit[cone.target[0]].top();
          }
        }
        else
        {
          auto tcp = request_ancilla();
          compute_big_xor(tcp, cone.copies);
          roots_targets[i] = tcp;
        }
      }
      id_to_tcp[id] = roots_targets;
    }
  }

  void remove_copies( std::map<node_t, std::vector<uint32_t>>& id_to_tcp, caterpillar::level_info_t  const&  level )
  {
    for(auto node : level)
    {
      if(ntk.is_nary_xor(node.first)) continue;

      for(auto i = 0; i < 2 ; i++)
      {
        auto cone = node.second[i];
        if(cone.copies.empty()) continue;

        auto tcp = id_to_tcp[node.first][i];
        
        for (auto c : cone.copies)
        {
          add_gate(tweedledum::gate::cx, tweedledum::qubit_id( node_to_qubit[c].top() ), tcp );
        }
        release_ancilla(tcp);
      }
    }
  }

  void compute_and_xor_from_controls(uint32_t id, SetQubits const& pol_controls, uint32_t target)
  {
    if (ntk.is_and(id))
    {
      compute_and(pol_controls, target );
    }
    else if (ntk.is_xor(id))
    {
      compute_xor(pol_controls[0].index(), pol_controls[1].index(), 
                pol_controls[0].is_complemented() != pol_controls[1].is_complemented(), 
                target );
    }
  }

  
  void compute_level_with_copies(caterpillar::level_info_t const& level)
  {      

    std::map<node_t, std::vector<uint32_t>> id_to_tcp;
    compute_copies(id_to_tcp, level);
    std::vector<uint32_t> qubit_offset;

    for(auto node : level)
    {      
      auto &id = node.first;
      auto target = request_ancilla();

      /* nary xor nodes directly point to AND nodes */
      if(ntk.is_nary_xor(id))
      {
        compute_node(id, target);
        node_to_qubit[id].push(target);
        continue;
      }

      SetQubits pol_controls; 
      for(auto i = 0; i < 2 ; i++)
      {
        auto &cone = node.second[i];
        auto &tcp = id_to_tcp[id][i];
        pol_controls.emplace_back(tcp, cone.complemented);

        std::vector<uint32_t> rem_leaves;
        std::set_symmetric_difference( 
                  cone.leaves.begin(), cone.leaves.end(), cone.copies.begin(), cone.copies.end(), 
                  std::back_inserter(rem_leaves) );
        
        compute_big_xor(tcp, rem_leaves);
        node_to_qubit[cone.root].push(tcp);
      }


      //  automatically take into account the extra qubit needed 
      //  for the AND implementation with T-depth = 1
      if (ntk.is_and(id))
      { 
        if (ps.low_tdepth_AND)
        {
          qubit_offset.push_back(request_ancilla());
        }
      }

      compute_and_xor_from_controls(id, pol_controls, target);
      node_to_qubit[id].push(target);

      for(auto i = 1; i >= 0 ; i--)
      {
        auto& cone = node.second[i];
        auto& tcp = id_to_tcp[id][i];

        std::vector<uint32_t> rem_leaves;
        std::set_symmetric_difference( 
                  cone.leaves.begin(), cone.leaves.end(), cone.copies.begin(), cone.copies.end(), 
                  std::back_inserter(rem_leaves) );
        

        compute_big_xor(tcp, rem_leaves);
      }
      node_to_qubit[node.second[0].root].pop();
      node_to_qubit[node.second[1].root].pop();

    }
    assert(qubit_offset.size() <= level.size());
    
    for (auto q : qubit_offset)
      release_ancilla(q);

    remove_copies(id_to_tcp, level);
  }

  void uncompute_level(caterpillar::level_info_t const& level)
  {
    //reverse the orther of the cones
    // only AND nodes are uncomputed
    for(int n = level.size()-1; n >=0; n--)
    {
      SetQubits pol_controls; 

      auto &id = level[n].first;
      
      for(auto i = 0; i < 2 ; i++)
      {
        auto &cone = level[n].second[i];
        
        if(cone.leaves.size() == 1)
        {
          pol_controls.emplace_back(node_to_qubit[cone.leaves[0]].top(), cone.complemented);
          continue;
        } 

        auto t = cone.target.empty() ? request_ancilla() : node_to_qubit[cone.target[0]].top();
        pol_controls.emplace_back(t, cone.complemented);

        compute_big_xor(t, cone.leaves);
        node_to_qubit[cone.root].push(t);
      }

      auto target = node_to_qubit[id].top();
      compute_and_xor_from_controls(id, pol_controls, target);
      node_to_qubit[id].pop();

      for(int i = 1; i >= 0 ; i--)
      {
        auto &cone = level[n].second[i];
        if(cone.leaves.size() == 1) continue;

        auto t = node_to_qubit[cone.root].top();

        compute_big_xor(t, cone.leaves);
        node_to_qubit[cone.root].pop();
      }
    }

  }


private:
  Ntk const& ntk;
  mapping_strategy<Ntk>& strategy;

  xag_tracer_params const& ps;
  xag_tracer_stats& st;

  std::vector<int> depths;
  std::vector<bool> mask;
  int num_qubits = 0;

  std::unordered_map<uint32_t, std::stack<uint32_t>> node_to_qubit;
  std::stack<uint32_t> free_ancillae;
 
}; // namespace detail

} // namespace detail

/*! \brief Hierarchical synthesis based on a logic network
 *
 * This algorithm used hierarchical synthesis and computes a reversible network
 * for each gate in the circuit and computes the intermediate result to an
 * ancilla line.  The node may be computed out-of-place or in-place.  The
 * order in which nodes are computed and uncomputed, and whether they are
 * computed out-of-place or in-place is determined by a separate mapper
 * component `MappingStrategy` that is passed as template parameter to the
 * function.
 */
template<class Ntk>
static bool xag_tracer(  Ntk const& ntk,
                              mapping_strategy<Ntk>& strategy,
                              xag_tracer_params const& ps = {},
                              xag_tracer_stats* pst = nullptr )
{

  xag_tracer_stats st;
  if constexpr (std::is_same_v<decltype(strategy), xag_low_depth_mapping_strategy>){ assert( ps.low_tdepth_AND ); }
  if constexpr (std::is_same_v<decltype(strategy), xag_mapping_strategy>)          { assert( !ps.low_tdepth_AND ); }
  if constexpr (std::is_same_v<decltype(strategy), xag_fast_lowt_mapping_strategy>){ assert( !ps.low_tdepth_AND ); }
  #ifdef USE_Z3
  if constexpr (std::is_same_v<decltype(strategy), xag_pebbling_mapping_strategy>) { assert( !ps.low_tdepth_AND ); }
  #endif
  detail::xag_tracer_impl<Ntk> impl( ntk, strategy, ps, st);
                                                                                                                                                                                     
  const auto result = impl.run();
  if ( ps.verbose )
  {
    st.report();
  }

  if ( pst )
  {
    *pst = st;
  }

  return result;
}

} /* namespace caterpillar */
