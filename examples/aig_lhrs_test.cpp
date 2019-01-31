#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/views/mapping_view.hpp>
#include <mockturtle/algorithms/lut_mapping.hpp>
#include <mockturtle/algorithms/collapse_mapped.hpp>
#include <mockturtle/io/aiger_reader.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <mockturtle/algorithms/lut_mapping.hpp>

#include <caterpillar/lhrs.hpp>
#include <caterpillar/optimization_graph.hpp>
#include <caterpillar/stg_to_mcx.hpp>

#include <tweedledum/algorithms/synthesis/stg.hpp>
#include <tweedledum/gates/mcmt_gate.hpp>
#include <tweedledum/networks/netlist.hpp>
#include <tweedledum/algorithms/decomposition/dt.hpp>


#include <lorina/aiger.hpp>

#include <vector>


struct stats
{
  mockturtle::stopwatch<>::duration total_time{0};
  uint32_t num_gates{0};
  uint32_t num_qubits{0};
  uint32_t T_count{0};

  void report( std::string const& name ) const
  {
    std::cout << fmt::format( "[i] strategy = {:16} qubits = {:8d} gates = {:8d} T-gates = {:8d} ({:>8.2f} secs)\n",
                              name,
                              num_qubits,
                              num_gates,
                              T_count,
                              mockturtle::to_seconds( total_time ) );
  }
}; /* stats */

template<typename SynthesisFn>
void run_lhrs_experiment (std::string const& name, SynthesisFn const& syn_fn, std::string const& method)
{
	using Q_Net = tweedledum::netlist<tweedledum::mcmt_gate>;

	stats st;

	mockturtle::lut_mapping_params ps;
	ps.cut_enumeration_ps.cut_size = 4;

	{
    auto count_T_gates = [&]( Q_Net const& netlist ){
      auto T_number = 0u;
      netlist.foreach_cgate( [&]( const auto& gate ){
          if ( gate.gate.operation() == tweedledum::gate_set::t )
          {
            ++T_number;
          }
        });
      return T_number;
    };
		
		auto est_T_gates = [&]( Q_Net const& netlist ){
      auto T_number = 0u;
      netlist.foreach_cgate( [&]( const auto& gate ){
          T_number += caterpillar::detail::t_cost(gate.gate.num_controls(), netlist.size());
        });
      return T_number;
    };
    mockturtle::stopwatch t( st.total_time );

		mockturtle::aig_network aig;
		lorina::read_aiger( "../epfl_bench/"+name+".aig" , mockturtle::aiger_reader( aig ) );

		mockturtle::mapping_view<mockturtle::aig_network, true> mapped_aig{ aig };

		mockturtle::lut_mapping<mockturtle::mapping_view<mockturtle::aig_network, true>, true>(mapped_aig, ps);

		auto lutn = mockturtle::collapse_mapped_network<mockturtle::klut_network>( mapped_aig );

    assert(lutn);

		Q_Net rev_net;
		
		caterpillar::logic_network_synthesis( rev_net, *lutn, syn_fn);    

		//auto const q_circ = tweedledum::relative_phase_mapping<Q_Net>( rev_net );
		auto& q_circ = rev_net;

		st.num_gates = q_circ.num_gates();
		st.num_qubits = q_circ.num_qubits();
		st.T_count = est_T_gates( q_circ );
  }
  st.report( method );
	
}

int main ()
{
	std::string const BOLD = "\033[1m";
  std::string const UNDERLINE = "\033[4m";
  std::string const ENDC = "\033[0m";

	std::vector<std::string> epfl_bench {
		"adder",
		"bar",
		"divisor",
		"hypotenuse",
		"log2",
		"max",
		"multiplier",
		"sin",
		"sqrt",
		"square" ,
	};
	
	for (auto name : epfl_bench)
	{
		std::cout << BOLD << "[i] benchmark: " << UNDERLINE << name << ENDC << std::endl;

		run_lhrs_experiment( name, tweedledum::stg_from_pprm(), "PPRM" );
		run_lhrs_experiment( name, tweedledum::stg_from_pkrm(), "PKRM" );
		run_lhrs_experiment( name, caterpillar::stg_from_exact_synthesis(), "EXACT(unit)" );

	}
	
	return 0;

}


