/*------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Mathias Soeken
*-----------------------------------------------------------------------------*/

#pragma once

#include <unordered_set>

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/node_map.hpp>
#include <mockturtle/views/topo_view.hpp>

#include "mapping_strategy.hpp"

namespace caterpillar
{

namespace mt = mockturtle;

/*! \brief Eager mapping strategy.
 *
 * This strategy computes each node in topological order.  At each primary
 * output, all nodes in the transitive fanin are uncomputed that are not
 * required any longer in successive steps.
 *
 * This strategy only finds compute and uncompute steps, but no inplace steps.
 */
template<class LogicNetwork>
class eager_mapping_strategy : public mapping_strategy<LogicNetwork>
{
public:
  using base_t = mapping_strategy<LogicNetwork>;

  /*! \brief Default constructor.
   *
   * \param ntk Logic network
   * \param ps  Mapping strategy parameters
   */
  eager_mapping_strategy( LogicNetwork const& ntk, mapping_strategy_params const& ps = {} )
      : mapping_strategy<LogicNetwork>( ntk, ps ),
        _ref_counts( ntk, 0u )
  {
    static_assert( mt::is_network_type_v<LogicNetwork>, "LogicNetwork is not a network type" );
    static_assert( mt::has_is_constant_v<LogicNetwork>, "LogicNetwork does not implement the is_constant method" );
    static_assert( mt::has_is_pi_v<LogicNetwork>, "LogicNetwork does not implement the is_pi method" );
    static_assert( mt::has_fanout_size_v<LogicNetwork>, "LogicNetwork does not implement the fanout_size method" );
    static_assert( mt::has_foreach_node_v<LogicNetwork>, "LogicNetwork does not implement the foreach_node method" );
    static_assert( mt::has_foreach_po_v<LogicNetwork>, "LogicNetwork does not implement the foreach_po method" );
    static_assert( mt::has_foreach_fanin_v<LogicNetwork>, "LogicNetwork does not implement the foreach_fanin method" );
    static_assert( mt::has_get_node_v<LogicNetwork>, "LogicNetwork does not implement the get_node method" );

    init_refs();
    run();
  }

  /*! \brief Iterate over mapping steps. */
  virtual bool foreach_step( typename base_t::step_function_t const& fn ) const override
  {
    for ( auto const& [n, a] : _steps )
    {
      fn( n, a );
    }

    return true;
  }

private:
  /* compute reference counters */
  void init_refs()
  {
    this->_ntk.foreach_node( [&]( auto n ) {
      _ref_counts[n] = this->_ntk.fanout_size( n );
    } );
    this->_ntk.foreach_po( [&]( auto f ) {
      _pos.insert( this->_ntk.get_node( f ) );
    } );
  }

  void run()
  {
    mt::topo_view<LogicNetwork> topo{this->_ntk};

    topo.foreach_node( [&]( auto n ) {
      if ( this->_ntk.is_constant( n ) || this->_ntk.is_pi( n ) )
        return true;

      _steps.emplace_back( n, compute_action{} );
      if ( _pos.count( n ) )
      {
        uncompute_eagerly( n );
      }

      return true;
    } );
  }

  void uncompute_eagerly( mt::node<LogicNetwork> n )
  {
    if ( this->_ntk.is_constant( n ) || this->_ntk.is_pi( n ) )
      return;

    this->_ntk.foreach_fanin( n, [&]( auto const& f ) {
      const auto child = this->_ntk.get_node( f );
      if ( this->_ntk.is_constant( child ) || this->_ntk.is_pi( child ) )
        return;

      if ( --_ref_counts[f] == 0u )
      {
        _steps.emplace_back( child, uncompute_action{} );
        uncompute_eagerly( child );
      }
    } );
  }

private:
  mt::node_map<uint32_t, LogicNetwork> _ref_counts;
  std::unordered_set<mt::node<LogicNetwork>> _pos;

  std::vector<std::pair<mt::node<LogicNetwork>, mapping_strategy_action>> _steps;
};

} // namespace caterpillar
