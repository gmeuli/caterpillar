#include <tweedledum/algorithms/mapping/relative_phase.hpp>
#include <tweedledum/algorithms/synthesis/decomposition_based.hpp>
#include <tweedledum/algorithms/synthesis/single_target_gates.hpp>
#include <tweedledum/gates/mcmt_gate.hpp>
#include <vector>
#include <map>

using namespace tweedledum;

int main( int argc, char** argv )
{
  /* prime */
  std::vector<uint16_t> prime3{{0, 2, 3, 5, 7, 1, 4, 6}};
  std::vector<uint16_t> prime4{{0, 2, 3, 5, 7, 11, 13, 1, 4, 6, 8, 9, 10, 12, 14, 15}};
  std::vector<uint16_t> prime5{{0, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 1, 4, 6, 8, 9, 10, 12, 14, 15, 16, 18, 20, 21, 22, 24, 25, 26, 27, 28, 30}};
  std::vector<uint16_t> prime6{{0, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 1, 4, 6, 8, 9, 10, 12, 14, 15, 16, 18, 20, 21, 22, 24, 25, 26, 27, 28, 30, 32, 33, 34, 35, 36, 38, 39, 40, 42, 44, 45, 46, 48, 49, 50, 51, 52, 54, 55, 56, 57, 58, 60, 62, 63}};

  /* tof */
  std::vector<uint16_t> tof{{0, 1, 2, 3, 4, 5, 7, 6}};
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

  benchmarks["tof"] = tof;
  benchmarks["tof4"] = tof4;
  benchmarks["tof5"] = tof5;
  benchmarks["tof6"] = tof6;

  benchmarks["hwb4"] = hwb4;
  benchmarks["hwb5"] = hwb5;

  using netlist_t = netlist<mcmt_gate>;

  auto count_T_gates = [&]( netlist_t const& netlist ){
    auto T_number = 0u;
    netlist.foreach_gate( [&]( const auto& gate ){
        if ( gate.gate.kind() == tweedledum::gate_kinds_t::t )
        {
          ++T_number;
        }
      });
    return T_number;
  };

  for( auto& [key, val] : benchmarks )
  {
    std::cout << "[i] benchmark: " + key << std::endl;

    { /* PPRM */
      auto val_copy = val;
      const auto stg_circ = tweedledum::decomposition_based_synthesis<netlist_t>( val_copy, tweedledum::stg_from_pprm() );
      const auto q_circ = tweedledum::relative_phase_mapping<netlist_t>( stg_circ );

      std::cout << fmt::format( "[i] strategy = {} qubits = {:8d} gates = {:8d} T-gates = {:8d}\n",
                                "PPRM",
                                q_circ.num_qubits(),
                                q_circ.num_gates(),
                                count_T_gates( q_circ ) );
    }

    { /* PKRM */
      auto val_copy = val;
      const auto stg_circ = tweedledum::decomposition_based_synthesis<netlist_t>( val_copy, tweedledum::stg_from_pkrm() );
      const auto q_circ = tweedledum::relative_phase_mapping<netlist_t>( stg_circ );

      std::cout << fmt::format( "[i] strategy = {} qubits = {:8d} gates = {:8d} T-gates = {:8d}\n",
                                "PPKM",
                                q_circ.num_qubits(),
                                q_circ.num_gates(),
                                count_T_gates( q_circ ) );
    }

    }
  }

  return 0;
}
