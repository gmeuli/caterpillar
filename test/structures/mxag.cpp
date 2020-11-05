#include "catch.hpp"
#include <caterpillar/structures/mxag.hpp>
#include <algorithm>
#include <vector>

#include <kitty/algorithm.hpp>
#include <kitty/bit_operations.hpp>
#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/operations.hpp>
#include <kitty/operators.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/traits.hpp>

using namespace caterpillar;
using namespace mockturtle;


TEST_CASE("create multi xor", "[create multixor]")
{
  mxag_network mxag;
  using signal = mxag_network::signal;

  signal x1 = mxag.create_pi();
  signal x2 = mxag.create_pi();
  signal x3 = mxag.create_pi();
  CHECK(mxag.size() == 4);

  mxag.create_xor(x1, x2);
  CHECK(mxag.size() == 5);

  auto bx = mxag.create_multi_xor({x1, x2, !x3});
  CHECK(mxag.size() == 6);

  std::vector<signal> children;
  mxag.foreach_fanin( mxag.get_node( bx ), [&] (const auto s)
  {
    children.push_back(s);
  });

  CHECK(children[0] == x2);
  CHECK(children[1] == x1);
  CHECK(children[2] == !x3);
}

TEST_CASE( "create and use constants in an mxag", "[mxag]" )
{
  mxag_network mxag;

  CHECK( mxag.size() == 1 );
  CHECK( has_get_constant_v<mxag_network> );
  CHECK( has_is_constant_v<mxag_network> );
  CHECK( has_get_node_v<mxag_network> );
  CHECK( has_is_complemented_v<mxag_network> );
  
  const auto c0 = mxag.get_constant( false );
  
  CHECK( mxag.is_constant( mxag.get_node( c0 ) ) );
  
  CHECK( !mxag.is_pi( mxag.get_node( c0 ) ) );
  
  CHECK( mxag.size() == 1 );
  CHECK( std::is_same_v<std::decay_t<decltype( c0 )>, mxag_network::signal> );
  CHECK( mxag.get_node( c0 ) == 0 );
  CHECK( !mxag.is_complemented( c0 ) );

  const auto c1 = mxag.get_constant( true );

  CHECK( mxag.get_node( c1 ) == 0 );
  CHECK( mxag.is_complemented( c1 ) );

  CHECK( c0 != c1 );
  CHECK( c0 == !c1 );
  CHECK( ( !c0 ) == c1 );
  CHECK( ( !c0 ) != !c1 );
  CHECK( -c0 == c1 );
  CHECK( -c1 == c1 );
  CHECK( c0 == +c1 );
  CHECK( c0 == +c0 );
}

TEST_CASE( "special cases in mxags", "[mxag]" )
{
  mxag_network mxag;
  auto x = mxag.create_pi();

  CHECK( mxag.create_xor( mxag.get_constant( false ), mxag.get_constant( false ) ) == mxag.get_constant( false ) );
  CHECK( mxag.create_xor( mxag.get_constant( false ), mxag.get_constant( true ) ) == mxag.get_constant( true ) );
  CHECK( mxag.create_xor( mxag.get_constant( true ), mxag.get_constant( false ) ) == mxag.get_constant( true ) );
  CHECK( mxag.create_xor( mxag.get_constant( true ), mxag.get_constant( true ) ) == mxag.get_constant( false ) );

  CHECK( mxag.create_and( mxag.get_constant( false ), mxag.get_constant( false ) ) == mxag.get_constant( false ) );
  CHECK( mxag.create_and( mxag.get_constant( false ), mxag.get_constant( true ) ) == mxag.get_constant( false ) );
  CHECK( mxag.create_and( mxag.get_constant( true ), mxag.get_constant( false ) ) == mxag.get_constant( false ) );
  CHECK( mxag.create_and( mxag.get_constant( true ), mxag.get_constant( true ) ) == mxag.get_constant( true ) );

  CHECK( mxag.create_xor( !x, mxag.get_constant( false ) ) == !x );
  CHECK( mxag.create_xor( !x, mxag.get_constant( true ) ) == x );
  CHECK( mxag.create_xor( x, mxag.get_constant( false ) ) == x );
  CHECK( mxag.create_xor( x, mxag.get_constant( true ) ) == !x );

  CHECK( mxag.create_and( !x, mxag.get_constant( false ) ) == mxag.get_constant( false ) );
  CHECK( mxag.create_and( !x, mxag.get_constant( true ) ) == !x );
  CHECK( mxag.create_and( x, mxag.get_constant( false ) ) == mxag.get_constant( false ) );
  CHECK( mxag.create_and( x, mxag.get_constant( true ) ) == x );

  CHECK( mxag.create_xor( x, x ) == mxag.get_constant( false ) );
  CHECK( mxag.create_xor( !x, x ) == mxag.get_constant( true ) );
  CHECK( mxag.create_xor( x, !x ) == mxag.get_constant( true ) );
  CHECK( mxag.create_xor( !x, !x ) == mxag.get_constant( false ) );

  CHECK( mxag.create_and( x, x ) == x );
  CHECK( mxag.create_and( !x, x ) == mxag.get_constant( false ) );
  CHECK( mxag.create_and( x, !x ) == mxag.get_constant( false ) );
  CHECK( mxag.create_and( !x, !x ) == !x );
}

