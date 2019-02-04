/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Giulia Meuli
*------------------------------------------------------------------------------------------------*/
#pragma once



#include <caterpillar/optimization_graph.hpp>
#include <caterpillar/post_opt_esop.hpp>

#include <cstdint>
#include <functional>
#include <iostream>
#include <vector>

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/esop.hpp>
#include <kitty/operations.hpp>
#include <kitty/print.hpp>
#include <kitty/spectral.hpp>

#include <tweedledum/algorithms/synthesis/gray_synth.hpp>
#include <tweedledum/networks/netlist.hpp>

#include <easy/esop/constructors.hpp>


namespace caterpillar
{

namespace td = tweedledum;

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

struct stg_from_exact_synthesis
{
public:
  explicit stg_from_exact_synthesis( std::function<int(kitty::cube)> const& cost_fn = []( kitty::cube const& cube ){ return 1; } )
    : cost_fn( cost_fn )
  {
  }

  template<class Network>
  void operator()( Network& net,  std::vector<tweedledum::qubit_id> const& qubit_map , kitty::dynamic_truth_table const& function) const
  {
    const auto num_controls = function.num_vars();
    assert( qubit_map.size() == std::size_t( num_controls ) + 1u );

    /* synthesize ESOP */
    easy::esop::helliwell_maxsat_statistics stats;
    easy::esop::helliwell_maxsat_params ps;
    auto const& esop = easy::esop::esop_from_tt<kitty::dynamic_truth_table, easy::sat2::maxsat_rc2, easy::esop::helliwell_maxsat>( stats, ps ).synthesize( function, cost_fn );

    std::vector<tweedledum::qubit_id> target = {qubit_map.back()};
    for (auto const& cube : esop )
    {
      std::vector<tweedledum::qubit_id> controls, negations;
      auto bits = cube._bits;
      auto mask = cube._mask;
      for (auto v = 0; v < num_controls; ++v) {
        if (mask & 1) {
          controls.push_back(tweedledum::qubit_id(qubit_map[v]) );
          if (!(bits & 1)) {
            negations.push_back(tweedledum::qubit_id(qubit_map[v]) );
          }
        }
        bits >>= 1;
        mask >>= 1;
      }
      for (auto n : negations) {
        net.add_gate(td::gate::cx, n);
      }
      net.add_gate(td::gate::mcx, controls, target);
      for (auto n : negations) {
        net.add_gate(td::gate::cx, n);
      }
    }
  }

protected:
  std::function<int(kitty::cube)> const& cost_fn;  
};

}//namespace caterpillar