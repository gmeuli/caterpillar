cmake_minimum_required(VERSION 3.8)
project(caterpillar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(CATERPILLAR_EXAMPLES "Build examples" ON)
option(CATERPILLAR_TEST "Build tests" OFF)
option(CATERPILLAR_Z3 "Use z3" OFF)

if(UNIX)
  # show quite some warnings (but remove some intentionally)
  include(CheckCXXCompilerFlag)
  add_compile_options(-W -Wall -Wextra)
  foreach (WARNING unknown-pragmas gnu-anonymous-struct nested-anon-types)
    check_cxx_compiler_flag("-Wno-${WARNING}" HAS_WNO_${WARNING})
    if (HAS_WNO_${WARNING})
      add_compile_options(-Wno-${WARNING})
    endif()
  endforeach()
endif()

add_subdirectory(include)
add_subdirectory(lib)

if(CATERPILLAR_EXAMPLES)
  add_subdirectory(examples)
endif()

if(CATERPILLAR_TEST)
  add_subdirectory(test)
endif()
