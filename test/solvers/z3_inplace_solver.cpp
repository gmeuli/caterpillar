#include <catch.hpp>

#include<caterpillar/caterpillar.hpp>

#include <mockturtle/networks/xag.hpp>
#include <mockturtle/io/write_dot.hpp>

#include <kitty/dynamic_truth_table.hpp>

#ifdef USE_Z3
#include <z3++.h>

using namespace caterpillar;

TEST_CASE("atleastatmost", "[alam]")
{
	using namespace z3;

	context ctx;
	auto s = solver(ctx);

	expr_vector vec (ctx);

	vec.push_back( ctx.bool_const( "a" ) );
	vec.push_back( ctx.bool_const( "b" ) );
	vec.push_back( ctx.bool_const( "c" ) );

	s.add(atleast(vec, 1) && atmost(vec, 1));
	s.add (vec[1]);

	CHECK(s.check() == check_result::sat );
	
	auto m = s.get_model();
	CHECK( m.eval(ctx.bool_const("a")).is_true() == false);
	CHECK( m.eval(ctx.bool_const("b")).is_true() == true);
	CHECK( m.eval(ctx.bool_const("c")).is_true() == false);

	s.add(vec[0]);

	CHECK(s.check() == check_result::unsat);
}

TEST_CASE(" pebble inplace simple graph ", "[pebin]")
{
	

	mockturtle::xag_network net;

	auto p1 = net.create_pi();
	auto p2 = net.create_pi();
	auto p3 = net.create_pi();

	auto n1 = net.create_xor(p1, p2);
	auto n2 = net.create_and(n1, p3);

	net.create_po(n2);

	pebbling_view<mockturtle::xag_network> pnet ( net );

  auto z3_solver = z3_pebble_inplace_solver( pnet, 1 );

	z3_solver.init();

	do{
		z3_solver.add_step();
	}while (z3_solver.solve() == unsat);

	CHECK(z3_solver.solve() == sat);
	
	z3_solver.save_model();
	auto strategy = z3_solver.extract_result(false);

  CHECK( strategy.size() == 3 );

	CHECK( strategy[0].first == 4);
	CHECK( std::get<2>(strategy[0].second).target_index <= 2);

	CHECK( strategy[1].first == 5);

	CHECK( strategy[2].first == 4);
	CHECK( std::get<3>(strategy[2].second).target_index <= 2);

}


TEST_CASE("pebble inplace simple graph 2", "[pebin2]")
{

	mockturtle::xag_network net;

	auto p1 = net.create_pi();
	auto p2 = net.create_pi();
	auto p3 = net.create_pi();

	auto n1 = net.create_xor(p1, p2);
	auto n2 = net.create_xor(n1, p3);


	net.create_po(n2);

	pebbling_view<mockturtle::xag_network> pnet ( net );

  auto z3_solver = z3_pebble_inplace_solver( pnet, 1 );

	z3_solver.init();
	do{
		z3_solver.add_step();
	}while (z3_solver.solve() == unsat);

	CHECK(z3_solver.solve() == sat);
	
	z3_solver.save_model();
	auto strategy = z3_solver.extract_result(false);

	CHECK( strategy.size() == 2 );
	CHECK( strategy[0].first == 4);
	CHECK( strategy[1].first == 5);
	CHECK( std::get<2>( strategy[1].second ).target_index == 4);

}


TEST_CASE("pebble inplace simple graph 3", "[pebin3]")
{

	mockturtle::xag_network net;

	auto p1 = net.create_pi();
	auto p2 = net.create_pi();
	auto p3 = net.create_pi();

	auto n4 = net.create_xor(p1, p2);
	auto n5 = net.create_xor(n4, p3);
	auto n6 = net.create_and(n5, p3);


	net.create_po(n6);

	pebbling_view<mockturtle::xag_network> pnet ( net );

  auto z3_solver = z3_pebble_inplace_solver( pnet, 1 );

	z3_solver.init();
	do{
		z3_solver.add_step();
	}while (z3_solver.solve() == unsat);

	CHECK(z3_solver.solve() == sat);

	z3_solver.save_model();
	auto strategy = z3_solver.extract_result(false);

	CHECK( strategy.size() == 5 );

	CHECK(strategy[0].first == 4);
	CHECK(std::get<2>(strategy[0].second).target_index <= 2);
	CHECK(strategy[1].first == 5);
	CHECK(std::get<2>(strategy[1].second).target_index == 4);
	CHECK(strategy[2].first == 6);
	CHECK(strategy[3].first == 5);
	CHECK(std::get<3>(strategy[3].second).target_index == 4);	
	CHECK(strategy[4].first == 4);
	CHECK(std::get<3>(strategy[4].second).target_index <= 2);

}

TEST_CASE("pebble inplace simple graph 4", "[pebin4]")
{

	mockturtle::xag_network net;

	auto p1 = net.create_pi();
	auto p2 = net.create_pi();
	auto p3 = net.create_pi();
	auto p4 = net.create_pi();


	auto n5 = net.create_xor(p1, p2);
	auto n6 = net.create_xor(p2, p3);
	auto n7 = net.create_xor(n5, n6);
	auto n8 = net.create_and(n6, p4);
	auto n9 = net.create_and(n8, n7);


	net.create_po(n9);

	pebbling_view<mockturtle::xag_network> pnet ( net );

  auto z3_solver = z3_pebble_inplace_solver( pnet, 2 );

	z3_solver.init();
	do{
		z3_solver.add_step();
	}while (z3_solver.solve() == unsat);

	CHECK(z3_solver.solve() == sat);
	z3_solver.save_model();
	auto strategy = z3_solver.extract_result(false);

	CHECK( strategy.size() == 9 );
	CHECK( strategy[0].first == 5);
	CHECK(std::get<2>(strategy[0].second).target_index == 1);
	CHECK( strategy[1].first == 6);
	CHECK(std::get<2>(strategy[1].second).target_index == 3);
	CHECK( strategy[2].first == 7);
	CHECK(std::get<2>(strategy[2].second).target_index == 5);

	CHECK( strategy[3].first == 8);
	CHECK( strategy[4].first == 9);
	CHECK( strategy[5].first == 7);
	CHECK(std::get<3>(strategy[5].second).target_index == 5);

	CHECK( strategy[6].first == 8);
	CHECK( strategy[7].first == 5);
	CHECK(std::get<3>(strategy[7].second).target_index == 1);
	CHECK( strategy[8].first == 6);
	CHECK(std::get<3>(strategy[8].second).target_index == 3);


}
#endif