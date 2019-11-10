#include <tweedledum/networks/netlist.hpp>
#include <tweedledum/gates/mcmt_gate.hpp>
#include <caterpillar/structures/stg_gate.hpp>

namespace caterpillar
{
  using namespace tweedledum;

  /*
    /brief! 
    transforms a reversible network result of the xag strategy in a quantum circuit
    >>>>>>> would need support for classically controlled gates 
  */
  void decompose_with_ands( netlist<mcmt_gate>& qnet, netlist<stg_gate> const& rnet)
  {
    //for each qubit create one in qnet and map the indices
    std::map<uint32_t, uint32_t> q_to_rev_id;

    //check repeting ANDs for uncomputation
    std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> ash;

    rnet.foreach_cqubit([&] ( const uint32_t cq)
    {
      uint32_t q = qnet.add_qubit();
      q_to_rev_id[cq] = q;
    });


    rnet.foreach_cgate([&] (const auto rgate)
    {
      auto cs = rgate.gate.controls();
      auto ts = rgate.gate.targets();

      assert ( rgate.gate.num_controls() <= 2 && rgate.gate.num_targets() == 1 );

      if (rgate.gate.num_controls() == 1)
      {
        //CNOT
        qnet.add_gate(gate::cx, 
          q_to_rev_id[cs[0]], 
          q_to_rev_id[ts[0]]);

      }
      else if (rgate.gate.num_controls() == 2)
      {
        //AND
        auto a = q_to_rev_id[ cs[0] ];
        auto b = q_to_rev_id[ cs[1] ];
        auto c = q_to_rev_id[ ts[0] ];

        if (std::find(ash.begin(), ash.end(), std::make_tuple(a, b, c)) == ash.end())
        {
          qnet.add_gate(gate::hadamard, c);
          qnet.add_gate(gate::t, c);
          qnet.add_gate(gate::cx, a, c);
          qnet.add_gate(gate::cx, b, c);
          qnet.add_gate(gate::cx, c, a);
          qnet.add_gate(gate::cx, c, b);
          qnet.add_gate(gate::t_dagger, a);
          qnet.add_gate(gate::t_dagger, b);
          qnet.add_gate(gate::t, c);
          qnet.add_gate(gate::cx, c, a);
          qnet.add_gate(gate::cx, c, b);
          qnet.add_gate(gate::hadamard, c);
          //s gate
          qnet.add_gate(gate::t, c);
          qnet.add_gate(gate::t, c);

          ash.push_back(std::make_tuple(a, b, c));
        }
        else
        {
          // worst case uncomputing 
          // >>>>>>>>>>>>>> error in functionality <<<<<<<<<<<<<<<<<<<
          //creg out[1];
          qnet.add_gate(gate::hadamard, c);
          qnet.add_gate(gate::cz, a, b);
          qnet.add_gate(gate::pauli_x, c);
        }

      }

    });


  }

}