#include <catch.hpp>

#include <mockturtle/networks/aig.hpp>

#include <tweedledum/algorithms/synthesis/stg.hpp>
#include <tweedledum/networks/netlist.hpp>

#include <caterpillar/stg_gate.hpp>
#include <caterpillar/synthesis/lhrs.hpp>

TEST_CASE( "synthesize AND", "[lhrs AND test]" )
{
  auto aig = mockturtle::aig_network();

  auto a = aig.create_pi();
  auto b = aig.create_pi();
  auto c = aig.create_and( a, b );
  auto o = aig.create_po( c );

  tweedledum::netlist<caterpillar::stg_gate> revnet;
  caterpillar::logic_network_synthesis( revnet, aig, tweedledum::stg_from_pprm() );

  CHECK( revnet.num_gates() == 1 );
  CHECK( revnet.num_qubits() == 3 );

  revnet.foreach_cgate( [&]( auto g ) {
    CHECK( g.gate.num_controls() == 2 );
    g.gate.foreach_control( [&]( auto c ) {
      CHECK( c.is_complemented() == false );
      CHECK( c.index() <= 2 );
    } );
  } );
}