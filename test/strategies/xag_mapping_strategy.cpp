
#include <catch.hpp>

#include <caterpillar/synthesis/lhrs.hpp>
#include <caterpillar/synthesis/strategies/xag_mapping_strategy.hpp>
#include <caterpillar/verification/circuit_to_logic_network.hpp>
#include <mockturtle/networks/xag.hpp>
#include <tweedledum/networks/netlist.hpp>
#include <tweedledum/io/write_unicode.hpp>
#include <tweedledum/io/write_projectq.hpp>

#include <mockturtle/io/write_verilog.hpp>
#include <mockturtle/io/write_dot.hpp>

#include <caterpillar/structures/stg_gate.hpp>
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
  logic_network_synthesis_stats st;
  ps.verbose = false;

  xag_mapping_strategy strategy;
  logic_network_synthesis( qnet, xag, strategy, {}, ps, &st );

  auto tt_xag = simulate<kitty::static_truth_table<4>>( xag )[0];
  const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
  auto tt_ntk = simulate<kitty::static_truth_table<4>>( *ntk )[0];

  CHECK(tt_xag == tt_ntk);
  
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
  ps.verbose = false;

  xag_mapping_strategy strategy;
  logic_network_synthesis( qnet, xag, strategy, {}, ps, &st );

  auto tt_xag = simulate<kitty::static_truth_table<4>>( xag )[0];
  const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
  auto tt_ntk = simulate<kitty::static_truth_table<4>>( *ntk )[0];
  
  CHECK(tt_xag == tt_ntk);
  
}
TEST_CASE("synthesize simple xag 3", "[XAG synthesis-3]")
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
  auto g = xag.create_xor(a, f);
  auto h = xag.create_and(g, d);
  xag.create_po( h );

  netlist<stg_gate> qnet;

  logic_network_synthesis_params ps;
  logic_network_synthesis_stats st;
  ps.verbose = false;

  xag_mapping_strategy strategy;
  logic_network_synthesis( qnet, xag, strategy, {}, ps, &st );

  auto tt_xag = simulate<kitty::static_truth_table<4>>( xag )[0];
  const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
  auto tt_ntk = simulate<kitty::static_truth_table<4>>( *ntk )[0];

  CHECK(tt_xag == tt_ntk);

  
}

TEST_CASE("synthesize simple xag 4", "[XAG synthesis-4]")
{
  using namespace caterpillar;
  using namespace mockturtle;
  using namespace tweedledum;
  auto xag = xag_network();

  auto s = xag.create_pi();
  auto t5 = xag.create_pi();
  auto t2 = xag.create_pi();
  auto t15 = xag.create_pi();
  auto t12 = xag.create_pi();
  auto t8 = xag.create_pi();
  auto t7 = xag.create_pi();
  auto t13 = xag.create_pi();

  auto t6 = xag.create_xor(t5, t2);
  auto t16 = xag.create_xor(t15, t12);
  auto t18 = xag.create_xor(t6, t16);
  auto t9 = xag.create_xor(t7, t8);
  auto t14 = xag.create_xor(t13, t12);
  auto t19 = xag.create_xor(t9, t14);
  auto t22 = xag.create_xor(t18, t19);
  auto s0 = xag.create_and(t22, s);


  xag.create_po( s0 );

  netlist<stg_gate> qnet;

  logic_network_synthesis_params ps;
  logic_network_synthesis_stats st;
  ps.verbose = false;

  xag_mapping_strategy strategy;
  logic_network_synthesis( qnet, xag, strategy, {}, ps, &st );

  auto tt_xag = simulate<kitty::static_truth_table<4>>( xag )[0];
  const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
  auto tt_ntk = simulate<kitty::static_truth_table<4>>( *ntk )[0];

  CHECK(tt_xag == tt_ntk);

  
}

TEST_CASE("synthesize simple xag 5", "[XAG synthesis-5]")
{
  using namespace caterpillar;
  using namespace mockturtle;
  using namespace tweedledum;
  auto xag = xag_network();

  auto x0 = xag.create_pi();
  auto x3 = xag.create_pi();
  auto x4 = xag.create_pi();
  auto x5 = xag.create_pi();
  auto x6 = xag.create_pi();
  auto n10 = xag.create_xor(x6, x0);
  auto n9 = xag.create_xor(x5, x3);
  auto n16 = xag.create_xor(n10, n9);
  auto n20 = xag.create_xor(n16, x4);
  auto n21 = xag.create_xor(n20, x5);
  auto n32 = xag.create_and(n16, n21);
  xag.create_po(n32);

  netlist<stg_gate> qnet;
  logic_network_synthesis_params ps;
  logic_network_synthesis_stats st;
  ps.verbose = false;

  
  xag_mapping_strategy strategy;
  logic_network_synthesis( qnet, xag, strategy, {}, ps, &st );

  auto tt_xag = simulate<kitty::static_truth_table<4>>( xag )[0];
  const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
  auto tt_ntk = simulate<kitty::static_truth_table<4>>( *ntk )[0];
  CHECK(tt_xag == tt_ntk);

}

