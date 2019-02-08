#include <catch.hpp>

#include <iostream>
#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <vector>

#include <tweedledum/networks/netlist.hpp>

#include <caterpillar/stg_gate.hpp>

TEST_CASE( "create a netlist with one stg", "[stg netlist]" )
{
  using namespace caterpillar;
  using namespace tweedledum;

  netlist<stg_gate> circ;
  auto a = circ.add_qubit();
  auto b = circ.add_qubit();
  auto c = circ.add_qubit();
  auto d = circ.add_qubit();

  kitty::dynamic_truth_table func( 3u );
  kitty::create_majority( func );

  circ.add_gate( stg_gate( func, {a, qubit_id( b, true ), c}, d ) );

  CHECK( circ.num_gates() == 1 );
  CHECK( circ.num_qubits() == 4 );

  std::vector<uint32_t> indexes;
  std::vector<bool> pol;

  circ.foreach_cgate( [&]( auto n ) {
    CHECK( n.gate.num_controls() == 3 );

    n.gate.foreach_control( [&]( auto c ) {
      indexes.push_back( c.index() );
      pol.push_back( c.is_complemented() );
    } );
  } );

  CHECK( indexes == std::vector<uint32_t>{{0, 1, 2}} );
  CHECK( pol == std::vector<bool>{{false, true, false}} );
}
