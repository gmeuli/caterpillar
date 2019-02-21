/*------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Mathias Soeken and Giulia Meuli
*-----------------------------------------------------------------------------*/

#pragma once

#include <cstdint>
#include <functional>

#include <mockturtle/traits.hpp>

#include "action.hpp"

namespace caterpillar
{

struct mapping_strategy_params
{
  /*! \brief Show progress bar. */
  bool progress{false};

  /*! \brief Maximum number of pebbles to use, if supported by mapping strategy (0 means no limit). */
  uint32_t pebble_limit{0u};

  /*! \brief Conflict limit for the SAT solver (0 means no limit). */
  uint32_t conflict_limit{0u};

  /*! \brief Increment pebble numbers, if timeout. */
  bool increment_on_timeout{false};

  /*! \brief Decrement pebble numbers, if satisfiable. */
  bool decrement_on_success{false};
};

template<class LogicNetwork>
class mapping_strategy
{
public:
  using step_function_t = std::function<void(mockturtle::node<LogicNetwork> const&, mapping_strategy_action const&)>;

  mapping_strategy( LogicNetwork const& ntk, mapping_strategy_params const& ps = {} )
      : _ntk( ntk ),
        _ps( ps )
  {
  }

  virtual bool foreach_step( step_function_t const& fn ) const = 0;

protected:
  LogicNetwork const& _ntk;
  mapping_strategy_params _ps;
};

} // namespace caterpillar
