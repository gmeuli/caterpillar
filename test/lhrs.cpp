#include <catch.hpp>

#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/xag.hpp>

#include <tweedledum/algorithms/synthesis/stg.hpp>
#include <tweedledum/io/write_unicode.hpp>
#include <tweedledum/networks/netlist.hpp>

#include <caterpillar/stg_gate.hpp>
#include <caterpillar/synthesis/lhrs.hpp>
#include <caterpillar/verification/circuit_to_logic_network.hpp>

TEST_CASE( "synthesize AND", "[lhrs AND test]" )
{
  using namespace tweedledum;
  using namespace caterpillar;
  using namespace mockturtle;

  auto aig = mockturtle::aig_network();

  auto a = aig.create_pi();
  auto b = aig.create_pi();
  auto c = aig.create_and( a, b );
  auto o = aig.create_po( c );

  netlist<stg_gate> revnet;
  logic_network_synthesis( revnet, aig, stg_from_pprm() );
  std::cout << "and net\n"
            << to_unicode_str( revnet );

  CHECK( revnet.num_gates() == 1 );
  CHECK( revnet.num_qubits() == 3 );

  revnet.foreach_cgate( [&]( auto g ) {
    CHECK( g.gate.num_controls() == 2 );
    g.gate.foreach_control( [&]( auto c ) {
      CHECK( c.is_complemented() == false );
      CHECK( c.index() <= 2 );
    } );
  } );

  const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( revnet, {0, 1}, {2} );
  CHECK( ntk );
  CHECK( ntk->num_pis() == 2u );
  CHECK( ntk->num_pos() == 1u );

  kitty::static_truth_table<2> function;
  kitty::create_from_binary_string( function, "1000" );

  CHECK( simulate<kitty::static_truth_table<2>>( *ntk )[0] == function );
}

TEST_CASE( "synthesize OR", "[lhrs OR test]" )
{
  using namespace tweedledum;
  using namespace caterpillar;
  using namespace mockturtle;

  auto aig = mockturtle::aig_network();

  auto a = aig.create_pi();
  auto b = aig.create_pi();
  auto c = aig.create_or( a, b );
  auto o = aig.create_po( c );

  netlist<stg_gate> revnet;
  logic_network_synthesis( revnet, aig, stg_from_pprm() );

  std::cout << "or net\n"
            << to_unicode_str( revnet );

  CHECK( revnet.num_gates() == 2 );
  CHECK( revnet.num_qubits() == 3 );

  auto i = 0u;
  revnet.foreach_cgate( [&]( auto g ) {
    if ( i == 0 )
      CHECK( g.gate.num_controls() == 2 );
    else
      CHECK( g.gate.num_controls() == 0 );

    g.gate.foreach_control( [&]( auto c ) {
      CHECK( c.is_complemented() == true );
      CHECK( c.index() <= 2 );
    } );
  } );

  const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( revnet, {0, 1}, {2} );
  CHECK( ntk );
  CHECK( ntk->num_pis() == 2u );
  CHECK( ntk->num_pos() == 1u );

  kitty::static_truth_table<2> function;
  kitty::create_from_binary_string( function, "1110" );

  CHECK( simulate<kitty::static_truth_table<2>>( *ntk )[0] == function );
}