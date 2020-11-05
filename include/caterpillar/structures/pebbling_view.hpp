/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Giulia Meuli
*------------------------------------------------------------------------------------------------*/

/*! 
  \file pebbling_view.hpp
  \brief Implements weights for networks
*/
#pragma once

#include <mockturtle/views/immutable_view.hpp>
#include <caterpillar/details/utils.hpp>

#include <iostream>
namespace caterpillar
{

/*! 

**Required network functions:**
  - num_gates,
  - num_pis

*/

#pragma region has_get_parents
template<class Ntk, class = void>
struct has_get_parents : std::false_type
{
};

template<class Ntk>
struct has_get_parents<Ntk, std::void_t<decltype( std::declval<Ntk>().get_parents( std::declval<mockturtle::node<Ntk>>() ) )>> : std::true_type
{
};

template<class Ntk>
inline constexpr bool has_get_parents_v = has_get_parents<Ntk>::value;
#pragma endregion

#pragma region has_get_weight
template<class Ntk, class = void>
struct has_get_weight : std::false_type
{
};

template<class Ntk>
struct has_get_weight<Ntk, std::void_t<decltype( std::declval<Ntk>().get_weight( std::declval<mockturtle::node<Ntk>>() ) )>> : std::true_type
{
};

template<class Ntk>
inline constexpr bool has_get_weight_v = has_get_weight<Ntk>::value;
#pragma endregion

template<typename Ntk>
class pebbling_view : public mockturtle::immutable_view<Ntk>
{
public:
  using node = typename Ntk::node;
  pebbling_view( Ntk const& network )
      : mockturtle::immutable_view<Ntk>( network )
  {
    static_assert( mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type" );
    static_assert( mockturtle::has_num_gates_v<Ntk>, "Ntk does not implement the num_gates method" );
    static_assert( mockturtle::has_num_pis_v<Ntk>, "Ntk does not implement the num_pis method" );
    static_assert( mockturtle::has_foreach_node_v<Ntk>, "Ntk does not implement the foreach_node method" );
    static_assert( mockturtle::has_foreach_fanin_v<Ntk>, "Ntk does not implement the foreach_fanin method" );

    _weights.resize(this ->num_gates(), 1);
    compute_parents();

  }

  uint32_t get_weight (const node n) const
  {
    return _weights[this->node_to_index( n ) - detail::resp_num_pis(*this) ];
  }

  void set_weight (const node n, uint32_t w)
  {
    _weights[this->node_to_index( n ) - detail::resp_num_pis(*this)] = w;
  }

  std::vector<node> get_parents( node const& n) const
  {
    return parents[n];
  }

private:

  void compute_parents()
  {
    parents.resize(this -> size());

    this -> foreach_node([&] (auto const& node) {
      this -> foreach_fanin( node, [&] (auto const& s) {
        auto ch = this -> get_node(s);
        parents[ch].push_back(node);
      });
    });
  }
  std::vector<uint32_t> _weights;
  std::vector<std::vector<node>> parents;
};

 } // namespace caterpillar