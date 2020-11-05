
#include <catch.hpp>
#include <../test_xag.hpp>

using namespace caterpillar;
using namespace caterpillar::test;
using namespace mockturtle;
using namespace tweedledum;


TEST_CASE("trace simple xag", "[tracetest-1]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 1, false));
  CHECK(test_tracer(xag_method::xag_lowt_fast, 1, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 1, false) );

  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 1, false) );
  #endif
}

TEST_CASE("trace simple xag 2", "[tracetest-2]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 2, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 2, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 2, false) );

  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 2, false) );
  #endif
}

TEST_CASE("trace simple xag 3", "[tracetest-3]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 3, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 3, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 3, false) );

  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 3, false) );
  #endif
}

TEST_CASE("trace simple xag 4", "[tracetest-4]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 4, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 4, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 4, false) );

  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 4, false) );
  #endif
}

TEST_CASE("trace simple xag 5", "[tracetest-5]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 5, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 5, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 5, false) );

  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 5, false) );
  #endif
}

TEST_CASE("trace simple xag 6", "[tracetest-6]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 6, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 6, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 6, false) );

  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 6, false) );
  #endif
}

TEST_CASE("trace simple xag with codependent xor outputs", "[tracetest-7]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 7, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 7, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 7, false) );

  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 7, false) );
  #endif
}

TEST_CASE("trace simple xag with reconvergence", "[tracetest-8]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 8, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 8, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 8, false) );

  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 8, false) );
  #endif
}

TEST_CASE("trace simple xag 9", "[tracetest-9]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 9, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 9, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 9, false) );
 
  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 9, false) );
  #endif
}

TEST_CASE("trace simple xag 10", "[tracetest-10]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 10, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 10, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 10, false) );
 
  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 10, false) );
  #endif
}

TEST_CASE("trace simple xag 11", "[tracetest-11]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 11, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 11, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 11, false) );

  #ifdef USE_Z3
  pebbling_mapping_strategy_params peb_ps;
  peb_ps.pebble_limit=2;
  CHECK(test_tracer(xag_method::xag_pebb, 11, false) );
  #endif
}

TEST_CASE("trace simple xag 12", "[tracetest-12]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 12, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 12, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 12, false) );

  #ifdef USE_Z3
  pebbling_mapping_strategy_params peb_ps;
  peb_ps.pebble_limit=4;
  CHECK(test_tracer(xag_method::xag_pebb, 12, false) );
  #endif
}

TEST_CASE("trace simple xag 13", "[tracetest-13]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 13, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 13, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 13, false) );

  #ifdef USE_Z3
  pebbling_mapping_strategy_params peb_ps;
  peb_ps.pebble_limit=28;
  CHECK(test_tracer(xag_method::xag_pebb, 13, false, peb_ps) );
  #endif
}

TEST_CASE("trace XAG and opt weights", "[tracetest-14]")
{
  
  CHECK(test_tracer(xag_method::xag_lowt, 14, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 14, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 14, false) );

  #ifdef USE_Z3
  pebbling_mapping_strategy_params peb_ps;
  peb_ps.pebble_limit=4;
  peb_ps.conflict_limit = 1000000;
  peb_ps.optimize_weight = true;
  peb_ps.verbose = false;
  CHECK(test_tracer(xag_method::xag_pebb, 14, false, peb_ps) );
  #endif
}

TEST_CASE("trace XAG 15", "[tracetest-15]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 15, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 15, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 15, false) );

  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 15, false) );
  #endif
}

TEST_CASE("trace XAG 16", "[tracetest-16]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 16, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 16, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 16, false) );

  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 16, false) );
  #endif
}

TEST_CASE("trace XAG 17", "[tracetest-17]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 17, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 17, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 17, false) );

  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 17, false) );
  #endif
}

TEST_CASE("trace min depth synthesis XAG-small", "[tracetest-18]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 18, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 18, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 18, false) );

  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 18, false) );
  #endif
}

TEST_CASE("trace min depth synthesis XAG-small ", "[tracetest-19]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 19, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 19, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 19, false) );

  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 19, false) );
  #endif
}


TEST_CASE("trace min depth synthesis parity buffer", "[tracetest-20]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 20, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 20, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 20, false) );

  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 20, false) );
  #endif
}
TEST_CASE("trace min depth included cone", "[tracetest-21]")
{
  CHECK(test_tracer(xag_method::xag_lowt, 21, false) );
  CHECK(test_tracer(xag_method::xag_lowt_fast, 21, false) );
  CHECK(test_tracer(xag_method::xag_lowd, 21, false) );

  #ifdef USE_Z3
  CHECK(test_tracer(xag_method::xag_pebb, 21, false));
  #endif
}
