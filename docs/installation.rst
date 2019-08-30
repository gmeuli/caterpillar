Installation
============

Caterpillar is a header-only C++-17 library.  Just add the include directory
of caterpillar to your include directories, and you can integrate caterpillar into
your source files using

.. code-block:: c++

   #include <caterpillar/caterpillar.hpp>

Requirements
------------

We tested building caterpillar on Mac OS and Linux using Clang 6.0.1, GCC 7.3.0,
and GCC 8.2.0. 

If you experience that the system compiler does not suffice the requirements,
you can manually pass a compiler to CMake using::

  cmake -DCMAKE_CXX_COMPILER=/path/to/c++-compiler ..

Building tests
--------------

In order to run the tests, you need to init the submodules and enable tests
in CMake::

  mkdir build
  cd build
  cmake -DCATERPILLAR_TEST=ON ..
  make
  ./test/run_tests

To enable caterpillar's features that make use of the open source SAT solver `Z3 <https://github.com/Z3Prover/z3>`_ use::

  cmake -DCATERPILLAR_Z3=ON -DZ3_INCLUDE_DIR=/path/to/z3headers -DZ3_LINK_DIR=/path/to/z3libraries ..

