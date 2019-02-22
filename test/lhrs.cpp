#include <catch.hpp>

#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>


#include <caterpillar/synthesis/lhrs.hpp>
#include <caterpillar/stg_gate.hpp>
#include <caterpillar/verification/circuit_to_logic_network.hpp>


#include <tweedledum/algorithms/synthesis/stg.hpp>
#include <tweedledum/io/write_unicode.hpp>
#include <tweedledum/networks/netlist.hpp>

TEST_CASE( "synthesize AND", "[lhrs AND test]" )
{
  using namespace tweedledum;
  using namespace caterpillar;
  using namespace mockturtle;

  auto aig = mockturtle::aig_network();

  auto a = aig.create_pi();
  auto b = aig.create_pi();
  auto c = aig.create_and( a, b );
  /*auto o = */aig.create_po( c );

  netlist<stg_gate> revnet;
  bennett_inplace_mapping_strategy<aig_network> strategy;
  logic_network_synthesis_stats st;
  logic_network_synthesis( revnet, aig, strategy, stg_from_pprm(), {}, &st );

  CHECK( revnet.num_gates() == 1 );
  CHECK( revnet.num_qubits() == 3 );

  revnet.foreach_cgate( [&]( auto g ) {
    CHECK( g.gate.num_controls() == 2 );
    g.gate.foreach_control( [&]( auto c ) {
      CHECK( c.is_complemented() == false );
      CHECK( c.index() <= 2 );
    } );
  } );

  const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( revnet, st.i_indexes, st.o_indexes );
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
  /*auto o = */aig.create_po( c );

  netlist<stg_gate> revnet;
  bennett_inplace_mapping_strategy<aig_network> strategy;
  logic_network_synthesis_stats st;
  logic_network_synthesis( revnet, aig, strategy, stg_from_pprm(), {}, &st);

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
    i++;
  } );

  const auto ntk = circuit_to_logic_network<xag_network>( revnet, st.i_indexes, st.o_indexes );
  CHECK( ntk );
  CHECK( ntk->num_pis() == 2u );
  CHECK( ntk->num_pos() == 1u );

  kitty::static_truth_table<2> function;
  kitty::create_from_binary_string( function, "1110" );

  CHECK( simulate<kitty::static_truth_table<2>>( *ntk )[0] == function );
}

TEST_CASE( "synthesize multioutput maj3", "[multiout MAJ]" )
{
  using namespace mockturtle;
  using namespace caterpillar;
  using namespace tweedledum;

  auto mig = mig_network();

  auto a = mig.create_pi();
  auto b = mig.create_pi();
  auto c = mig.create_pi();
  auto d = mig.create_maj( a, b, c );
  mig.create_po( d );
  mig.create_po( mig.create_not( d ) );
  mig.create_po( d );

  netlist<stg_gate> t_net;

  logic_network_synthesis_stats st;
  bennett_inplace_mapping_strategy<mig_network> strategy;
  logic_network_synthesis( t_net, mig, strategy, stg_from_pprm(), {}, &st );

  const auto ntk = circuit_to_logic_network<xag_network>( t_net, st.i_indexes, st.o_indexes );

  kitty::static_truth_table<3> maj;
  kitty::create_from_binary_string( maj, "11101000" );

  CHECK( simulate<kitty::static_truth_table<3>>( *ntk )[0] == maj );
  CHECK( simulate<kitty::static_truth_table<3>>( *ntk )[1] == ~maj );
  CHECK( simulate<kitty::static_truth_table<3>>( *ntk )[2] == maj );
}
