/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Giulia Meuli
*------------------------------------------------------------------------------------------------*/
#pragma once

#include "gray_synth.hpp"
#include "lin_comb_synth.hpp"
#include "optimization_graph.hpp"
#include "post_opt_esop.hpp"

#include <cstdint>
#include <functional>
#include <iostream>
#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/esop.hpp>
#include <kitty/operations.hpp>
#include <kitty/print.hpp>
#include <kitty/spectral.hpp>
#include <vector>

namespace caterpillar
{
struct stg_from_esop {
	using esop_synthesis_fn_t
	    = std::function<std::vector<kitty::cube>(kitty::dynamic_truth_table const&)>;
	stg_from_esop(esop_synthesis_fn_t esop_synthesis, bool optimize_esop = false)
	    : esop_synthesis_(esop_synthesis)
	    , optimize_esop_(optimize_esop)
	{}

	template<class Network>
	void operator()(Network& network, kitty::dynamic_truth_table const& function,
	                std::vector<uint32_t> const& qubit_map)
	{
		const auto num_controls = function.num_vars();
		assert(qubit_map.size() == static_cast<std::size_t>(num_controls) + 1u);

		std::vector<uint32_t> target = {qubit_map.back()};
		const auto cubes = esop_synthesis_(function);

		optimized_esop opt_esop;
		if (optimize_esop_) {
			opt_esop = match_pairing(cubes);
		} else {
			opt_esop.cubes = cubes;
		}

		opt_stg_from_esop(network, opt_esop, qubit_map);
	}

private:
	esop_synthesis_fn_t esop_synthesis_;
	bool optimize_esop_{false};
};

}//namespace caterpillar