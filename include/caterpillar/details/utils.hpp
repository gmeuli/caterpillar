/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Giulia Meuli
*------------------------------------------------------------------------------------------------*/
#include <tweedledum/gates/gate_set.hpp>
#include <tweedledum/networks/netlist.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/traits.hpp>
#include <caterpillar/structures/abstract_network.hpp>
#include <caterpillar/structures/stg_gate.hpp>

#pragma once

namespace caterpillar::detail
{
  template<class Ntk>
  static inline std::vector<typename Ntk::node> get_outputs(Ntk const& ntk)
  {
    static_assert(mockturtle::has_foreach_po_v<Ntk>, "Ntk does not implement the foreach_po method");

    std::vector<typename Ntk::node> drivers;
    ntk.foreach_po( [&]( auto const& f ) { drivers.push_back( ntk.get_node( f ) ); } );
    return drivers;
  }

  template<class Ntk>
  static uint32_t resp_num_pis(Ntk const& net)
	{
		if constexpr (std::is_base_of_v<mockturtle::klut_network, Ntk> || std::is_same_v<Ntk, abstract_network>)
			return net.num_pis() + 2;
		else 
			return net.num_pis() + 1;
	}

  template<class QuantumCircuit>
  static inline int t_cost( QuantumCircuit const& netlist)
  {
    int count = 0;
    netlist.foreach_cgate([&](const auto gate)
    {
      if (gate.gate.is(tweedledum::gate_set::t))
      {
        count++;
      }
    });

    return count;
  }

  static inline int t_cost( const int tof_controls, const int lines )
  {
    switch ( tof_controls )
    {
    case 0u:
    case 1u:
      return 0;

    case 2u:
      return 7;

    case 3u:
      return 16;
      
    default:
      if ( lines - tof_controls - 1 >= ( tof_controls - 1 ) / 2 )
      {
        return 8 * ( tof_controls - 1 );
      }
      else
      {
        return 16 * ( tof_controls - 1 );
      }
    }
  }

  template<class TofNetwork>
  static inline int count_t_gates ( TofNetwork const& netlist ) 
  {
      auto T_number = 0u;
      netlist.foreach_cgate( [&]( const auto& gate ) {
        T_number += t_cost( gate.gate.num_controls(), netlist.size() );
      } );
      return T_number;
  }

  /* finds T-count, T-depth and #CNOT for {X, CNOT, CCNOT} circuits where all CCNOT are computed on a clean helper line */
  static inline std::tuple<uint32_t, uint32_t, uint32_t> qc_stats(tweedledum::netlist<caterpillar::stg_gate> const& ntk, bool use_tdepth1 = false)
  {
    auto Tcount = 0u;
    auto CNOT = 0u;

    std::vector<uint32_t> depths (ntk.num_qubits());
    std::vector<bool> mask (ntk.num_qubits());


    ntk.foreach_cgate([&] (auto& gate)
    {
      assert(gate.gate.num_controls() <= 2);
      auto t = gate.gate.targets()[0];

      if (gate.gate.num_controls() == 1)
      { 
        CNOT++;

        auto c = gate.gate.controls()[0];
        depths[t] = std::max(depths[c], depths[t]);
      }
      else if(gate.gate.num_controls() == 2)
      {
        auto c1 = gate.gate.controls()[0];
        auto c2 = gate.gate.controls()[1];
        if (!mask[t])
        {

          depths[t] = use_tdepth1 ? std::max(std::max(depths[t] + 1, depths[c1]+1), depths[c2]+1)  : std::max(std::max(depths[t] + 2, depths[c1]+1), depths[c2]+1);

          gate.gate.foreach_control([&] (auto& c)
          {
            depths[c] = depths[c] + 1;
          });
          
          Tcount = Tcount + 4;
        }
        mask[t] = !(mask[t]);
      }
      
    });


    auto Tdepth = depths[std::max_element(depths.begin(), depths.end()) - depths.begin()];
    return { CNOT, Tcount, Tdepth}; 
  }
  
} // namespace caterpillar::detail