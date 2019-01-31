#include <tweedledum/algorithms/mapping/relative_phase.hpp>
#include <tweedledum/algorithms/synthesis/decomposition_based.hpp>
#include <tweedledum/algorithms/synthesis/single_target_gates.hpp>
#include <tweedledum/gates/mcmt_gate.hpp>
#include <tweedledum/io/print.hpp>
#include <tweedledum/algorithms/mapping/nct.hpp>

#include <mockturtle/utils/stopwatch.hpp>
#include <vector>
#include <map>

using namespace tweedledum;
using namespace mockturtle;

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
      netlist.foreach_gate( [&]( const auto& gate ){
          if ( gate.gate.kind() == tweedledum::gate_kinds_t::t )
          {
            ++T_number;
          }
        });
      return T_number;
    };

    stopwatch t( st.total_time );
    auto const stg_circ = tweedledum::decomposition_based_synthesis<Ntk>( perm, synth_fn );
    std::cout << tweedledum::to_unicode(stg_circ)<< std::endl;

    tweedledum::nct_mapping_params ps;
    ps.controls_threshold = 4;
    auto const dec_stg_circ =  tweedledum::nct_mapping<Ntk>(stg_circ, ps);
    std::cout << tweedledum::to_unicode(dec_stg_circ)<< std::endl;

    auto const q_circ = tweedledum::relative_phase_mapping<Ntk>( dec_stg_circ );
    std::cout << tweedledum::to_unicode(q_circ)<< std::endl;

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
  std::vector<uint16_t> prime7{{0, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 1, 4, 6, 8, 9, 10, 12, 14, 15, 16, 18, 20, 21, 22, 24, 25, 26, 27, 28, 30, 32, 33, 34, 35, 36, 38, 39, 40, 42, 44, 45, 46, 48, 49, 50, 51, 52, 54, 55, 56, 57, 58, 60, 62, 63, 64, 65, 66, 68, 69, 70, 72, 74, 75, 76, 77, 78, 80, 81, 82, 84,85, 86, 87, 88, 90, 91, 92, 93, 94, 95, 96, 98, 99, 100, 102, 104, 105, 106, 108, 110, 111, 112, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126}};
  std::vector<uint16_t> prime8{{0, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 1, 4, 6, 8, 9, 10, 12, 14, 15, 16, 18, 20, 21, 22, 24, 25, 26, 27, 28, 30, 32, 33, 34, 35, 36, 38, 39, 40, 42, 44, 45, 46, 48, 49, 50, 51, 52, 54, 55, 56, 57, 58, 60, 62, 63, 64, 65, 66, 68, 69, 70, 72, 74, 75, 76, 77, 78, 80, 81, 82, 84, 85, 86,87, 88, 90, 91, 92, 93, 94, 95, 96, 98, 99, 100, 102, 104, 105, 106, 108, 110, 111, 112, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 128, 129, 130, 132, 133, 134, 135, 136, 138, 140, 141, 142, 143, 144, 145, 146, 147, 148, 150, 152, 153, 154, 155, 156, 158, 159, 160, 161, 162, 164, 165, 166, 168, 169, 170, 171, 172, 174, 175, 176, 177, 178, 180,182, 183, 184, 185, 186, 187, 188, 189, 190, 192, 194, 195, 196, 198, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 224, 225, 226, 228, 230, 231, 232, 234, 235, 236, 237, 238, 240, 242, 243, 244, 245, 246, 247, 248, 249, 250, 252, 253, 254, 255}};
  
  /* tof */
  std::vector<uint16_t> tof3{{0, 1, 2, 3, 4, 5, 7, 6}};
  std::vector<uint16_t> tof4{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 14}};
  std::vector<uint16_t> tof5{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 31, 30}};
  std::vector<uint16_t> tof6{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 63, 62}};

  /* hwb */
  std::vector<uint16_t> hwb4{{0, 2, 4, 12, 8, 5, 9, 11, 1, 6, 10, 13, 3, 14, 7, 15}};
  std::vector<uint16_t> hwb5{{0, 2, 4, 12, 8, 20, 24, 25, 16, 5, 9, 26, 17, 11, 19, 23, 1, 6, 10, 28, 18, 13, 21, 27, 3, 14, 22, 29, 7, 30, 15, 31}};
  std::vector<uint16_t> hwb6{{0, 2, 4, 12, 8, 20, 24, 56, 16, 36, 40, 25, 48, 41, 49, 51, 32, 5, 9, 26, 17, 42, 50, 53, 33, 11, 19, 54, 35, 23, 39, 47,1, 6, 10, 28, 18, 44, 52, 57, 34, 13, 21, 58, 37, 27, 43, 55, 3, 14, 22, 60, 38, 29, 45, 59, 7, 30, 46, 61, 15, 62, 31, 63}};
  std::vector<uint16_t> hwb7{{0, 2, 4, 12, 8, 20, 24, 56, 16, 36, 40, 88, 48, 104, 112, 113, 32, 68, 72, 25, 80, 41, 49, 114, 96, 73, 81, 51, 97, 83, 99, 103, 64, 5, 9, 26, 17, 42, 50, 116, 33, 74, 82, 53, 98, 85, 101, 107, 65, 11, 19, 54, 35, 86, 102, 109, 67, 23, 39, 110,71, 47, 79, 95, 1, 6, 10, 28, 18, 44, 52, 120, 34, 76, 84, 57, 100, 89, 105, 115, 66, 13, 21, 58, 37, 90, 106, 117, 69, 27, 43, 118, 75, 55, 87, 111, 3, 14, 22, 60, 38, 92, 108, 121, 70, 29, 45, 122, 77, 59, 91, 119, 7, 30, 46, 124, 78, 61, 93, 123, 15, 62, 94, 125, 31, 126, 63, 127}};
  std::vector<uint16_t> hwb8{{0, 2, 4, 12, 8, 20, 24, 56, 16, 36, 40, 88, 48, 104, 112, 240, 32, 68, 72, 152, 80, 168, 176, 113, 96, 200, 208, 177, 224, 209, 225, 227, 64, 132, 136, 25, 144, 41, 49, 114, 160, 73, 81, 178, 97, 210, 226, 229, 192, 137, 145, 51, 161, 83, 99, 230, 193, 147, 163, 103, 195, 167, 199, 207, 128, 5, 9, 26, 17, 42, 50, 116, 33, 74, 82, 180, 98, 212, 228, 233, 65, 138, 146, 53, 162, 85, 101, 234, 194, 149, 165, 107, 197, 171, 203, 215, 129, 11, 19, 54, 35, 86, 102, 236, 67, 150, 166, 109, 198, 173, 205, 219, 131, 23, 39, 110, 71, 174, 206, 221, 135, 47, 79, 222, 143, 95, 159, 191, 1, 6, 10, 28, 18, 44, 52, 120, 34, 76, 84, 184, 100, 216, 232, 241, 66, 140, 148, 57, 164, 89, 105, 242, 196, 153, 169, 115, 201, 179, 211, 231, 130, 13, 21, 58, 37, 90, 106, 244, 69, 154, 170, 117, 202, 181, 213, 235, 133, 27, 43, 118, 75, 182, 214, 237, 139, 55, 87, 238, 151,111, 175, 223, 3, 14, 22, 60, 38, 92, 108, 248, 70, 156, 172, 121, 204, 185, 217, 243, 134, 29, 45, 122, 77, 186, 218, 245, 141, 59, 91, 246, 155, 119, 183, 239, 7, 30, 46, 124, 78, 188, 220, 249, 142, 61, 93, 250, 157, 123, 187, 247, 15, 62, 94, 252, 158, 125, 189, 251, 31, 126, 190, 253, 63, 254, 127, 255}};
  
  /* mod */
  std::vector<uint16_t> mod3_3{{0, 3, 2, 6, 4, 5, 1, 7}};
  std::vector<uint16_t> mod3_5{{0, 5, 4, 6, 2, 3, 1, 7}};
  std::vector<uint16_t> mod5_3{{0, 3, 9, 27, 19, 26, 16, 17, 20, 29, 25, 13, 8, 24, 10, 30, 28, 22, 4, 12, 5, 15, 14, 11, 2, 6, 18, 23, 7, 21, 1, 31}};
  std::vector<uint16_t> mod5_11{{0, 11, 28, 29, 9, 6, 4, 13, 19, 23, 5, 24, 16, 21, 14, 30, 20, 3, 2, 22, 25, 27, 18, 12, 8, 26, 7, 15, 10, 17, 1,31}};
  std::vector<uint16_t> mod5_12{{0, 12, 20, 23, 28, 26, 2, 24, 9, 15, 25, 21, 4, 17, 18, 30, 19, 11, 8, 3, 5, 29, 7, 22, 16, 6, 10, 27, 14, 13, 1, 31}};
  std::vector<uint16_t> mod5_13{{0, 13, 14, 27, 10, 6, 16, 22, 7, 29, 5, 3, 8, 11, 19, 30, 18, 17, 4, 21, 25, 15, 9, 24, 2, 26, 28, 23, 20, 12, 1, 31}};
  std::vector<uint16_t> mod5_17{{0, 17, 10, 15, 7, 26, 8, 12, 18, 27, 25, 22, 2, 3, 20, 30, 14, 21, 16, 24, 5, 23, 19, 13, 4, 6, 9, 29, 28, 11, 1, 31}};
  std::vector<uint16_t> mod5_21{{0, 21, 7, 23, 18, 6, 2, 11, 14, 15, 5, 12, 4, 22, 28, 30, 10, 24, 8, 13, 25, 29, 20, 17, 16, 26, 19, 27, 9, 3, 1, 31}};
  std::vector<uint16_t> mod5_22{{0, 22, 19, 15, 20, 6, 8, 21, 28, 27, 5, 17, 2, 13, 7, 30, 9, 12, 16, 11, 25, 23, 10, 3, 4, 26, 14, 29, 18, 24, 1, 31}};
  std::vector<uint16_t> mod5_24{{0, 24, 18, 29, 14, 26, 4, 3, 10, 23, 25, 11, 16, 12, 9, 30, 7, 13, 2, 17, 5, 27, 28, 21, 8, 6, 20, 15, 19, 22, 1, 31}};
  std::vector<uint16_t> mod7_3{{0, 3, 9, 27, 81, 116, 94, 28, 84, 125, 121, 109, 73, 92, 22, 66, 71, 86, 4, 12, 36, 108, 70, 83, 122, 112, 82, 119, 103, 55, 38, 114, 88, 10, 30, 90, 16, 48, 17, 51, 26, 78, 107, 67, 74, 95, 31, 93, 25, 75, 98, 40, 120, 106, 64, 65, 68, 77, 104, 58, 47, 14, 42, 126, 124, 118, 100, 46, 11, 33, 99, 43, 2, 6, 18, 54, 35, 105, 61, 56, 41, 123, 115, 91, 19, 57, 44, 5, 15, 45, 8, 24, 72, 89, 13, 39, 117, 97, 37, 111, 79, 110, 76, 101, 49, 20, 60, 53, 32, 96, 34, 102, 52, 29, 87, 7, 21, 63, 62, 59, 50, 23, 69, 80, 113, 85, 1, 127}};
  std::vector<uint16_t> mod7_7{{0, 7, 49, 89, 115, 43, 47, 75, 17, 119, 71, 116, 50, 96, 37, 5, 35, 118, 64, 67, 88, 108, 121, 85, 87, 101, 72, 123, 99, 58, 25, 48, 82, 66, 81, 59, 32, 97, 44, 54, 124, 106, 107, 114, 36, 125, 113, 29, 76, 24, 41, 33, 104, 93, 16, 112, 22, 27, 62, 53, 117, 57, 18, 126, 120, 78, 38, 12, 84, 80, 52, 110, 8, 56, 11, 77, 31, 90, 122, 92, 9, 63, 60, 39, 19, 6, 42, 40, 26, 55, 4, 28, 69, 102, 79, 45, 61, 46, 68, 95, 30, 83, 73, 3, 21, 20, 13, 91, 2, 14, 98, 51, 103, 86, 94, 23, 34, 111, 15, 105, 100, 65, 74, 10, 70, 109, 1, 127}};
  std::vector<uint16_t> mod7_14{{0, 14, 69, 77, 62, 106, 87, 75, 34, 95, 60, 78, 76, 48, 37, 10, 13, 55, 8, 112, 44, 108, 115, 86, 61, 92, 18, 125, 99, 116, 100, 3, 42, 80, 104, 59, 64, 7, 98, 102, 31, 53, 107, 101, 17, 111, 30, 39, 38, 24, 82, 5, 70, 91, 4, 56, 22, 54, 121, 43, 94, 46, 9, 126, 113, 58, 50, 65, 21, 40, 52, 93, 32, 67, 49, 51, 79, 90, 117, 114, 72, 119, 15, 83, 19, 12, 41, 66, 35, 109, 2, 28, 11, 27, 124, 85, 47, 23, 68, 63, 120, 29, 25, 96, 74, 20, 26, 110, 16, 97, 88, 89, 103, 45, 122, 57, 36, 123, 71, 105, 73, 6, 84, 33, 81, 118, 1, 127}};
  
  
  std::map<std::string, std::vector<uint16_t>> benchmarks;
  benchmarks["prime3"] = prime3;
  benchmarks["prime4"] = prime4;
  benchmarks["prime5"] = prime5;
  benchmarks["prime6"] = prime6;
  benchmarks["prime7"] = prime7;
  benchmarks["prime8"] = prime8;

  benchmarks["tof3"] = tof3;
  benchmarks["tof4"] = tof4;
  benchmarks["tof5"] = tof5;
  benchmarks["tof6"] = tof6;

  benchmarks["hwb4"] = hwb4;
  benchmarks["hwb5"] = hwb5;
  benchmarks["hwb6"] = hwb6;
  benchmarks["hwb7"] = hwb7;
  benchmarks["hwb8"] = hwb8;

  benchmarks["mod3_3"] = mod3_3;
  benchmarks["mod3_5"] = mod3_5;
  benchmarks["mod5_3"] = mod5_3;
  benchmarks["mod5_11"] = mod5_11;
  benchmarks["mod5_12"] = mod5_12;
  benchmarks["mod5_13"] = mod5_13;
  benchmarks["mod5_17"] = mod5_17;
  benchmarks["mod5_21"] = mod5_21;
  benchmarks["mod5_22"] = mod5_22;
  benchmarks["mod5_24"] = mod5_24;
  benchmarks["mod7_3"] = mod7_3;
  benchmarks["mod7_7"] = mod7_7;
  benchmarks["mod7_14"] = mod7_14;

  using netlist_t = netlist<mcmt_gate>;

  for( auto& [key, val] : benchmarks )
  {
    std::cout << BOLD << "[i] benchmark: " << UNDERLINE << key << ENDC << std::endl;

    if(key == "tof6")
    {
      run_dbs_with_strategy<netlist_t>( val, tweedledum::stg_from_pprm(), "PPRM" );
      run_dbs_with_strategy<netlist_t>( val, tweedledum::stg_from_pkrm(), "PKRM" );
      run_dbs_with_strategy<netlist_t>( val, tweedledum::stg_from_exact_synthesis(), "EXACT(unit)" );
    }
    if ( true )
      continue;
    
    run_dbs_with_strategy<netlist_t>( val, tweedledum::stg_from_exact_synthesis( lit_cost ), "EXACT(lit)" );
    run_dbs_with_strategy<netlist_t>( val, tweedledum::stg_from_exact_synthesis( complex_cost ), "EXACT(complex)" );
  }

  return 0;
}
