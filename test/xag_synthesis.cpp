
#include <catch.hpp>

#include <caterpillar/synthesis/lhrs.hpp>
#include <caterpillar/synthesis/strategies/xag_mapping_strategy.hpp>
#include <caterpillar/verification/circuit_to_logic_network.hpp>
#include <mockturtle/networks/xag.hpp>
#include <tweedledum/networks/netlist.hpp>
#include <tweedledum/io/write_unicode.hpp>
#include <mockturtle/io/write_bench.hpp>
#include <caterpillar/stg_gate.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <kitty/static_truth_table.hpp>


TEST_CASE("synthesize simple xag", "[XAG synthesis]")
{
  using namespace caterpillar;
  using namespace mockturtle;
  using namespace tweedledum;
  auto xag = xag_network();

  auto a = xag.create_pi();
  auto b = xag.create_pi();
  auto c = xag.create_pi();
  auto d = xag.create_and( a, b );
  auto e = xag.create_xor( d, c );
  xag.create_po( e );

  netlist<stg_gate> qnet;

  logic_network_synthesis_params ps;
  //ps.verbose = true;

  logic_network_synthesis<netlist<stg_gate>, xag_network, xag_mapping_strategy>( qnet, xag, {}, ps );

  std::vector<stg_gate> gates;
  qnet.foreach_cgate([&gates] (auto g){ gates.push_back(g.gate); });

  CHECK(qnet.num_gates() == 4);
  CHECK(qnet.num_qubits() == 5);
  CHECK(gates[0].num_controls() == 2);
  CHECK(gates[1].num_controls() == 1);
  CHECK(gates[2].num_controls() == 1);
  CHECK(gates[3].num_controls() == 2);
  
}


TEST_CASE("synthesize simple xag 2", "[XAG synthesis-2]")
{
  using namespace caterpillar;
  using namespace mockturtle;
  using namespace tweedledum;
  auto xag = xag_network();

  auto a = xag.create_pi();
  auto b = xag.create_pi();
  auto c = xag.create_pi();
  auto d = xag.create_pi();
  auto e = xag.create_xor(a, b);
  auto f = xag.create_xor(e, c);
  auto g = xag.create_and(a, f);
  auto h = xag.create_and(g, d);
  auto i = xag.create_xor(h, d);
  xag.create_po( i );

  netlist<stg_gate> qnet;

  logic_network_synthesis_params ps;
  logic_network_synthesis_stats st;
  //ps.verbose = true;

  logic_network_synthesis<netlist<stg_gate>, xag_network, xag_mapping_strategy>( qnet, xag, {}, ps, &st );

  auto tt_xag = simulate<kitty::static_truth_table<4>>( xag )[0];
  const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
  auto tt_ntk = simulate<kitty::static_truth_table<4>>( *ntk )[0];
  
  CHECK(qnet.num_gates() == 14);
  CHECK(tt_xag == tt_ntk);
  
}
