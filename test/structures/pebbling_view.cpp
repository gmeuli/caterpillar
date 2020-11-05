#include "catch.hpp"
#include <caterpillar/caterpillar.hpp>

using namespace caterpillar;
using namespace mockturtle;

TEST_CASE("create and use pebbling view", "[pw]")
{
  xag_network xag;

  auto n1 = xag.create_pi();
  auto n2 = xag.create_pi();
  auto n3 = xag.create_pi();
  auto n4 = xag.create_pi();

  auto n5 = xag.create_xor(n1, n2);
  auto n6 = xag.create_and( n2, n4);
  auto n7 = xag.create_xor(n5, n6);
  auto n8 = xag.create_and(n7, n3);

  xag.create_po(n8);

  pebbling_view pxag (xag);

  pxag.set_weight(pxag.get_node(n5), 4);
  CHECK(pxag.get_weight(pxag.get_node(n5)) == 4);

  CHECK(pxag.get_parents(pxag.get_node(n5)).size() == 1);
  CHECK(pxag.get_parents(pxag.get_node(n1)).size() == 1);
  CHECK(pxag.get_parents(pxag.get_node(n2)).size() == 2);
  CHECK(pxag.get_parents(pxag.get_node(n8)).size() == 0);
  
}