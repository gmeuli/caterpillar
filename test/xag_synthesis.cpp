#include <catch.hpp>
#include <caterpillar/synthesis/xag_synthesis.hpp>
#include <mockturtle/networks/xag.hpp>
#include <tweedledum/networks/netlist.hpp>
#include <tweedledum/io/write_unicode.hpp>
#include<mockturtle/io/write_bench.hpp>
#include <caterpillar/stg_gate.hpp>


TEST_CASE("synthesize simple xag", "[XAG synthesis]")
{
  auto xag = mockturtle::xag_network();

  auto a = xag.create_pi();
  auto b = xag.create_pi();
  auto c = xag.create_pi();
  auto d = xag.create_and( a, b );
  auto e = xag.create_xor( d, c );
  xag.create_po( e );

  mockturtle::write_bench( xag, std::cout );

  tweedledum::netlist<caterpillar::stg_gate> qnet;
  caterpillar::xag_heuristic_synthesis( xag, qnet );

  std::cout << tweedledum::to_unicode_str( qnet ) << std::endl;
  
}
