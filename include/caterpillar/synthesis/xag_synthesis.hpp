#include <mockturtle/networks/xag.hpp>
#include <tweedledum/networks/netlist.hpp>
#include <caterpillar/stg_gate.hpp>
#include <mockturtle/views/topo_view.hpp>



namespace caterpillar
{

struct xag_heuristic_synthesis_stat
{
  uint32_t required_ancillae = 0;
};
struct xag_heuristic_synthesis_param
{
};

namespace detail
{
class xag_synthesizer
{
public:
  xag_synthesizer( mockturtle::xag_network const& xag, tweedledum::netlist<stg_gate> qnet, xag_heuristic_synthesis_param const& ps, xag_heuristic_synthesis_stat st )
      :xag(xag), qnet(qnet), ps(ps), st(st)
      {}
  
  void prepare_pi()
  {
    /* prepare primary inputs of logic network */
    xag.foreach_pi( [&]( auto n ) {
      node_to_qubit[n] = qnet.num_qubits();
      qnet.add_qubit();
    } );

  }

  void accumulate_leaves(std::vector<uint32_t>& leaves, mockturtle::signal<mockturtle::xag_network> const& s)
  {
    xag.foreach_fanin( xag.get_node( s ), [&]( auto si ) {
      auto node = xag.get_node( si );
      if (!xag.is_xor(node))
      {
        leaves.push_back( node );
      }
      else 
        accumulate_leaves( leaves, si );
    } );
  }

  std::vector<uint32_t> get_cone_leaves( mockturtle::signal<mockturtle::xag_network> s)
  {
    std::vector<uint32_t> leaves;
    accumulate_leaves( leaves, s );

    return leaves;
  }

  void order_sets(std::vector<std::pair<std::vector<uint32_t>, uint32_t>>& leaf_sets)
  {
    std::sort(leaf_sets[0].first.begin(), leaf_sets[0].first.end());
    std::sort(leaf_sets[1].first.begin(), leaf_sets[1].first.end());

    leaf_sets[0].first.erase(std::unique(leaf_sets[0].first.begin(), leaf_sets[0].first.end()), leaf_sets[0].first.end());
    leaf_sets[1].first.erase(std::unique(leaf_sets[1].first.begin(), leaf_sets[1].first.end()), leaf_sets[1].first.end());

    std::vector<uint32_t> diff_0, diff_1;
    std::set_difference( leaf_sets[0].first.begin(), leaf_sets[0].first.end(), leaf_sets[1].first.begin(), leaf_sets[1].first.end(), std::back_inserter( diff_0 ) );
    std::set_difference( leaf_sets[1].first.begin(), leaf_sets[1].first.end(), leaf_sets[0].first.begin(), leaf_sets[0].first.end(), std::back_inserter( diff_1 ) );

    for(auto elem : leaf_sets[0].first)
    {
      if (std::find(diff_0.begin(), diff_0.end(), elem) != diff_0.end())
      {
        auto it_move = std::find( leaf_sets[0].first.begin(), leaf_sets[0].first.end(), elem );
        std::move( it_move, it_move + 1, leaf_sets[0].first.begin() );
        break;
      }
    }
    for(auto elem : leaf_sets[1].first)
    {
      if (std::find(diff_1.begin(), diff_1.end(), elem) != diff_1.end())
      {
        auto it_move = std::find( leaf_sets[1].first.begin(), leaf_sets[1].first.end(), elem );
        std::move( it_move, it_move + 1, leaf_sets[1].first.begin() );
        break;
      }
    }

  }

  void compute_toffoli( std::vector<std::pair<std::vector<uint32_t>, uint32_t>> const& leaf_sets )
  {
    for ( auto pair : leaf_sets )
    {
      for( auto leaf : pair.first)
      {
        auto dest = pair.first[0];
        if ( leaf != dest )
        {
          qnet.add_gate( stg_gate( {{node_to_qubit[leaf]}}, node_to_qubit[dest] ) );
        }
      }
    }
  }

  uint32_t request_ancillae()
  {
      const auto r = qnet.num_qubits();
      st.required_ancillae++;
      qnet.add_qubit();
      return r;
  }

  bool run()
  {
    prepare_pi();

    auto xagw = mockturtle::topo_view( xag );
    xagw.foreach_node( [&]( auto node ) {

      if (xagw.is_and(node))
      {
        std::vector<std::pair<std::vector<uint32_t>, uint32_t>> leaf_sets;
        xagw.foreach_fanin( node, [&]( auto sn, auto i ) {
          leaf_sets.push_back( std::make_pair( get_cone_leaves( sn ), xag.get_node( sn ) ) );
        } );

        order_sets(leaf_sets);

        compute_toffoli(leaf_sets);

        auto anc = request_ancillae();
        qnet.add_gate( stg_gate( {{node_to_qubit[leaf_sets[0].first[0]], node_to_qubit[leaf_sets[1].first[0]]}}, anc ) );
        node_to_qubit[node] = anc;

        compute_toffoli(leaf_sets);

      }
    } );

  }


private: 
  mockturtle::xag_network xag;
  tweedledum::netlist<stg_gate> qnet;

  xag_heuristic_synthesis_param ps;
  xag_heuristic_synthesis_stat st;

  std::unordered_map<mockturtle::node<mockturtle::xag_network>, uint32_t> node_to_qubit;
};
}
void xag_heuristic_synthesis( mockturtle::xag_network const& xag, tweedledum::netlist<stg_gate> qnet, xag_heuristic_synthesis_param const& ps = {}, xag_heuristic_synthesis_stat* pst = nullptr )
{
  xag_heuristic_synthesis_stat st;
  auto xs = detail::xag_synthesizer( xag, qnet, ps, st );

  xs.run();

  if ( pst )
  {
    *pst = st;
  }
}
}

