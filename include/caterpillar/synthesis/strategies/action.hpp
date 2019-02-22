/*------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Giulia Meuli
*-----------------------------------------------------------------------------*/
#pragma once

#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

#include <kitty/dynamic_truth_table.hpp>

namespace caterpillar
{

struct compute_action
{
  /*! \brief Override node as cell.
   *
   * If a value is assigned to this variable, instead of using the gate
   * associated to the computed node, the cell with respective truth table
   * and leaves is considered.
   */
  std::optional<std::pair<kitty::dynamic_truth_table, std::vector<uint32_t>>> cell_override;
};

struct uncompute_action
{
  /*! \brief Override node as cell.
   *
   * If a value is assigned to this variable, instead of using the gate
   * associated to the uncomputed node, the cell with respective truth table
   * and leaves is considered.
   */
  std::optional<std::pair<kitty::dynamic_truth_table, std::vector<uint32_t>>> cell_override;
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
