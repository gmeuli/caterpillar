#include <caterpillar/synthesis/lhrs.hpp>
#include <caterpillar/synthesis/xag_tracer.hpp>
#include <caterpillar/synthesis/strategies/xag_mapping_strategy.hpp>
#include <caterpillar/synthesis/strategies/abstract_xag_mapping_strategy.hpp>

#include <caterpillar/verification/circuit_to_logic_network.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/algorithms/cleanup.hpp>
#include <tweedledum/networks/netlist.hpp>
#include <tweedledum/io/write_unicode.hpp>
#include <tweedledum/io/write_projectq.hpp>

#include <mockturtle/io/write_verilog.hpp>
#include <mockturtle/io/write_dot.hpp>

#include <caterpillar/structures/stg_gate.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <fmt/format.h>
#pragma once
namespace caterpillar::test
{

enum class xag_method { 
  xag_lowt,
  xag_lowt_fast,
  xag_lowd,
  xag_pebb ,
  abs_xag_lowt,
  abs_xag_lowt_fast,
  abs_xag_lowd,
};

inline static mockturtle::xag_network get_xag(uint32_t const& val)
{
  mockturtle::xag_network xag;
  if(val == 1)
  {
    /* synthesize simple xag */
    auto a = xag.create_pi();
    auto b = xag.create_pi();
    auto c = xag.create_pi();
    auto d = xag.create_and( a, b );
    auto e = xag.create_xor( d, c );
    xag.create_po( e );
  }
  else if(val == 2)
  {
    auto a = xag.create_pi();
    auto b = xag.create_pi();
    auto c = xag.create_pi();
    auto d = xag.create_pi();
    auto e = xag.create_xor(a, b);
    auto f = xag.create_xor(e, c);
    auto g = xag.create_and(a, f);
    auto h = xag.create_and(g, d);
    auto i = xag.create_xor(h, d);
    xag.create_po( i ); 
  }
  else if(val == 3)
  {
    auto a = xag.create_pi();
    auto b = xag.create_pi();
    auto c = xag.create_pi();
    auto d = xag.create_pi();
    auto e = xag.create_xor(a, b);
    auto f = xag.create_xor(e, c);
    auto g = xag.create_xor(a, f);
    auto h = xag.create_and(g, d);
    xag.create_po( h ); 
  }
  else if(val == 4)
  {
    auto s = xag.create_pi();
    auto t5 = xag.create_pi();
    auto t2 = xag.create_pi();
    auto t15 = xag.create_pi();
    auto t12 = xag.create_pi();
    auto t8 = xag.create_pi();
    auto t7 = xag.create_pi();
    auto t13 = xag.create_pi();

    auto t6 = xag.create_xor(t5, t2);
    auto t16 = xag.create_xor(t15, t12);
    auto t18 = xag.create_xor(t6, t16);
    auto t9 = xag.create_xor(t7, t8);
    auto t14 = xag.create_xor(t13, t12);
    auto t19 = xag.create_xor(t9, t14);
    auto t22 = xag.create_xor(t18, t19);
    auto s0 = xag.create_and(t22, s);

    xag.create_po( s0 );
  }
  else if(val == 5)
  {
    auto x1 = xag.create_pi();
    auto x2 = xag.create_pi();
    auto x3 = xag.create_pi();
    auto x4 = xag.create_pi();
    auto x5 = xag.create_pi();
    auto n6 = xag.create_xor(x5, x1);
    auto n7 = xag.create_xor(x4, x2);
    auto n8 = xag.create_xor(n6, n7);
    auto n9 = xag.create_xor(n8, x3);
    auto n10 = xag.create_xor(n9, x4);
    auto n11 = xag.create_and(n8, n10);
    xag.create_po(n11);
  }
  else if(val == 6)
  {
    auto x0 = xag.create_pi();
    auto x1 = xag.create_pi();
    auto x2 = xag.create_pi();
    auto x3 = xag.create_pi();
    auto x4 = xag.create_pi();
    auto x5 = xag.create_pi();
    auto x6 = xag.create_pi();

    auto n10 = xag.create_xor(x6, x0);
    auto n9 = xag.create_xor(x5, x3);
    auto n16 = xag.create_xor(n10, n9);
    auto n20 = xag.create_xor(n16, x4);
    auto n22 = xag.create_xor(n20, x1);
    auto n11 = xag.create_xor(x3, x0);
    auto n25 = xag.create_xor(n22, n11);
    auto n13 = xag.create_xor(x2, x1);
    auto n29 = xag.create_xor(n25, n13);
    auto n37 = xag.create_xor(n10, n29);
    xag.create_po(n37);
  }
  else if(val == 7)
  {
    /*synthesize simple xag with codependent xor outputs*/
    auto x0 = xag.create_pi();
    auto x1 = xag.create_pi();
    auto x2 = xag.create_pi();
    auto x3 = xag.create_pi();
    auto x4 = xag.create_pi();
    auto x5 = xag.create_pi();
    auto x6 = xag.create_pi();
    auto x7 = xag.create_pi();
    auto x8 = xag.create_pi();

    auto z0 = xag.create_xor(x1 , x2 );
    auto z1 = xag.create_xor(x3 , x4 );
    auto z2 = xag.create_xor(z0 , z1 );
    auto z3 = xag.create_xor(x0 , z2 );
    auto z4 = xag.create_xor(x6 , z1 );
    auto z5 = xag.create_xor(x5 , z4 );
    auto z6 = xag.create_xor(z5 , z3 );
    auto z7 = xag.create_xor(x7 , x1 );
    auto z8 = xag.create_xor(x8 , x3 );
    auto z9 = xag.create_xor(z7 , z8 );
    auto z10 = xag.create_xor(z9 , z3 );
    
    xag.create_po(z3);
    xag.create_po(!z6);
    xag.create_po(z10);
  }
  else if(val == 8)
  {
    /*synthesize simple xag with reconvergence*/
    auto x2 = xag.create_pi();
    auto x5 = xag.create_pi();
    auto x6 = xag.create_pi();
    auto x7 = xag.create_pi();
    auto x4 = xag.create_pi();

    auto n12 = xag.create_xor(x7, x5);
    auto n13 = xag.create_xor(n12, x6);
    auto n17 = xag.create_xor(n12, x2);
    auto n19 = xag.create_xor(n17, n13);
    auto n20 = xag.create_xor(x4, x6);
    auto n22 = xag.create_and(n19, n20);
    xag.create_po(n22);
  }
  else if(val == 9)
  {
    auto x0 = xag.create_pi();
    auto x5 = xag.create_pi();
    auto x6 = xag.create_pi();
    auto x7 = xag.create_pi();

    auto n19 = xag.create_xor(x7, x0);
    auto n20 = xag.create_and(n19, x6);
    auto n21 = xag.create_and(n19, n20);
    auto n26 = xag.create_xor(x5, x0);
    auto n27 = xag.create_xor(n26, n20);
    auto n29 = xag.create_and(x7, n27);
    auto n32 = xag.create_and(n21, n29);

    xag.create_po(n32);
  }
  else if(val == 10)
  {
    auto x1 = xag.create_pi();
    auto x2 = xag.create_pi();
    auto x6 = xag.create_pi();

    auto n13 = xag.create_xor(x2, x1);
    auto n14 = xag.create_xor(n13, x6);
    auto n18 = xag.create_xor(n14, x6);
    auto n38 = xag.create_and(n14, n18);
    
    xag.create_po(n38);
  }
  else if(val == 11)
  {
    auto x1 = xag.create_pi();
    auto x2 = xag.create_pi();
    auto x3 = xag.create_pi();

    auto n1 = xag.create_and(x1, x2);
    auto n2 = xag.create_xor(x2, x3);
    auto n3 = xag.create_xor(n1, n2);
    auto n4 = xag.create_and(n2, n3);
  
    xag.create_po(n4);
  }
  else if(val == 12)
  {
    auto x0 = xag.create_pi();
    auto x5 = xag.create_pi();
    auto x6 = xag.create_pi();
    auto x7 = xag.create_pi();

    auto n19 = xag.create_xor(x7, x0);
    auto n20 = xag.create_and(n19, x6);
    auto n21 = xag.create_and(n19, n20);
    auto n26 = xag.create_xor(x5, x0);
    auto n27 = xag.create_xor(n26, n20);
    auto n29 = xag.create_and(x7, n27);
    auto n32 = xag.create_xor(n21, n29);

    xag.create_po(n32);
  }
  else if(val == 13)
  {
    auto z0 = xag.create_pi();
    auto z1 = xag.create_pi();
    auto z3 = xag.create_pi();
    auto z4 = xag.create_pi();
    auto z6 = xag.create_pi();
    auto z7 = xag.create_pi();
    auto z9 = xag.create_pi();
    auto z10 = xag.create_pi();
    auto z15 = xag.create_pi();
    auto z16 = xag.create_pi();

    auto t46 = xag.create_xor(z15 , z16);
    auto t49 = xag.create_xor(z9 , z10);
    auto t53 = xag.create_xor(z0 , z3);
    auto t54 = xag.create_xor(z6 , z7);
    auto t58 = xag.create_xor(z4 , t46);
    auto t59 = xag.create_xor(z3 , t54);
    auto t63 = xag.create_xor(t49 , t58);
    auto t64 = xag.create_xor(z4 , t59);
    auto t66 = xag.create_xor(z1 , t63);
    auto s3 = xag.create_xor(t53 , t66);
    auto s1pre = xag.create_xor(t64 , s3);
    auto s1 = xag.create_not(s1pre);

    xag.create_po(s3);
    xag.create_po(s1);
  }
  else if(val == 14)
  {
    auto x1 = xag.create_pi();
    auto x2 = xag.create_pi();
    auto x3 = xag.create_pi();
    auto x4 = xag.create_pi();
    auto x5 = xag.create_pi();
    auto x6 = xag.create_pi();

    auto n1 = xag.create_xor(x1, x2);
    auto n2 = xag.create_and(x2, n1);
    auto n3 = xag.create_and(x2, n2);
    auto n4 = xag.create_xor(x3, x4);
    auto n5 = xag.create_xor(x5, x6);
    auto n6 = xag.create_and(n4, n5);
    auto n7 = xag.create_xor(n6, n4);
    auto n8 = xag.create_and(x3, n7);
    auto n9 = xag.create_and(n8, n3);

    xag.create_po(n9);
  }
  else if(val == 15)
  {
    auto x1 = xag.create_pi();
    auto x2 = xag.create_pi();
    auto x3 = xag.create_pi();
    auto x4 = xag.create_pi();
    auto x5 = xag.create_pi();
    auto x6 = xag.create_pi();

    auto n1 = xag.create_xor(x1, x2);
    auto n2 = xag.create_xor(x3, x4);
    auto n3 = xag.create_and(n1, n2);
    auto n4 = xag.create_xor(x4, x5);
    auto n5 = xag.create_and(n4, x6);
    auto n6 = xag.create_and(n3, n5);

    xag.create_po(n6);
  }
  else if(val == 16)
  {
    auto x1 = xag.create_pi();
    auto x2 = xag.create_pi();
    auto x3 = xag.create_pi();

    auto n1 = xag.create_and(x1, x2);
    auto n2 = xag.create_and(x2, x3);
    auto n3 = xag.create_and(!x1, x2);
    auto n4 = xag.create_xor(n1, n2);
    auto n5 = xag.create_xor(n4, n3);

    xag.create_po(n5);
  }
  else if(val == 17)
  {
    auto x1 = xag.create_pi();
    auto x2 = xag.create_pi();
    auto x3 = xag.create_pi();
    auto x4 = xag.create_pi();
    auto x5 = xag.create_pi();


    auto n1 = xag.create_xor(x1, x2);
    auto n2 = xag.create_and(n1, x3);
    auto n3 = xag.create_and(x3, x4);
    auto n4 = xag.create_and(x4, x5);
    auto n5 = xag.create_and( n2, n3);

    xag.create_po(n5);
    xag.create_po(n4);
    xag.create_po(n1);
  }
  else if(val == 18)
  {
    auto x1 = xag.create_pi();
    auto x2 = xag.create_pi();
    auto x3 = xag.create_pi();
    auto x4 = xag.create_pi();

    auto n1 = xag.create_xor(x1, x2);
    auto n2 = xag.create_xor(x2, x3);
    auto n3 = xag.create_and(n1, n2);
    auto n4 = xag.create_and(n2, x4);
    auto n5 = xag.create_and( n4, n3);

    xag.create_po(n5);
  }
  else if(val == 19)
  {
    auto x1 = xag.create_pi();
    auto x2 = xag.create_pi();
    auto x3 = xag.create_pi();
    auto x4 = xag.create_pi();

    auto n1 = xag.create_and(x1, x2);
    auto n2 = xag.create_and(x1, x3);
    auto n3 = xag.create_and(x1, x4);
    auto n4 = xag.create_and(n2, n3);
    auto n5 = xag.create_and( n4, n1);

    xag.create_po(n5);
  }
  else if(val == 20)
  {
    auto x1 = xag.create_pi();
    auto x2 = xag.create_pi();
    auto x3 = xag.create_pi();
    auto x4 = xag.create_pi();
    auto x5 = xag.create_pi();

    auto n1 = xag.create_xor(x1, x2);
    auto n2 = xag.create_xor(n1, x3);
    auto n3 = xag.create_xor(n2, x4);
    auto n4 = xag.create_xor(n3, x5);
    auto n5 = xag.create_xor(n3, n4);
    auto n5b = xag.create_xor(n5, x3);

    auto n6 = xag.create_and(n5, n5b);
    auto n7 = xag.create_and(n6, x1);
    xag.create_po(n7);
  }
  else if(val == 21)
  {
    auto x1 = xag.create_pi();
    auto x2 = xag.create_pi();
    auto x3 = xag.create_pi();
    auto x4 = xag.create_pi();

    auto n1 = xag.create_xor(x2, x3);
    auto n2 = xag.create_xor(n1, x1);
    auto n3 = xag.create_xor(n1, x4);
    auto n4 = xag.create_and(n1, n2);
    auto n5 = xag.create_and(n1, n3);
    
    xag.create_po(n5);
    xag.create_po(n4);
  }
  return xag;
}

static bool xag_synthesis(xag_method const& m, uint32_t const& num_xag, bool verbose = false, pebbling_mapping_strategy_params const& peb_ps = {} )
{
  using namespace caterpillar;
  using namespace mockturtle;
  using namespace tweedledum;

  xag_network xag = get_xag(num_xag);
  uint32_t pis = xag.num_pis();
  netlist<stg_gate> qnet;
  logic_network_synthesis_params ps;
  logic_network_synthesis_stats st;
  ps.verbose = verbose;

  if(m == xag_method::xag_lowt)
  {
    xag_mapping_strategy strategy;
    logic_network_synthesis( qnet, xag, strategy, {}, ps, &st );
    auto tt_xag = simulate<kitty::dynamic_truth_table>( xag, {pis});
    const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
    auto tt_ntk = simulate<kitty::dynamic_truth_table>( *ntk, {pis});
    if(verbose) write_unicode(qnet);
    return (tt_xag == tt_ntk);
  }
  if(m == xag_method::xag_lowt_fast)
  {
    xag_fast_lowt_mapping_strategy strategy;
    logic_network_synthesis( qnet, xag, strategy, {}, ps, &st );
    auto tt_xag = simulate<kitty::dynamic_truth_table>( xag, {pis});
    const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
    auto tt_ntk = simulate<kitty::dynamic_truth_table>( *ntk, {pis});
    if(verbose) write_unicode(qnet);
    return (tt_xag == tt_ntk);
  }
  else if(m == xag_method::xag_lowd)
  {
    xag_low_depth_mapping_strategy strategy;
    logic_network_synthesis( qnet, xag, strategy, {}, ps, &st );
    auto tt_xag = simulate<kitty::dynamic_truth_table>( xag, {pis});
    const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
    auto tt_ntk = simulate<kitty::dynamic_truth_table>( *ntk, {pis});
    if(verbose) write_unicode(qnet);
    return (tt_xag == tt_ntk);
  }
  else if(m== xag_method::xag_pebb)
  {
    #ifdef USE_Z3
    xag_pebbling_mapping_strategy strategy (peb_ps);
    logic_network_synthesis( qnet, xag, strategy, {}, ps, &st );
    auto tt_xag = simulate<kitty::dynamic_truth_table>( xag, {pis} );
    const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
    auto tt_ntk = simulate<kitty::dynamic_truth_table>( *ntk, {pis} );
    if(verbose) write_unicode(qnet);
    return (tt_xag == tt_ntk);
    #endif
    return false;
  }
  else if(m== xag_method::abs_xag_lowt)
  {
    abstract_xag_mapping_strategy strategy;
    const auto abs_xag = cleanup_dangling<xag_network, abstract_xag_network>( xag );
    logic_network_synthesis( qnet, abs_xag, strategy, {}, ps, &st );
    auto tt_xag = simulate<kitty::dynamic_truth_table>( abs_xag, {pis} );
    const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
    auto tt_ntk = simulate<kitty::dynamic_truth_table>( *ntk, {pis} );
    if(verbose) write_unicode(qnet);
    return (tt_xag == tt_ntk);
   
  }
  else if(m== xag_method::abs_xag_lowt_fast)
  {
    abstract_xag_fast_lowt_mapping_strategy strategy;
    const auto abs_xag = cleanup_dangling( cleanup_dangling<xag_network, abstract_xag_network>( xag ) );
    logic_network_synthesis( qnet, abs_xag, strategy, {}, ps, &st );
    auto tt_xag = simulate<kitty::dynamic_truth_table>( abs_xag, {pis} );
    const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
    auto tt_ntk = simulate<kitty::dynamic_truth_table>( *ntk, {pis} );
    if(verbose) write_unicode(qnet);
    return (tt_xag == tt_ntk);
   
  }
  else if(m== xag_method::abs_xag_lowd)
  {
    abstract_xag_low_depth_mapping_strategy strategy (false);
    const auto abs_xag = cleanup_dangling( cleanup_dangling<xag_network, abstract_xag_network>( xag ) );
    logic_network_synthesis( qnet, abs_xag, strategy, {}, ps, &st );
    auto tt_xag = simulate<kitty::dynamic_truth_table>( abs_xag, {pis} );
    const auto ntk = circuit_to_logic_network<xag_network, netlist<stg_gate>>( qnet, st.i_indexes, st.o_indexes );
    auto tt_ntk = simulate<kitty::dynamic_truth_table>( *ntk, {pis} );
    if(verbose) write_unicode(qnet);
    return (tt_xag == tt_ntk);
  }
  return false;
}

static bool test_tracer(xag_method const& m, uint32_t const& num_xag, bool verbose = false, pebbling_mapping_strategy_params const& peb_ps = {} )
{

  using namespace caterpillar;
  using namespace caterpillar::test;
  using namespace mockturtle;
  using namespace tweedledum;

  xag_network xag = get_xag(num_xag);
  netlist<stg_gate> qnet;

  logic_network_synthesis_params psl;
  xag_tracer_params ps;
  xag_tracer_stats st;
  ps.verbose = verbose;
  psl.verbose = verbose;

  if(m == xag_method::xag_lowt)
  {
    xag_mapping_strategy strategy;
    logic_network_synthesis( qnet, xag, strategy, {}, psl);
    auto [CNOT, T_count, T_depth] = caterpillar::detail::qc_stats(qnet, false);

    xag_mapping_strategy strategy2;
    ps.low_tdepth_AND = false;
    xag_tracer(xag, strategy2, ps, &st);
    return ( (CNOT == st.CNOT_count) && (T_count == st.T_count) && (T_depth == st.T_depth) );
  }
  else if(m == xag_method::xag_lowt_fast)
  {
    xag_fast_lowt_mapping_strategy strategy;
    logic_network_synthesis( qnet, xag, strategy, {}, psl);
    auto [CNOT, T_count, T_depth] = caterpillar::detail::qc_stats(qnet, false);

    xag_fast_lowt_mapping_strategy strategy2;
    ps.low_tdepth_AND = false;
    xag_tracer(xag, strategy2, ps, &st);
    return ( (CNOT == st.CNOT_count) && (T_count == st.T_count) && (T_depth == st.T_depth) );
  }
  else if(m == xag_method::xag_lowd)
  {
    xag_low_depth_mapping_strategy strategy (true);
    logic_network_synthesis( qnet, xag, strategy, {}, psl);
    auto [CNOT, T_count, T_depth] = caterpillar::detail::qc_stats(qnet, true);


    xag_low_depth_mapping_strategy strategy2 (true);
    ps.low_tdepth_AND = true;
    xag_tracer(xag, strategy2, ps, &st);
    return ( (CNOT == st.CNOT_count) && (T_count == st.T_count) && (T_depth == st.T_depth) );
  }
  else if(m== xag_method::xag_pebb)
  {
    #ifdef USE_Z3
    xag_pebbling_mapping_strategy strategy (peb_ps);
    logic_network_synthesis( qnet, xag, strategy, {}, psl);
    auto [CNOT, T_count, T_depth] = caterpillar::detail::qc_stats(qnet, false);

    xag_pebbling_mapping_strategy strategy2 (peb_ps);
    ps.low_tdepth_AND = false;
    xag_tracer(xag, strategy2, ps, &st);
    return ( (CNOT == st.CNOT_count) && (T_count == st.T_count) && (T_depth == st.T_depth) );
    #endif
    return false;
  }
  else if(m== xag_method::abs_xag_lowt)
  {
    abstract_xag_mapping_strategy strategy;
    const auto abs_xag = cleanup_dangling( cleanup_dangling<xag_network, abstract_xag_network>( xag ) );
    logic_network_synthesis( qnet, abs_xag, strategy, {}, psl );
    auto [CNOT, T_count, T_depth] = caterpillar::detail::qc_stats(qnet, false);

    abstract_xag_mapping_strategy strategy2;
    ps.low_tdepth_AND = false;
    xag_tracer(abs_xag, strategy2, ps, &st);
    return ( (CNOT == st.CNOT_count) && (T_count == st.T_count) && (T_depth == st.T_depth) );
  }
  else if(m== xag_method::abs_xag_lowt_fast)
  {
    abstract_xag_fast_lowt_mapping_strategy strategy;
    const auto abs_xag = cleanup_dangling( cleanup_dangling<xag_network, abstract_xag_network>( xag ) );
    logic_network_synthesis( qnet, abs_xag, strategy, {}, psl );
    auto [CNOT, T_count, T_depth] = caterpillar::detail::qc_stats(qnet, false);

    abstract_xag_fast_lowt_mapping_strategy strategy2;
    ps.low_tdepth_AND = false;
    xag_tracer(abs_xag, strategy2, ps, &st);
    return ( (CNOT == st.CNOT_count) && (T_count == st.T_count) && (T_depth == st.T_depth) );
  }
  else if(m== xag_method::abs_xag_lowd)
  {
    abstract_xag_low_depth_mapping_strategy strategy (false);
    const auto abs_xag = cleanup_dangling( cleanup_dangling<xag_network, abstract_xag_network>( xag ) );
    logic_network_synthesis( qnet, abs_xag, strategy, {}, psl );
    auto [CNOT, T_count, T_depth] = caterpillar::detail::qc_stats(qnet, true);

    abstract_xag_low_depth_mapping_strategy strategy2 (false);
    ps.low_tdepth_AND = true;
    xag_tracer(abs_xag, strategy2, ps, &st);
    return ( (CNOT == st.CNOT_count) && (T_count == st.T_count) && (T_depth == st.T_depth) );
    
  }
  return false;
}

}//caterpillar::test