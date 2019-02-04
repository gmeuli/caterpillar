[![Build Status](https://travis-ci.com/gmeuli/caterpillar.svg?token=DiDrPCoZ4z6fZ8AyMtUC&branch=master)](https://travis-ci.com/gmeuli/caterpillar)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

<img src="https://user-images.githubusercontent.com/37411238/51745329-be3d7400-20a2-11e9-9ac5-d5e15602ec7b.png" width="64" height="64" align="left" style="margin-right: 12pt" />

# caterpillar
caterpillar is a C++-17 quantum circuit synthesis library. It provides several LUT-based methods for quantum compilation.

## Example
The following code reads an AIG from an Aiger file and synthesizes the corresponding reversible circuit using a logic network based synthesis method. 

```c++

mockturtle::aig_network aig;

lorina::read_aiger( "adder.aig", mockturtle::aiger_reader( aig ) );

mockturtle::mapping_view<mockturtle::aig_network, true> mapped_aig{ aig };

mockturtle::lut_mapping<mockturtle::mapping_view<mockturtle::aig_network, true>, true>(mapped_aig, ps);

auto lutn = mockturtle::collapse_mapped_network<mockturtle::klut_network>( mapped_aig );

tweedledum::gg_network<tweedledum::mcst_gate> rev_net;
if(lutn) caterpillar::logic_network_synthesis( rev_net, *lutn, tweedledum::stg_from_pkrm());
    
``` 

## Installation requirements

A modern compiler is required to build *caterpillar*.  We are continously
testing with Clang 6.0.1, GCC 7.3.0, and GCC 8.2.0.

## EPFL logic sythesis libraries

caterpillar is part of the [EPFL logic synthesis](https://lsi.epfl.ch/page-138455-en.html) libraries.  The other libraries and several examples on how to use and integrate the libraries can be found in the [logic synthesis tool showcase](https://github.com/lsils/lstools-showcase).

