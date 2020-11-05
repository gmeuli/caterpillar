![Linux CI](https://github.com/gmeuli/caterpillar/workflows/Linux%20CI/badge.svg)
![MacOS CI](https://github.com/gmeuli/caterpillar/workflows/MacOS%20CI/badge.svg)
[![Documentation Status](https://readthedocs.org/projects/qcaterpillar/badge/?version=latest)](https://qcaterpillar.readthedocs.io/en/latest/?badge=latest)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

<img src="https://raw.githubusercontent.com/gmeuli/caterpillar/master/docs/logoII.svg?sanitize=true" width="64" height="64" align="left" style="margin-right: 12pt" />

# caterpillar
Caterpillar is a C++-17 library dedicated to the synthesis of large quantum circuits implementing Boolean functions, with a focus on quantum memory management.
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


## EPFL logic sythesis libraries

caterpillar is part of the [EPFL logic synthesis](https://lsi.epfl.ch/page-138455-en.html) libraries.  The other libraries and several examples on how to use and integrate the libraries can be found in the [logic synthesis tool showcase](https://github.com/lsils/lstools-showcase).

