#include <catch.hpp>

#include <caterpillar/structures/pebbling_view.hpp>
#include <caterpillar/solvers/z3_solver.hpp>
#include <caterpillar/synthesis/strategies/mapping_strategy.hpp>
#include <caterpillar/synthesis/strategies/pebbling_mapping_strategy.hpp>

#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/klut.hpp>

#include <kitty/dynamic_truth_table.hpp>


using namespace caterpillar;

#ifdef USE_Z3
TEST_CASE(" pebble simple graph ", "[pebble simple graph]")
{
	

	mockturtle::xag_network net;

	auto p1 = net.create_pi();
	auto p2 = net.create_pi();
	auto p3 = net.create_pi();

	auto n1 = net.create_xor(p1, p2);
	auto n2 = net.create_and(n1, p3);

	net.create_po(n2);

	pebbling_view<mockturtle::xag_network> pnet ( net );

  auto z3_solver = z3_pebble_solver( pnet, 2 );

	z3_solver.init();

	do{
		z3_solver.add_step();
	}while (z3_solver.solve() == unsat);

	CHECK(z3_solver.solve() == sat);
	z3_solver.save_model();
	auto strategy = z3_solver.extract_result(false);

  assert( strategy.size() == 3 );
}

TEST_CASE(" pebble simple graph 4 nodes ", "[pebble simple graph 4 nodes]")
{
	auto net = mockturtle::aig_network();

	auto p1 = net.create_pi();
	auto p2 = net.create_pi();
	auto p3 = net.create_pi();
	auto p4 = net.create_pi();

	auto n1 = net.create_and(p1, p2);
	auto n2 = net.create_and(n1, p3);
	auto n3 = net.create_and( p1, p4);
	auto n4 = net.create_and(n2, n3);

	net.create_po(n4);

	pebbling_view<mockturtle::aig_network> pnet( net );

	auto z3_solver = z3_pebble_solver( pnet, 3 );

	z3_solver.init();

	do{
		z3_solver.add_step();
	}while (z3_solver.solve() == unsat);

	CHECK(z3_solver.solve() == sat);

	z3_solver.save_model();
	auto strategy = z3_solver.extract_result(false);

	assert( strategy.size() == 9 );

}

TEST_CASE("pebble using weights", "[pebble using weights]")
{
	kitty::dynamic_truth_table tt_1( 1 ), tt_rand1( 2 ), tt_rand2(2);
  kitty::create_random( tt_1 );
	kitty::create_random( tt_rand1 );
	kitty::create_random( tt_rand2 );

	auto net = mockturtle::klut_network();

	auto p1 = net.create_pi();
	auto p2 = net.create_pi();
	auto p3 = net.create_pi();

	auto n1 = net.create_node({p1}, tt_1);
	auto n2 = net.create_node({p2}, tt_1);
	auto n3 = net.create_node({p3}, tt_1);
	auto n4 = net.create_node({n1, n2}, tt_rand1);
	auto n5 = net.create_node({n4, n3}, tt_rand2);

	net.create_po(n5);

	pebbling_view<mockturtle::klut_network> pnet( net );

	auto n = net.get_node( n1 );
	pnet.set_weight( n, 5 );

  auto z3_solver = z3_pebble_solver( pnet, 4 , 0 );

  z3_solver.init();

	do{
		z3_solver.add_step();
	}while (z3_solver.solve() == unsat);

	CHECK(z3_solver.solve() == sat);
	z3_solver.save_model();
	auto strategy = z3_solver.extract_result( false );
}
#endif