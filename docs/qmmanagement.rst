Quantum Memory Management
=========================

Most manual and automatic approaches for quantum circuit synthesis decompose large functionality into smaller parts in order to deal with complexity. 
Each part requires some resources in terms of qubits and quantum operations. 
The components can be connected together in order to obtain the desired computation for the overall circuit.

There are many possible ways to combine the small parts of a decomposition, each of which resulting in different accumulated costs for number of qubits and number of quantum operations. 
The requirement that all intermediate results must be uncomputed makes finding a good way to combine parts particularly difficult in quantum computing. 
Consequently, effective memory management, which guarantees erasure of intermediate results, is crucial in quantum circuit synthesis.

The problem of finding a strategy to compute and uncompute intermediate states for a given fixed number of qubits corresponds to solving the reversible pebbling game. 
The reversible pebbling game is played on an input *Directed Acyclic Graph* (DAG), in which each node corresponds to one part of the decomposed computation, and edges define data dependency. 
The game consists of placing pebbles on the graph nodes. 
When a node is *pebbled* the computed value is available on an ancillae qubit.  
Initially no node is pebbled. A pebble can be placed on a node if all its children are pebbled, and the same condition is required to remove a pebble from a node. 
The game is concluded if all the outputs are pebbled and all the other nodes are unpebbled. 
Solving the problem returns a valid clean-up strategy.

.. image:: anim.gif

The animated pictures shows one possible *eager* strategy to generate a reversible circuit (on the left) from an example DAG (on the right). 
The depicted strategy first computes the first output, then uncomputes all the nodes that are no more needed, then computes the second output and finally uncomputes the last remaining node. 


Please refer to the paper :cite:`MS19` for further details on this subject.

