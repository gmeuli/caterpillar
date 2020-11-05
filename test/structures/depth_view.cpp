#include <catch.hpp>

#include <mockturtle/traits.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/views/depth_view.hpp>

using namespace mockturtle;


/*TEST_CASE( "compute multiplicative depth information for xags", "[depth_view]")
{
  xag_network xag;

  const auto n1 = xag.create_pi();
  const auto n2 = xag.create_pi();
  const auto n3 = xag.create_pi();

  const auto n4 = xag.create_xor(n1, n2);
  const auto n5 = xag.create_and(n2, n3);
  const auto n6 = xag.create_and(n4, n5);
  const auto n7 = xag.create_xor(n6, n1);
  const auto n8 = xag.create_and(n7, n2);

  xag.create_po( n8 );

  depth_view_params ps;
  ps.compute_m_critical_path = true;
  depth_view depth_xag{xag, ps};

  CHECK( depth_xag.m_depth() == 3 );
  CHECK( depth_xag.depth() == 4 );
  CHECK( depth_xag.m_level( xag.get_node(n1) ) == 0 );
  CHECK( depth_xag.m_level( xag.get_node(n2) ) == 0 );
  CHECK( depth_xag.m_level( xag.get_node(n3) ) == 0 );
  CHECK( depth_xag.m_level( xag.get_node(n4) ) == 0 );
  CHECK( depth_xag.m_level( xag.get_node(n5) ) == 1 );
  CHECK( depth_xag.m_level( xag.get_node(n6) ) == 2 );
  CHECK( depth_xag.m_level( xag.get_node(n7) ) == 2 );
  CHECK( depth_xag.m_level( xag.get_node(n8) ) == 3 );

  CHECK( !depth_xag.is_on_critical_m_path( xag.get_node(n1) ) );
  CHECK( !depth_xag.is_on_critical_m_path( xag.get_node(n2) ) );
  CHECK( !depth_xag.is_on_critical_m_path( xag.get_node(n3) ) );
  CHECK( !depth_xag.is_on_critical_m_path( xag.get_node(n4) ) );
  CHECK( depth_xag.is_on_critical_m_path( xag.get_node(n5) ) );
  CHECK( depth_xag.is_on_critical_m_path( xag.get_node(n6) ) );
  CHECK( !depth_xag.is_on_critical_m_path( xag.get_node(n7) ) );
  CHECK( depth_xag.is_on_critical_m_path( xag.get_node(n8) ) );

}

TEST_CASE( "compute multiplicative depth information for xags 2", "[depth_view]")
{
  xag_network xag;

  const auto n1 = xag.create_pi();
  const auto n2 = xag.create_pi();
  const auto n3 = xag.create_pi();

  const auto n4 = xag.create_and(n1, n2);
  const auto n5 = xag.create_and(n2, n3);
  const auto n6 = xag.create_and(n4, n5);
 

  xag.create_po( n6 );

  depth_view_params ps;
  ps.compute_m_critical_path = true;
  depth_view depth_xag{xag, ps};

  CHECK( depth_xag.m_depth() == 2 );
  CHECK( depth_xag.depth() == 2 );
  CHECK( depth_xag.m_level( xag.get_node(n1) ) == 0 );
  CHECK( depth_xag.m_level( xag.get_node(n2) ) == 0 );
  CHECK( depth_xag.m_level( xag.get_node(n3) ) == 0 );
  CHECK( depth_xag.m_level( xag.get_node(n4) ) == 1 );
  CHECK( depth_xag.m_level( xag.get_node(n5) ) == 1 );
  CHECK( depth_xag.m_level( xag.get_node(n6) ) == 2 );

  CHECK( !depth_xag.is_on_critical_m_path( xag.get_node(n1) ) );
  CHECK( !depth_xag.is_on_critical_m_path( xag.get_node(n2) ) );
  CHECK( !depth_xag.is_on_critical_m_path( xag.get_node(n3) ) );
  CHECK( depth_xag.is_on_critical_m_path( xag.get_node(n4) ) );
  CHECK( depth_xag.is_on_critical_m_path( xag.get_node(n5) ) );
  CHECK( depth_xag.is_on_critical_m_path( xag.get_node(n6) ) );

}
*/