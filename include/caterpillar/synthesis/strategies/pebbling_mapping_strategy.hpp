/*------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Giulia Meuli
*-----------------------------------------------------------------------------*/

#pragma once

#include <algorithm>
#include <cstdint>
#include <vector>

#include "mapping_strategy.hpp"
#include <caterpillar/solvers/solver_manager.hpp>
#include <caterpillar/solvers/z3_solver.hpp>

namespace caterpillar
{

namespace mt = mockturtle;


/*!
  \verbatim embed:rst
  The pebbling strategy is obtained by solving iteratively the reversible pebbling game on the given network.
  The problem is encoded as a SAT problem. Details can be found in :cite:`MS19`.
  \endverbatim
*/
template<class LogicNetwork, class Solver>
class pebbling_mapping_strategy : public mapping_strategy<LogicNetwork>
{
public:
  pebbling_mapping_strategy( pebbling_mapping_strategy_params const& ps = {} )
    : ps( ps )
  {
    static_assert( mt::is_network_type_v<LogicNetwork>, "LogicNetwork is not a network type" );
    static_assert( mt::has_is_pi_v<LogicNetwork>, "LogicNetwork does not implement the is_pi method" );
    static_assert( mt::has_foreach_fanin_v<LogicNetwork>, "LogicNetwork does not implement the foreach_fanin method" );
    static_assert( mt::has_foreach_gate_v<LogicNetwork>, "LogicNetwork does not implement the foreach_gate method" );
    static_assert( mt::has_num_gates_v<LogicNetwork>, "LogicNetwork does not implement the num_gates method" );
    static_assert( mt::has_foreach_po_v<LogicNetwork>, "LogicNetwork does not implement the foreach_po method" );
    static_assert( mt::has_index_to_node_v<LogicNetwork>, "LogicNetwork does not implement the index_to_node method" );
  }

  bool compute_steps( LogicNetwork const& ntk ) override
  {
    
    this->steps() = pebble<Solver, LogicNetwork> (ntk, ps);

    if ( this->steps().empty() )
      return false;

    return true;
    
  }

private:
  pebbling_mapping_strategy_params ps;
};

#ifdef USE_Z3
template<class LogicNetwork>
class weighted_pebbling_mapping_strategy : public mapping_strategy<LogicNetwork>
{
public:
  weighted_pebbling_mapping_strategy( pebbling_mapping_strategy_params const& ps = {} )
    : ps( ps )
  {
    static_assert( has_get_weight_v<LogicNetwork>, "LogicNetwork does not implement the get_weight method");
    static_assert( mt::is_network_type_v<LogicNetwork>, "LogicNetwork is not a network type" );
    static_assert( mt::has_is_pi_v<LogicNetwork>, "LogicNetwork does not implement the is_pi method" );
    static_assert( mt::has_foreach_fanin_v<LogicNetwork>, "LogicNetwork does not implement the foreach_fanin method" );
    static_assert( mt::has_foreach_gate_v<LogicNetwork>, "LogicNetwork does not implement the foreach_gate method" );
    static_assert( mt::has_num_gates_v<LogicNetwork>, "LogicNetwork does not implement the num_gates method" );
    static_assert( mt::has_foreach_po_v<LogicNetwork>, "LogicNetwork does not implement the foreach_po method" );
    static_assert( mt::has_index_to_node_v<LogicNetwork>, "LogicNetwork does not implement the index_to_node method" );
  }

  bool compute_steps( LogicNetwork const& ntk ) override
  {
    using Solver = z3_pebble_solver<LogicNetwork>;

    this->steps() = pebble<Solver, LogicNetwork> (ntk, ps);

    if ( this->steps().empty() )
      return false;

    return true;
  }

private:
  pebbling_mapping_strategy_params ps;
};

#endif
}
