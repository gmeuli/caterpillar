Philosophy
==========

Caterpillar is a library dedicated to the compilation of quantum circuits. 
In particular it translates a logic network into a quantum circuit. 

Caterpillar interfaces with the logic networks defined in the library `mockturtle <https://github.com/msoeken/mockturtle>`_ and uses `tweedledum <https://github.com/boschmitt/tweedledum>`_ to describe quantum circuits.
Both libraries are, together with Caterpillar, part of the `LSI Logic Synthesis Libraries <https://github.com/lsils/lstools-showcase>`_. 

The core algorithm of the library is a hierarchical method called **logic network synthesis**. 
It takes a logic network as input and returns a reversible network, saving all the intermediate results on helper qubits (*ancillae*).
This synthesis task requires all intermediate results to be uncomputed. 

Caterpillar supports several different uncomputing methods, called **strategies**. 
In some cases the problem of finding a good strategy is encoded as a SAT problem and solved with state-of-the-art SAT solvers. 