TEST_CASE( "create and use primary inputs in an mxag", "[mxag]" )
{
  mxag_network mxag;

  CHECK( has_create_pi_v<mxag_network> );

  auto a = mxag.create_pi();

  CHECK( mxag.size() == 2 );
  CHECK( mxag.num_pis() == 1 );

  CHECK( std::is_same_v<std::decay_t<decltype( a )>, mxag_network::signal> );

  CHECK( a.index == 1 );
  CHECK( a.complement == 0 );

  a = !a;

  CHECK( a.index == 1 );
  CHECK( a.complement == 1 );

  a = +a;

  CHECK( a.index == 1 );
  CHECK( a.complement == 0 );

  a = +a;

  CHECK( a.index == 1 );
  CHECK( a.complement == 0 );

  a = -a;

  CHECK( a.index == 1 );
  CHECK( a.complement == 1 );

  a = -a;

  CHECK( a.index == 1 );
  CHECK( a.complement == 1 );

  a = a ^ true;

  CHECK( a.index == 1 );
  CHECK( a.complement == 0 );

  a = a ^ true;

  CHECK( a.index == 1 );
  CHECK( a.complement == 1 );
}

TEST_CASE( "create and use primary outputs in an mxag", "[mxag]" )
{
  mxag_network mxag;

  CHECK( has_create_po_v<mxag_network> );

  const auto c0 = mxag.get_constant( false );
  const auto x1 = mxag.create_pi();

  CHECK( mxag.size() == 2 );
  CHECK( mxag.num_pis() == 1 );
  CHECK( mxag.num_pos() == 0 );

  mxag.create_po( c0 );
  mxag.create_po( x1 );
  mxag.create_po( !x1 );

  CHECK( mxag.size() == 2 );
  CHECK( mxag.num_pos() == 3 );

  mxag.foreach_po( [&]( auto s, auto i ) {
    switch ( i )
    {
    case 0:
      CHECK( s == c0 );
      break;
    case 1:
      CHECK( s == x1 );
      break;
    case 2:
      CHECK( s == !x1 );
      break;
    }
  } );
}


TEST_CASE( "create unary operations in an mxag", "[mxag]" )
{
  mxag_network mxag;

  CHECK( has_create_buf_v<mxag_network> );
  CHECK( has_create_not_v<mxag_network> );

  auto x1 = mxag.create_pi();

  CHECK( mxag.size() == 2 );

  auto f1 = mxag.create_buf( x1 );
  auto f2 = mxag.create_not( x1 );

  CHECK( mxag.size() == 2 );
  CHECK( f1 == x1 );
  CHECK( f2 == !x1 );
}

TEST_CASE( "create binary operations in an mxag", "[mxag]" )
{
  mxag_network mxag;

  CHECK( has_create_and_v<mxag_network> );
  CHECK( has_create_nand_v<mxag_network> );
  CHECK( has_create_or_v<mxag_network> );
  CHECK( has_create_nor_v<mxag_network> );
  CHECK( has_create_xor_v<mxag_network> );
  CHECK( has_create_xnor_v<mxag_network> );

  const auto x1 = mxag.create_pi();
  const auto x2 = mxag.create_pi();

  CHECK( mxag.size() == 3 );

  const auto f1 = mxag.create_and( x1, x2 );
  CHECK( mxag.size() == 4 );

  const auto f2 = mxag.create_nand( x1, x2 );
  CHECK( mxag.size() == 4 );
  CHECK( f1 == !f2 );

  const auto f3 = mxag.create_or( x1, x2 );
  CHECK( mxag.size() == 5 );

  const auto f4 = mxag.create_nor( x1, x2 );
  CHECK( mxag.size() == 5 );
  CHECK( f3 == !f4 );

  const auto f5 = mxag.create_xor( x1, x2 );
  CHECK( mxag.size() == 6 );

  const auto f6 = mxag.create_xnor( x1, x2 );
  CHECK( mxag.size() == 6 );
  CHECK( f5 == !f6 );
}

