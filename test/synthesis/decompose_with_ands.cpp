#include <catch.hpp>

#include <mockturtle/mockturtle.hpp>
#include <caterpillar/synthesis/decompose_with_ands.hpp>
#include <caterpillar/caterpillar.hpp>
#include <caterpillar/details/utils.hpp>
#include <tweedledum/tweedledum.hpp>
#include <tweedledum/io/write_unicode.hpp>


TEST_CASE("decompose simple xag", "[XAG decompose]")
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


  netlist<mcmt_gate> qcirc;
  decompose_with_ands(qcirc, qnet);

  CHECK(caterpillar::detail::t_cost(qcirc) == 4);
  
}

TEST_CASE("decompose circ with many ands", "[decopmpose circ with many ands]")
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

  netlist<mcmt_gate> qcirc;
  decompose_with_ands(qcirc, qnet);

  CHECK(caterpillar::detail::t_cost(qcirc) == 16);
}
