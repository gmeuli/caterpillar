#include <catch.hpp>

#include <caterpillar/structures/abstract_network.hpp>
#include <caterpillar/pebbling_solvers/z3_solver.hpp>

#ifdef USE_Z3
TEST_CASE(" pebble simple graph ", "[pebble simple graph]")
{
	using namespace caterpillar;
	auto net = abstract_network();

	auto p1 = net.create_pi();
	auto p2 = net.create_pi();
	auto p3 = net.create_pi();

	auto n1 = net.create_node({p1, p2}, 1);
	auto n2 = net.create_node({n1, p3}, 2);

	net.create_po(n2);

	auto z3_solver = z3_pebble_solver(net, 2);

	z3_solver.init();

	do{
		z3_solver.add_step();
	}while (z3_solver.solve() == unsat);

	CHECK(z3_solver.solve() == sat);

	//z3_solver.print();
	auto strategy = z3_solver.extract_result(false);
}

TEST_CASE(" pebble simple graph 4 nodes ", "[pebble simple graph 4 nodes]")
{
	using namespace caterpillar;
	auto net = abstract_network();

	auto p1 = net.create_pi();
	auto p2 = net.create_pi();
	auto p3 = net.create_pi();
	auto p4 = net.create_pi();
	auto p5 = net.create_pi();

	auto n1 = net.create_node({p1, p2}, 1);
	auto n2 = net.create_node({n1, p3, p4}, 2);
	auto n3 = net.create_node({ p1, p5}, 2);
	auto n4 = net.create_node({n2, n3}, 2);

	net.create_po(n4);

	auto z3_solver = z3_pebble_solver(net, 3);

	z3_solver.init();

	do{
		z3_solver.add_step();
	}while (z3_solver.solve() == unsat);

	CHECK(z3_solver.solve() == sat);

	//z3_solver.print();
	auto strategy = z3_solver.extract_result(false);

}

TEST_CASE("pebble using weights", "[pebble using weights]")
{
	using namespace caterpillar;
	auto net = abstract_network();

	auto p1 = net.create_pi();
	auto p2 = net.create_pi();
	auto p3 = net.create_pi();

	auto n1 = net.create_node({p1}, 5);
	auto n2 = net.create_node({p2}, 1);
	auto n3 = net.create_node({p3}, 1);
	auto n4 = net.create_node({n1, n2}, 1);
	auto n5 = net.create_node({n4, n3}, 1);

	net.create_po(n5);

	auto z3_solver = z3_pebble_solver(net, 4, 19);

	z3_solver.init();

	do{
		z3_solver.add_step();
	}while (z3_solver.solve() == unsat);

	CHECK(z3_solver.solve() == sat);

	//z3_solver.print();
	auto strategy = z3_solver.extract_result(false);

}
#endif