TEST_CASE( "hash nodes in mxag network", "[mxag]" )
{
  mxag_network mxag;

  auto a = mxag.create_pi();
  auto b = mxag.create_pi();

  auto f = mxag.create_and( a, b );
  auto g = mxag.create_and( a, b );

  CHECK( mxag.size() == 4u );
  CHECK( mxag.num_gates() == 1u );

  CHECK( mxag.get_node( f ) == mxag.get_node( g ) );
}
/*
TEST_CASE( "clone a node in mxag network", "[mxag]" )
{
  mxag_network mxag1, mxag2;

  CHECK( has_clone_node_v<mxag_network> );

  auto a1 = mxag1.create_pi();
  auto b1 = mxag1.create_pi();
  auto f1 = mxag1.create_and( a1, b1 );
  CHECK( mxag1.size() == 4 );

  auto a2 = mxag2.create_pi();
  auto b2 = mxag2.create_pi();
  CHECK( mxag2.size() == 3 );

  auto f2 = mxag2.clone_node( mxag1, mxag1.get_node( f1 ), {a2, b2} );
  CHECK( mxag2.size() == 4 );

  mxag2.foreach_fanin( mxag2.get_node( f2 ), [&]( auto const& s, auto ) {
    CHECK( !mxag2.is_complemented( s ) );
  } );
}
*/
TEST_CASE( "structural properties of an mxag", "[mxag]" )
{
  mxag_network mxag;

  CHECK( has_size_v<mxag_network> );
  CHECK( has_num_pis_v<mxag_network> );
  CHECK( has_num_pos_v<mxag_network> );
  CHECK( has_num_gates_v<mxag_network> );
  CHECK( has_fanin_size_v<mxag_network> );
  CHECK( has_fanout_size_v<mxag_network> );

  const auto x1 = mxag.create_pi();
  const auto x2 = mxag.create_pi();

  const auto f1 = mxag.create_and( x1, x2 );
  const auto f2 = mxag.create_xor( x1, x2 );

  mxag.create_po( f1 );
  mxag.create_po( f2 );

  CHECK( mxag.size() == 5 );
  CHECK( mxag.is_and( mxag.get_node( f1 ) ) == true );
  CHECK( mxag.is_xor( mxag.get_node( f1 ) ) == false );
  CHECK( mxag.num_pis() == 2 );
  CHECK( mxag.num_pos() == 2 );
  CHECK( mxag.num_gates() == 2 );
  CHECK( mxag.fanin_size( mxag.get_node( x1 ) ) == 0 );
  CHECK( mxag.fanin_size( mxag.get_node( x2 ) ) == 0 );
  CHECK( mxag.fanin_size( mxag.get_node( f1 ) ) == 2 );
  CHECK( mxag.fanin_size( mxag.get_node( f2 ) ) == 2 );
  CHECK( mxag.fanout_size( mxag.get_node( x1 ) ) == 2 );
  CHECK( mxag.fanout_size( mxag.get_node( x2 ) ) == 2 );
  CHECK( mxag.fanout_size( mxag.get_node( f1 ) ) == 1 );
  CHECK( mxag.fanout_size( mxag.get_node( f2 ) ) == 1 );
}

