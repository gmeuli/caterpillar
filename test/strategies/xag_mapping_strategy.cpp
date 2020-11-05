
#include <catch.hpp>
#include "../test_xag.hpp"

using namespace caterpillar;
using namespace caterpillar::test;

TEST_CASE("synthesize simple xag", "[XAG synthesis]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 1, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 1, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 1, false) );

  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 1, false) );
  #endif
}

TEST_CASE("synthesize simple xag 2", "[XAG synthesis-2]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 2, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 2, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 2, false) );

  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 2, false) );
  #endif
}

TEST_CASE("synthesize simple xag 3", "[XAG synthesis-3]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 3, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 3, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 3, false) );

  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 3, false) );
  #endif
}

TEST_CASE("synthesize simple xag 4", "[XAG synthesis-4]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 4, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 4, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 4, false) );

  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 4, false) );
  #endif  
}

TEST_CASE("synthesize simple xag 5", "[XAG synthesis-5]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 5, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 5, false) );

  CHECK(xag_synthesis(xag_method::xag_lowd, 5, false) );

  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 5, false) );
  #endif

}

TEST_CASE("synthesize simple xag 6", "[XAG synthesis-6]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 6, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 6, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 6, false) );

  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 6, false) );
  #endif
}

TEST_CASE("synthesize simple xag with codependent xor outputs", "[XAG synthesis-7]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 7, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 7, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 7, false) );

  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 7, false) );
  #endif
}

TEST_CASE("synthesize simple xag with reconvergence", "[XAG synthesis-8]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 8, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 8, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 8, false) );

  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 8, false) );
  #endif
}

TEST_CASE("synthesize simple xag 9", "[XAG synthesis-9]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 9, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 9, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 9, false) );

  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 9, false) );
  #endif
}

TEST_CASE("synthesize simple xag 10", "[XAG synthesis-10]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 10, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 10, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 10, false) );
  
  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 10, false) );
  #endif
}

TEST_CASE("synthesize simple xag using pebbling", "[XAG synthesis-11]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 11, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 11, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 11, false) );
  
  #ifdef USE_Z3
  pebbling_mapping_strategy_params peb_ps;
  peb_ps.pebble_limit=2;
  CHECK(xag_synthesis(xag_method::xag_pebb, 11, false) );
  #endif
}

TEST_CASE("pebble simple xag 10", "[XAG synthesis-12]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 12, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 12, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 12, false) );
  
  #ifdef USE_Z3
  pebbling_mapping_strategy_params peb_ps;
  peb_ps.pebble_limit=4;
  CHECK(xag_synthesis(xag_method::xag_pebb, 12, false) );
  #endif
}

TEST_CASE("pebble simple xag 11", "[XAG synthesis-13]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 13, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 13, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 13, false) );
  
  #ifdef USE_Z3
  pebbling_mapping_strategy_params peb_ps;
  peb_ps.pebble_limit=28;
  CHECK(xag_synthesis(xag_method::xag_pebb, 13, false, peb_ps) );
  #endif
}

TEST_CASE("pebbling XAG with weights", "[XAG synthesis-14]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 14, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 14, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 14, false) );
  
  #ifdef USE_Z3
  pebbling_mapping_strategy_params peb_ps;
  peb_ps.pebble_limit=4;
  peb_ps.conflict_limit = 1000000;
  peb_ps.optimize_weight = true;
  peb_ps.verbose = false;
  CHECK(xag_synthesis(xag_method::xag_pebb, 14, false, peb_ps) );
  #endif
}

TEST_CASE("min depth synthesis XAG", "[XAG synthesis-15]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 15, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 15, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 15, false) );
  
  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 15, false) );
  #endif
}

TEST_CASE("min depth synthesis XAG-2", "[XAG synthesis-16]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 16, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 16, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 16, false) );
  
  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 16, false) );
  #endif
}

TEST_CASE("min depth synthesis XAG no copies", "[XAG synthesis-17]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 17, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 17, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 17, false) );
  
  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 17, false) );
  #endif
}

TEST_CASE("min depth synthesis XAG-small", "[XAG synthesis-18]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 18, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 18, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 18, false) );
 
  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 18, false) );
  #endif
}

TEST_CASE("min depth synthesis XAG-small ", "[XAG synthesis-19]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 19, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 19, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 19, false) );
  
  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 19, false) );
  #endif
}


TEST_CASE("min depth synthesis parity buffer", "[XAG synthesis-20]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 20, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 20, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 20, false) );
  
  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 20, false) );
  #endif
}
TEST_CASE("min depth included cone", "[XAG synthesis-21]")
{
  CHECK(xag_synthesis(xag_method::xag_lowt, 21, false) );
  CHECK(xag_synthesis(xag_method::xag_lowt_fast, 21, false) );
  CHECK(xag_synthesis(xag_method::xag_lowd, 21, false) );
  
  #ifdef USE_Z3
  CHECK(xag_synthesis(xag_method::xag_pebb, 21, false) );
  #endif
}