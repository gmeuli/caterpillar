#include <algorithm>
#include <cstdint>
#include <iostream>

#include <kitty/spectral.hpp>
#include <lorina/bench.hpp>
#include <mockturtle/io/bench_reader.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/algorithms/collapse_mapped.hpp>

#include <caterpillar/lhrs.hpp>
#include <mockturtle/generators/arithmetic.hpp>
#include <mockturtle/views/mapping_view.hpp>

#include <tweedledum/networks/netlist.hpp>
#include <tweedledum/gates/mcst_gate.hpp>


int main( int argc, char** argv )
{
  using namespace caterpillar;
  namespace mc = mockturtle;
  namespace td = tweedledum;

  mc::aig_network aig;
  td::netlist<td::mcst_gate> qnet;

  const auto a = aig.create_pi();
  const auto b = aig.create_pi();
  const auto c = aig.create_pi();

  auto [sum, carry] = mc::full_adder( aig, a, b, c );

  aig.create_po( sum );
  aig.create_po( carry );

  mc::mapping_view<mc::aig_network, true> mapped_aig{ aig };

  mc::lut_mapping(mapped_aig);

  auto lutn = mc::collapse_mapped_network<mc::klut_network>( mapped_aig );

  if(lutn) logic_network_synthesis( qnet, *lutn, td::stg_from_pkrm());
  

  return 0;
}
