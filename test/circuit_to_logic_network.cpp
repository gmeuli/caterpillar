#include <catch.hpp>

#include <cstdint>
#include <vector>

#include <caterpillar/circuit_to_logic_network.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/constructors.hpp>
#include <kitty/static_truth_table.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/networks/xag.hpp>
#include <tweedledum/gates/mcmt_gate.hpp>
#include <tweedledum/networks/netlist.hpp>

TEST_CASE("Convert simple reversible circuit to XAG", "[circuit_to_logic_network]")
{
  using namespace caterpillar;
  using namespace mockturtle;
  using namespace tweedledum;

  netlist<mcmt_gate> circ;
  const auto a = circ.add_qubit();
  const auto b = circ.add_qubit();
  const auto c = circ.add_qubit();
  const auto d = circ.add_qubit();
  const auto e = circ.add_qubit();

  circ.add_gate( gate_kinds_t::mcx, std::vector<uint32_t>{{a, b}}, {d} );
  circ.add_gate( gate_kinds_t::mcx, std::vector<uint32_t>{{c, d}}, {e} );
  circ.add_gate( gate_kinds_t::mcx, std::vector<uint32_t>{{a, b}}, {d} );

  const auto ntk = circuit_to_logic_network<xag_network>( circ, {a, b, c}, {e} );

  CHECK( ntk );
  CHECK( ntk->num_pis() == 3u );
  CHECK( ntk->num_pos() == 1u );

  kitty::static_truth_table<3> function;
  kitty::create_from_hex_string( function, "80" );

  CHECK( simulate<kitty::static_truth_table<3>>( *ntk )[0] == function );

  kitty::dynamic_truth_table dfunction( 3u );
  kitty::create_from_binary_string( dfunction, "10000000" );

  default_simulator<kitty::dynamic_truth_table> sim( dfunction.num_vars() );
  CHECK( simulate<kitty::dynamic_truth_table>( *ntk, sim )[0] == dfunction );
}

TEST_CASE("Convert incrementer XAG", "[circuit_to_logic_network]")
{
  using namespace caterpillar;
  using namespace mockturtle;
  using namespace tweedledum;

  netlist<mcmt_gate> circ;
  const auto a = circ.add_qubit();
  const auto b = circ.add_qubit();
  const auto c = circ.add_qubit();

  circ.add_gate( gate_kinds_t::mcx, std::vector<uint32_t>{{a, b}}, {c} );
  circ.add_gate( gate_kinds_t::cx, a, b );
  circ.add_gate( gate_kinds_t::pauli_x, a );

  const auto ntk = circuit_to_logic_network<xag_network>( circ, {a, b, c}, {a, b, c} );

  CHECK( ntk );
  CHECK( ntk->num_pis() == 3u );
  CHECK( ntk->num_pos() == 3u );

  const auto simvals = simulate<kitty::static_truth_table<3>>( *ntk );

  CHECK( simvals[0]._bits == 0b01010101u );
  CHECK( simvals[1]._bits == 0b01100110u );
  CHECK( simvals[2]._bits == 0b01111000u );
}

TEST_CASE("Fail when converting quantum circuit to logic network", "[circuit_to_logic_network]")
{
  using namespace caterpillar;
  using namespace mockturtle;
  using namespace tweedledum;

  netlist<mcmt_gate> circ;
  const auto a = circ.add_qubit();

  circ.add_gate( gate_kinds_t::hadamard, a );

  const auto ntk = circuit_to_logic_network<xag_network>( circ, {a}, {a} );

  CHECK( !ntk );
}