TEST_CASE("synthesize simple xag 6", "[XAG synthesis-6]")
{
  using namespace caterpillar;
  using namespace mockturtle;
  using namespace tweedledum;
  auto xag = xag_network();

  auto x0 = xag.create_pi();
  auto x1 = xag.create_pi();
  auto x2 = xag.create_pi();
  auto x3 = xag.create_pi();
  auto x4 = xag.create_pi();
  auto x5 = xag.create_pi();
  auto x6 = xag.create_pi();

  auto n10 = xag.create_xor(x6, x0);
  auto n9 = xag.create_xor(x5, x3);
  auto n16 = xag.create_xor(n10, n9);
  auto n20 = xag.create_xor(n16, x4);
  auto n22 = xag.create_xor(n20, x1);
  auto n11 = xag.create_xor(x3, x0);
  auto n25 = xag.create_xor(n22, n11);
  auto n13 = xag.create_xor(x2, x1);
  auto n29 = xag.create_xor(n25, n13);
  auto n37 = xag.create_xor(n10, n29);
  xag.create_po(n37);
  netlist<stg_gate> qnet;

  logic_network_synthesis_params ps;
  logic_network_synthesis_stats st;
  ps.verbose = false;

  xag_mapping_strategy strategy;
  logic_network_synthesis( qnet, xag, strategy, {}, ps, &st );

  auto tt_xag = simulate<kitty::static_truth_table<8>>( xag );
  const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
  auto tt_ntk = simulate<kitty::static_truth_table<8>>( *ntk );

  CHECK(tt_xag == tt_ntk);

}

TEST_CASE("synthesize simple xag with reconvergence", "[XAG synthesis-7]")
{
  using namespace caterpillar;
  using namespace mockturtle;
  using namespace tweedledum;
  auto xag = xag_network();

  auto x2 = xag.create_pi();
  auto x5 = xag.create_pi();
  auto x6 = xag.create_pi();
  auto x7 = xag.create_pi();
  auto x4 = xag.create_pi();


  auto n12 = xag.create_xor(x7, x5);
  auto n13 = xag.create_xor(n12, x6);
  auto n17 = xag.create_xor(n12, x2);
  auto n19 = xag.create_xor(n17, n13);
  auto n20 = xag.create_xor(x4, x6);
  auto n22 = xag.create_and(n19, n20);
  xag.create_po(n22);

  netlist<stg_gate> qnet;

  logic_network_synthesis_params ps;
  logic_network_synthesis_stats st;
  ps.verbose = false;

  xag_mapping_strategy strategy;
  logic_network_synthesis( qnet, xag, strategy, {}, ps, &st );

  auto tt_xag = simulate<kitty::static_truth_table<8>>( xag );
  const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
  auto tt_ntk = simulate<kitty::static_truth_table<8>>( *ntk );

  CHECK(tt_xag == tt_ntk);

}

TEST_CASE("synthesize simple xag 8", "[XAG synthesis-8]")
{
  using namespace caterpillar;
  using namespace mockturtle;
  using namespace tweedledum;
  auto xag = xag_network();

  auto x0 = xag.create_pi();
  auto x5 = xag.create_pi();
  auto x6 = xag.create_pi();
  auto x7 = xag.create_pi();

  auto n19 = xag.create_xor(x7, x0);
  auto n20 = xag.create_and(n19, x6);
  auto n21 = xag.create_and(n19, n20);
  auto n26 = xag.create_xor(x5, x0);
  auto n27 = xag.create_xor(n26, n20);
  auto n29 = xag.create_and(x7, n27);
  auto n32 = xag.create_and(n21, n29);

  xag.create_po(n32);

  netlist<stg_gate> qnet;

  logic_network_synthesis_params ps;
  logic_network_synthesis_stats st;
  ps.verbose = false;

  xag_mapping_strategy strategy;
  logic_network_synthesis( qnet, xag, strategy, {}, ps, &st );

  auto tt_xag = simulate<kitty::static_truth_table<4>>( xag );
  const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
  auto tt_ntk = simulate<kitty::static_truth_table<4>>( *ntk );

  CHECK(tt_xag == tt_ntk);


}

TEST_CASE("synthesize simple xag 9", "[XAG synthesis-9]")
{
  using namespace caterpillar;
  using namespace mockturtle;
  using namespace tweedledum;
  auto xag = xag_network();

  auto x1 = xag.create_pi();
  auto x2 = xag.create_pi();
  auto x6 = xag.create_pi();

  auto n13 = xag.create_xor(x2, x1);
  auto n14 = xag.create_xor(n13, x6);
  auto n18 = xag.create_xor(n14, x6);
  auto n38 = xag.create_and(n14, n18);
  
  xag.create_po(n38);

  netlist<stg_gate> qnet;

  logic_network_synthesis_params ps;
  logic_network_synthesis_stats st;
  ps.verbose = false;

  xag_mapping_strategy strategy;
  logic_network_synthesis( qnet, xag, strategy, {}, ps, &st );

  auto tt_xag = simulate<kitty::static_truth_table<4>>( xag );
  const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
  auto tt_ntk = simulate<kitty::static_truth_table<4>>( *ntk );

  CHECK(tt_xag == tt_ntk);


}