TEST_CASE( "node and signal iteration in an mxag", "[mxag]" )
{
  mxag_network mxag;

  CHECK( has_foreach_node_v<mxag_network> );
  CHECK( has_foreach_pi_v<mxag_network> );
  CHECK( has_foreach_po_v<mxag_network> );
  CHECK( has_foreach_gate_v<mxag_network> );
  CHECK( has_foreach_fanin_v<mxag_network> );

  const auto x1 = mxag.create_pi();
  const auto x2 = mxag.create_pi();
  const auto f1 = mxag.create_and( x1, x2 );
  const auto f2 = mxag.create_or( x1, x2 );
  mxag.create_po( f1 );
  mxag.create_po( f2 );

  CHECK( mxag.size() == 5 );

  // iterate over nodes 
  uint32_t mask{0}, counter{0};
  mxag.foreach_node( [&]( auto n, auto i ) { mask |= ( 1 << n ); counter += i; } );
  CHECK( mask == 31 );
  CHECK( counter == 10 );

  mask = 0;
  mxag.foreach_node( [&]( auto n ) { mask |= ( 1 << n ); } );
  CHECK( mask == 31 );

  mask = counter = 0;
  mxag.foreach_node( [&]( auto n, auto i ) { mask |= ( 1 << n ); counter += i; return false; } );
  CHECK( mask == 1 );
  CHECK( counter == 0 );

  mask = 0;
  mxag.foreach_node( [&]( auto n ) { mask |= ( 1 << n ); return false; } );
  CHECK( mask == 1 );

  // iterate over PIs 
  mask = counter = 0;
  mxag.foreach_pi( [&]( auto n, auto i ) { mask |= ( 1 << n ); counter += i; } );
  CHECK( mask == 6 );
  CHECK( counter == 1 );

  mask = 0;
  mxag.foreach_pi( [&]( auto n ) { mask |= ( 1 << n ); } );
  CHECK( mask == 6 );

  mask = counter = 0;
  mxag.foreach_pi( [&]( auto n, auto i ) { mask |= ( 1 << n ); counter += i; return false; } );
  CHECK( mask == 2 );
  CHECK( counter == 0 );

  mask = 0;
  mxag.foreach_pi( [&]( auto n ) { mask |= ( 1 << n ); return false; } );
  CHECK( mask == 2 );

  // iterate over POs 
  mask = counter = 0;
  mxag.foreach_po( [&]( auto s, auto i ) { mask |= ( 1 << mxag.get_node( s ) ); counter += i; } );
  CHECK( mask == 24 );
  CHECK( counter == 1 );

  mask = 0;
  mxag.foreach_po( [&]( auto s ) { mask |= ( 1 << mxag.get_node( s ) ); } );
  CHECK( mask == 24 );

  mask = counter = 0;
  mxag.foreach_po( [&]( auto s, auto i ) { mask |= ( 1 << mxag.get_node( s ) ); counter += i; return false; } );
  CHECK( mask == 8 );
  CHECK( counter == 0 );

  mask = 0;
  mxag.foreach_po( [&]( auto s ) { mask |= ( 1 << mxag.get_node( s ) ); return false; } );
  CHECK( mask == 8 );

  // iterate over gates 
  mask = counter = 0;
  mxag.foreach_gate( [&]( auto n, auto i ) { mask |= ( 1 << n ); counter += i; } );
  CHECK( mask == 24 );
  CHECK( counter == 1 );

  mask = 0;
  mxag.foreach_gate( [&]( auto n ) { mask |= ( 1 << n ); } );
  CHECK( mask == 24 );

  mask = counter = 0;
  mxag.foreach_gate( [&]( auto n, auto i ) { mask |= ( 1 << n ); counter += i; return false; } );
  CHECK( mask == 8 );
  CHECK( counter == 0 );

  mask = 0;
  mxag.foreach_gate( [&]( auto n ) { mask |= ( 1 << n ); return false; } );
  CHECK( mask == 8 );

  // iterate over fanins 
  mask = counter = 0;
  mxag.foreach_fanin( mxag.get_node( f1 ), [&]( auto s, auto i ) { mask |= ( 1 << mxag.get_node( s ) ); counter += i; } );
  CHECK( mask == 6 );
  CHECK( counter == 1 );

  mask = 0;
  mxag.foreach_fanin( mxag.get_node( f1 ), [&]( auto s ) { mask |= ( 1 << mxag.get_node( s ) ); } );
  CHECK( mask == 6 );

  mask = counter = 0;
  mxag.foreach_fanin( mxag.get_node( f1 ), [&]( auto s, auto i ) { mask |= ( 1 << mxag.get_node( s ) ); counter += i; return false; } );
  CHECK( mask == 2 );
  CHECK( counter == 0 );

  mask = 0;
  mxag.foreach_fanin( mxag.get_node( f1 ), [&]( auto s ) { mask |= ( 1 << mxag.get_node( s ) ); return false; } );
  CHECK( mask == 2 );
}
/*
TEST_CASE( "compute values in mxags", "[mxag]" )
{
  mxag_network mxag;

  CHECK( has_compute_v<mxag_network, bool> );
  CHECK( has_compute_v<mxag_network, kitty::dynamic_truth_table> );

  const auto x1 = mxag.create_pi();
  const auto x2 = mxag.create_pi();
  const auto f1 = mxag.create_and( !x1, x2 );
  const auto f2 = mxag.create_and( x1, !x2 );
  mxag.create_po( f1 );
  mxag.create_po( f2 );

  std::vector<bool> values{{true, false}};

  CHECK( mxag.compute( mxag.get_node( f1 ), values.begin(), values.end() ) == false );
  CHECK( mxag.compute( mxag.get_node( f2 ), values.begin(), values.end() ) == true );

  std::vector<kitty::dynamic_truth_table> xs{2, kitty::dynamic_truth_table( 2 )};
  kitty::create_nth_var( xs[0], 0 );
  kitty::create_nth_var( xs[1], 1 );

  CHECK( mxag.compute( mxag.get_node( f1 ), xs.begin(), xs.end() ) == ( ~xs[0] & xs[1] ) );
  CHECK( mxag.compute( mxag.get_node( f2 ), xs.begin(), xs.end() ) == ( xs[0] & ~xs[1] ) );
}
*/
TEST_CASE( "custom node values in mxags", "[mxag]" )
{
  mxag_network mxag;

  CHECK( has_clear_values_v<mxag_network> );
  CHECK( has_value_v<mxag_network> );
  CHECK( has_set_value_v<mxag_network> );
  CHECK( has_incr_value_v<mxag_network> );
  CHECK( has_decr_value_v<mxag_network> );

  const auto x1 = mxag.create_pi();
  const auto x2 = mxag.create_pi();
  const auto f1 = mxag.create_and( x1, x2 );
  const auto f2 = mxag.create_or( x1, x2 );
  mxag.create_po( f1 );
  mxag.create_po( f2 );

  CHECK( mxag.size() == 5 );

  mxag.clear_values();
  mxag.foreach_node( [&]( auto n ) {
    CHECK( mxag.value( n ) == 0 );
    mxag.set_value( n, n );
    CHECK( mxag.value( n ) == n );
    CHECK( mxag.incr_value( n ) == n );
    CHECK( mxag.value( n ) == n + 1 );
    CHECK( mxag.decr_value( n ) == n );
    CHECK( mxag.value( n ) == n );
  } );
  mxag.clear_values();
  mxag.foreach_node( [&]( auto n ) {
    CHECK( mxag.value( n ) == 0 );
  } );
}

