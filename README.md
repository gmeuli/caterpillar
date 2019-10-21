[![Build Status](https://travis-ci.com/gmeuli/caterpillar.svg?token=DiDrPCoZ4z6fZ8AyMtUC&branch=master)](https://travis-ci.com/gmeuli/caterpillar)
[![Documentation Status](https://readthedocs.org/projects/qcaterpillar/badge/?version=latest)](https://qcaterpillar.readthedocs.io/en/latest/?badge=latest)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

<img src="https://raw.githubusercontent.com/gmeuli/caterpillar/master/docs/logoII.svg?sanitize=true" width="64" height="64" align="left" style="margin-right: 12pt" />

# caterpillar
caterpillar is a C++-17 quantum circuit synthesis library. It provides several LUT-based methods for quantum compilation.

[Read the full documentation.](https://qcaterpillar.readthedocs.io/en/latest/?badge=latest)

## Example
The following code reads an XAG network from a verilog file and uses logic network based synthesis to obtain the corresponding reversible circuit. In this example, we use the pebbling mapping strategy, that is a SAT-based method enabling constraints on the number of qubits. 

```c++
  /* read verilog file using lorina */
  mockturtle::xag_network xag;

  auto const result = lorina::read_verilog( "ex.v", mockturtle::verilog_reader( xag ) );
  if ( result != lorina::return_code::success )
    return 0;  

  /* select the pebbling compilation strategy */
  caterpillar::pebbling_mapping_strategy_params ps;
  ps.pebble_limit = 100;

  caterpillar::pebbling_mapping_strategy<mockturtle::xag_network> strategy( ps );

  /* compile to quantum circuit */
  tweedledum::netlist<caterpillar::stg_gate> circ;

  caterpillar::logic_network_synthesis( circ, xag, strategy);
``` 

## Installation requirements

A modern compiler is required to build *caterpillar*.  We are continously
testing with Clang 6.0.1, GCC 7.3.0, and GCC 8.2.0.

## EPFL logic sythesis libraries

caterpillar is part of the [EPFL logic synthesis](https://lsi.epfl.ch/page-138455-en.html) libraries.  The other libraries and several examples on how to use and integrate the libraries can be found in the [logic synthesis tool showcase](https://github.com/lsils/lstools-showcase).

