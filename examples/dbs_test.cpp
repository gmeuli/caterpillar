#include <tweedledum/algorithms/decomposition/dt.hpp>
#include <tweedledum/algorithms/synthesis/dbs.hpp>
#include <tweedledum/algorithms/synthesis/stg.hpp>
#include <tweedledum/gates/mcmt_gate.hpp>
#include <mockturtle/utils/stopwatch.hpp>
#include <vector>
#include <map>
#include <caterpillar/stg_to_mcx.hpp>

using namespace tweedledum;
using namespace mockturtle;
using namespace caterpillar;
struct stats
{
  stopwatch<>::duration total_time{0};
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
                              to_seconds( total_time ) );
  }
}; /* stats */

template<typename Ntk, typename SynthesisFn>
void run_dbs_with_strategy( std::vector<uint16_t> perm, SynthesisFn const& synth_fn, std::string const& name )
{
  stats st;
  {
    auto count_T_gates = [&]( Ntk const& netlist ){
      auto T_number = 0u;
      netlist.foreach_cgate( [&]( const auto& gate ){
          if ( gate.gate.operation() == tweedledum::gate::t )
          {
            ++T_number;
          }
        });
      return T_number;
    };

    stopwatch t( st.total_time );
    auto const stg_circ = tweedledum::dbs<Ntk>( perm, synth_fn );
    auto const q_circ = tweedledum::dt_decomposition<Ntk>( stg_circ );

    st.num_gates = q_circ.num_gates();
    st.num_qubits = q_circ.num_qubits();
    st.T_count = count_T_gates( q_circ );
  }
  st.report( name );
}

int main( int argc, char** argv )
{
  std::string const BOLD = "\033[1m";
  std::string const UNDERLINE = "\033[4m";
  std::string const ENDC = "\033[0m";

  /* cost functions */
  auto lit_cost = []( kitty::cube const& cube ){ return cube.num_literals() + 1; };
  auto complex_cost = []( kitty::cube const& cube ){ return 1 + 100*( cube.num_literals() > 1 ); };

  /* prime */
  std::vector<uint16_t> prime3{{0, 2, 3, 5, 7, 1, 4, 6}};
  std::vector<uint16_t> prime4{{0, 2, 3, 5, 7, 11, 13, 1, 4, 6, 8, 9, 10, 12, 14, 15}};
  std::vector<uint16_t> prime5{{0, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 1, 4, 6, 8, 9, 10, 12, 14, 15, 16, 18, 20, 21, 22, 24, 25, 26, 27, 28, 30}};
  std::vector<uint16_t> prime6{{0, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 1, 4, 6, 8, 9, 10, 12, 14, 15, 16, 18, 20, 21, 22, 24, 25, 26, 27, 28, 30, 32, 33, 34, 35, 36, 38, 39, 40, 42, 44, 45, 46, 48, 49, 50, 51, 52, 54, 55, 56, 57, 58, 60, 62, 63}};

  /* tof */
  std::vector<uint16_t> tof3{{0, 1, 2, 3, 4, 5, 7, 6}};
  std::vector<uint16_t> tof4{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 14}};
  std::vector<uint16_t> tof5{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 31, 30}};
  std::vector<uint16_t> tof6{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 63, 62}};

  /* hwb */
  std::vector<uint16_t> hwb4{{0, 2, 4, 12, 8, 5, 9, 11, 1, 6, 10, 13, 3, 14, 7, 15}};
  std::vector<uint16_t> hwb5{{0, 2, 4, 12, 8, 20, 24, 25, 16, 5, 9, 26, 17, 11, 19, 23, 1, 6, 10, 28, 18, 13, 21, 27, 3, 14, 22, 29, 7, 30, 15, 31}};

  std::map<std::string, std::vector<uint16_t>> benchmarks;
  benchmarks["prime3"] = prime3;
  benchmarks["prime4"] = prime4;
  benchmarks["prime5"] = prime5;
  benchmarks["prime6"] = prime6;

  benchmarks["tof3"] = tof3;
  benchmarks["tof4"] = tof4;
  benchmarks["tof5"] = tof5;
  benchmarks["tof6"] = tof6;

  benchmarks["hwb4"] = hwb4;
  benchmarks["hwb5"] = hwb5;

  using netlist_t = netlist<mcmt_gate>;

  for( auto& [key, val] : benchmarks )
  {
    std::cout << BOLD << "[i] benchmark: " << UNDERLINE << key << ENDC << std::endl;

    run_dbs_with_strategy<netlist_t>( val, tweedledum::stg_from_pprm(), "PPRM" );
    run_dbs_with_strategy<netlist_t>( val, tweedledum::stg_from_pkrm(), "PKRM" );
    run_dbs_with_strategy<netlist_t>( val, stg_from_exact_synthesis(), "EXACT(unit)" );

    if ( key == "prime6" )
      continue;

    run_dbs_with_strategy<netlist_t>( val, stg_from_exact_synthesis( lit_cost ), "EXACT(lit)" );
    run_dbs_with_strategy<netlist_t>( val, stg_from_exact_synthesis( complex_cost ), "EXACT(complex)" );
  }

  return 0;
}
