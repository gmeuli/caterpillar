Mapping Strategies
===================

A mapping strategy provides a description of how the synthesizer should transpile a network. 

All the mapping strategies have a method `foreach_step` that iterates over all the steps required to translate the network.
A step is defined by a network node and by a mapping action: `compute_action`, `uncompute_action`, `compute_inplace_action` and `uncompute_inplace_action`.

All the strategies work on AIGs, XAGs, XMGs, MIGs and LUT networks, unless specifically noted otherwise.

Mapping strategies are solution to the reversible pebbling game problem.

.. doxygenclass:: caterpillar::mapping_strategy
  :members: compute_steps, foreach_step

Bennett strategy
----------------
**Header:** ``caterpillar/strategies/bennett_mapping_strategy.hpp``

.. doxygenclass:: caterpillar::bennett_mapping_strategy

Eager strategy
--------------
**Header:** ``caterpillar/strategies/eager_mapping_strategy.hpp``

.. doxygenclass:: caterpillar::eager_mapping_strategy

Best-fit strategy
-----------------
**Header:** ``caterpillar/strategies/best_fit_mapping_strategy.hpp``

.. doxygenclass:: caterpillar::best_fit_mapping_strategy

Pebbling strategy
-----------------
**Header:** ``caterpillar/strategies/pebbling_mapping_strategy.hpp``

.. doxygenclass:: caterpillar::pebbling_mapping_strategy

Parameters
^^^^^^^^^^

.. doxygenstruct:: caterpillar::pebbling_mapping_strategy_params
  :members:

XAG strategy
------------
**Header:** ``caterpillar/strategies/xag_mapping_strategy.hpp``

.. doxygenclass:: caterpillar::xag_mapping_strategy


