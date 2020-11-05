
#include <catch.hpp>
#include "../test_xag.hpp"

using namespace caterpillar;
using namespace caterpillar::test;

TEST_CASE("synthesize simple abs xag", "[AXAG synthesis]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 1, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 1, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 1, false) );  
}


TEST_CASE("synthesize simple abs xag 2", "[AXAG synthesis-2]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 2, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 2, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 2, false) );
}

TEST_CASE("synthesize simple abs xag 3", "[AXAG synthesis-3]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 3, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 3, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 3, false) );
}

TEST_CASE("synthesize simple abs xag 4", "[AXAG synthesis-4]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 4, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 4, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 4, false) );
}

TEST_CASE("synthesize simple abs xag 5", "[AXAG synthesis-5]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 5, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 5, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 5, false) );  
}

TEST_CASE("synthesize simple abs xag 6", "[AXAG synthesis-6]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 6, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 6, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 6, false) );
}

TEST_CASE("synthesize simple abs xag with codependent xor outputs", "[AXAG synthesis-7]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 7, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 7, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 7, false) );
}

TEST_CASE("synthesize simple abs xag with reconvergence", "[AXAG synthesis-8]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 8, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 8, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 8, false) );
}

TEST_CASE("synthesize simple abs xag 9", "[AXAG synthesis-9]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 9, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 9, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 9, false) );
}

TEST_CASE("synthesize simple abs xag 10", "[AXAG synthesis-10]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 10, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 10, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 10, false) );
}

TEST_CASE("synthesize simple abs xag using pebbling", "[AXAG synthesis-11]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 11, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 11, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 11, false) );
}

TEST_CASE("pebble simple abs xag 10", "[AXAG synthesis-12]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 12, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 12, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 12, false) );
}

TEST_CASE("pebble simple abs xag 11", "[AXAG synthesis-13]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 13, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 13, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 13, false) );
}

TEST_CASE("pebbling abs xag with weights", "[AXAG synthesis-14]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 14, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 14, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 14, false) );
}

TEST_CASE("min depth synthesis abs XAG", "[AXAG synthesis-15]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 15, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 15, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 15, false) );
}

TEST_CASE("min depth synthesis abs XAG-2", "[AXAG synthesis-16]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 16, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 16, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 16, false) );
}

TEST_CASE("min depth synthesis abs xag no copies", "[AXAG synthesis-17]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 17, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 17, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 17, false) );
}

TEST_CASE("min depth synthesis abs XAG-small", "[AXAG synthesis-18]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 18, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 18, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 18, false) );
}

TEST_CASE("min depth synthesis abs XAG-small ", "[AXAG synthesis-19]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 19, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 19, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 19, false) );
}

TEST_CASE("abs xag with parity buffer", "[AXAG synthesis-20]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 20, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 20, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 20, false) );
}

TEST_CASE("abs xag with included cone", "[AXAG synthesis-21]")
{
  CHECK(xag_synthesis(xag_method::abs_xag_lowt, 21, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowt_fast, 21, false) );
  CHECK(xag_synthesis(xag_method::abs_xag_lowd, 21, false) );
}
