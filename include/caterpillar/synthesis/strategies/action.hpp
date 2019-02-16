/*------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Giulia Meuli
*-----------------------------------------------------------------------------*/
#pragma once

#include <cstdint>
#include <variant>

namespace caterpillar
{

struct compute_action
{
};

struct uncompute_action
{
};

struct compute_inplace_action
{
  uint32_t target_index;
};

struct uncompute_inplace_action
{
  uint32_t target_index;
};

using mapping_strategy_action = std::variant<compute_action, uncompute_action, compute_inplace_action, uncompute_inplace_action>;

}
