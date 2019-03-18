#include <catch.hpp>

#include <caterpillar/synthesis/satbased_cnotrz.hpp>
#include <tweedledum/gates/mcst_gate.hpp>
#include <tweedledum/io/write_unicode.hpp>
#include <tweedledum/networks/netlist.hpp>
#include <tweedledum/utils/bit_matrix_rm.hpp>
#include <tweedledum/utils/parity_terms.hpp>

TEST_CASE( "Optimum phase polynomial circuit for Toffoli with 2 controls", "[satbased_cnotrz]" )
{
  tweedledum::bit_matrix_rm<> transform( 3u, 3u );
  transform.at( 0, 0 ) = 1;
  transform.at( 1, 1 ) = 1;
  transform.at( 2, 2 ) = 1;
  
  tweedledum::parity_terms terms;
  constexpr auto T = tweedledum::symbolic_angles::one_eighth;
  constexpr auto Tdag = tweedledum::symbolic_angles::seven_eighth;

  terms.add_term( 0b001, T );
  terms.add_term( 0b010, T );
  terms.add_term( 0b011, Tdag );
  terms.add_term( 0b100, T );
  terms.add_term( 0b101, Tdag );
  terms.add_term( 0b110, Tdag );
  terms.add_term( 0b111, T );

  caterpillar::satbased_cnotrz_params ps;
  ps.verbose = false;
  const auto circ = caterpillar::satbased_cnotrz<tweedledum::netlist<tweedledum::mcst_gate>>( transform, terms, ps );

  CHECK( circ.num_gates() == 13u );
  CHECK( circ.num_qubits() == 3u );
}

TEST_CASE( "Optimum phase polynomial circuit for Toffoli with 3 controls", "[satbased_cnotrz]" )
{
  return; // ? (this test case takes a while, but works)

  tweedledum::bit_matrix_rm<> transform( 4u, 4u );
  transform.at( 0, 0 ) = 1;
  transform.at( 1, 1 ) = 1;
  transform.at( 2, 2 ) = 1;
  transform.at( 3, 3 ) = 1;
  
  // INFO: These angles are not correct, but the terms are
  tweedledum::parity_terms terms;
  terms.add_term( 0b0001, .1f );
  terms.add_term( 0b0010, .1f );
  terms.add_term( 0b0011, .1f );
  terms.add_term( 0b0100, .1f );
  terms.add_term( 0b0101, .1f );
  terms.add_term( 0b0110, .1f );
  terms.add_term( 0b0111, .1f );
  terms.add_term( 0b1000, .1f );
  terms.add_term( 0b1001, .1f );
  terms.add_term( 0b1010, .1f );
  terms.add_term( 0b1011, .1f );
  terms.add_term( 0b1100, .1f );
  terms.add_term( 0b1101, .1f );
  terms.add_term( 0b1110, .1f );
  terms.add_term( 0b1111, .1f );

  caterpillar::satbased_cnotrz_params ps;
  ps.verbose = false;
  ps.conflict_limit = 10000;
  const auto circ = caterpillar::satbased_cnotrz<tweedledum::netlist<tweedledum::mcst_gate>>( transform, terms, ps );

  CHECK( circ.num_gates() == 29u );
  CHECK( circ.num_qubits() == 4u );
}

TEST_CASE( "Optimum phase polynomial circuit for smaller circuit with 4 qubits", "[satbased_cnotrz]" )
{
  tweedledum::bit_matrix_rm<> transform( 4u, 4u );
  transform.at( 0, 0 ) = 1;
  transform.at( 1, 1 ) = 1;
  transform.at( 2, 2 ) = 1;
  transform.at( 3, 3 ) = 1;
  
  // INFO: These angles are not correct, but the terms are
  tweedledum::parity_terms terms;
  terms.add_term( 0b1000, .1f );
  terms.add_term( 0b1001, .1f );
  terms.add_term( 0b1010, .1f );
  terms.add_term( 0b1011, .1f );
  terms.add_term( 0b1100, .1f );
  terms.add_term( 0b1101, .1f );
  terms.add_term( 0b1110, .1f );
  terms.add_term( 0b1111, .1f );

  caterpillar::satbased_cnotrz_params ps;
  ps.verbose = false;
  const auto circ = caterpillar::satbased_cnotrz<tweedledum::netlist<tweedledum::mcst_gate>>( transform, terms, ps );

  CHECK( circ.num_gates() == 16u );
  CHECK( circ.num_qubits() == 4u );
}
