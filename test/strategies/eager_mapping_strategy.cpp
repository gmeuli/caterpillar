#include <catch.hpp>

#include <cstdint>

#include <caterpillar/stg_gate.hpp>
#include <caterpillar/synthesis/lhrs.hpp>
#include <caterpillar/synthesis/strategies/eager_mapping_strategy.hpp>
#include <caterpillar/verification/circuit_to_logic_network.hpp>
#include <kitty/static_truth_table.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/networks/aig.hpp>
#include <tweedledum/io/write_unicode.hpp>
#include <tweedledum/networks/netlist.hpp>

TEST_CASE( "Eager mapping strategy for 3-bit sorting network", "[eager_mapping_strategy]" )
{
  using namespace caterpillar;
  using namespace caterpillar::detail;
  using namespace mockturtle;
  using namespace tweedledum;

  aig_network sorter;
  const auto a = sorter.create_pi();
  const auto b = sorter.create_pi();
  const auto c = sorter.create_pi();

  const auto w1 = sorter.create_and( a, b );
  const auto w2 = sorter.create_and( c, w1 );
  const auto w3 = sorter.create_and( !a, !b );
  const auto w4 = sorter.create_and( !c, !w1 );
  const auto w5 = sorter.create_and( !w3, !w4 );
  const auto w6 = sorter.create_or( c, !w3 );

  sorter.create_po( w2 );
  sorter.create_po( w5 );
  sorter.create_po( w6 );

  eager_mapping_strategy<aig_network> strategy;
  CHECK( strategy.compute_steps( sorter ) );
  uint32_t compute{0u}, uncompute{0u};

  strategy.foreach_step( [&]( auto, auto a ) {
    std::visit(
        overloaded{
            []( auto ) {},
            [&]( compute_action const& ) {
              ++compute;
            },
            [&]( uncompute_action const& ) {
              ++uncompute;
            },
            [&]( compute_inplace_action const& ) {
              CHECK( false );
            },
            [&]( uncompute_inplace_action const& ) {
              CHECK( false );
            }},
        a );
  } );

  CHECK( compute == 6u );
  CHECK( uncompute == 3u );

  netlist<stg_gate> circ;
  eager_mapping_strategy<aig_network> strategy2;
  logic_network_synthesis_stats st;
  logic_network_synthesis(circ, sorter, strategy2, {}, {}, &st);

  const auto sorter2 = circuit_to_logic_network<aig_network>(circ, st.i_indexes, st.o_indexes);
  CHECK( sorter2 );
  CHECK( simulate<kitty::static_truth_table<3>>( sorter ) == simulate<kitty::static_truth_table<3>>( *sorter2 ) );

  //write_unicode(circ, false);
}