TEST_CASE( "visited values in mxags", "[mxag]" )
{
  mxag_network mxag;

  CHECK( has_clear_visited_v<mxag_network> );
  CHECK( has_visited_v<mxag_network> );
  CHECK( has_set_visited_v<mxag_network> );

  const auto x1 = mxag.create_pi();
  const auto x2 = mxag.create_pi();
  const auto f1 = mxag.create_and( x1, x2 );
  const auto f2 = mxag.create_or( x1, x2 );
  mxag.create_po( f1 );
  mxag.create_po( f2 );

  CHECK( mxag.size() == 5 );

  mxag.clear_visited();
  mxag.foreach_node( [&]( auto n ) {
    CHECK( mxag.visited( n ) == 0 );
    mxag.set_visited( n, static_cast<uint32_t>( n ) );
    CHECK( mxag.visited( n ) == static_cast<uint32_t>( n ) );
  } );
  mxag.clear_visited();
  mxag.foreach_node( [&]( auto n ) {
    CHECK( mxag.visited( n ) == 0 );
  } );
}
/*
TEST_CASE( "simulate some special functions in mxags", "[mxag]" )
{
  mxag_network mxag;
  const auto x1 = mxag.create_pi();
  const auto x2 = mxag.create_pi();
  const auto x3 = mxag.create_pi();

  const auto f1 = mxag.create_maj( x1, x2, x3 );
  const auto f2 = mxag.create_ite( x1, x2, x3 );

  mxag.create_po( f1 );
  mxag.create_po( f2 );

  CHECK( mxag.num_gates() == 7u );

  auto result = simulate<kitty::dynamic_truth_table>( mxag, default_simulator<kitty::dynamic_truth_table>( 3 ) );

  CHECK( result[0]._bits[0] == 0xe8u );
  CHECK( result[1]._bits[0] == 0xd8u );
}

TEST_CASE( "create nary functions in mxags", "[mxag]" )
{
  mxag_network mxag;
  std::vector<mxag_network::signal> pis( 8u );
  std::generate( pis.begin(), pis.end(), [&]() { return mxag.create_pi(); } );
  mxag.create_po( mxag.create_nary_and( pis ) );
  mxag.create_po( mxag.create_nary_or( pis ) );
  mxag.create_po( mxag.create_nary_xor( pis ) );

  CHECK( mxag.num_gates() == 21u );

  auto result = simulate<kitty::dynamic_truth_table>( mxag, default_simulator<kitty::dynamic_truth_table>( 8 ) );

  CHECK( kitty::count_ones( result[0] ) == 1u );
  CHECK( kitty::get_bit( result[0], 255 ) );

  CHECK( kitty::count_ones( result[1] ) == 255u );
  CHECK( !kitty::get_bit( result[1], 0 ) );

  auto copy = result[2].construct();
  kitty::create_parity( copy );
  CHECK( result[2] == copy );
}
*/