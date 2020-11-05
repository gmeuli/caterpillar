/* caterpillar: C++ logic network library
 * Copyright (C) 2018-2019  EPFL
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*!
  \file caterpillar.hpp
  \brief Main header file for caterillar

  \author Giulia Meuli
*/

#pragma once

#include "caterpillar/details/utils.hpp"
#include "caterpillar/optimization/optimization_graph.hpp"
#include "caterpillar/optimization/post_opt_esop.hpp"
#include "caterpillar/solvers/bsat_solver.hpp"
#include "caterpillar/solvers/z3_solver.hpp"
#include "caterpillar/solvers/z3_inplace_solver.hpp"
#include "caterpillar/structures/stg_gate.hpp"
#include "caterpillar/structures/abstract_network.hpp"
#include "caterpillar/structures/pebbling_view.hpp"
#include "caterpillar/synthesis/lhrs.hpp"
#include "caterpillar/synthesis/satbased_cnotrz.hpp"
#include "caterpillar/synthesis/stg_to_mcx.hpp"
#include "caterpillar/synthesis/strategies/action.hpp"
#include "caterpillar/synthesis/strategies/bennett_mapping_strategy.hpp"
#include "caterpillar/synthesis/strategies/best_fit_mapping_strategy.hpp"
#include "caterpillar/synthesis/strategies/eager_mapping_strategy.hpp"
#include "caterpillar/synthesis/strategies/mapping_strategy.hpp"
#include "caterpillar/synthesis/strategies/pebbling_mapping_strategy.hpp"
#include "caterpillar/synthesis/strategies/xag_mapping_strategy.hpp"
#include "caterpillar/verification/circuit_to_logic_network.hpp"
