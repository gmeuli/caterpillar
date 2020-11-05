/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for details.
| Author(s): Giulia Meuli
*------------------------------------------------------------------------------------------------*/

#pragma once

#include <mockturtle/traits.hpp>
#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/networks/storage.hpp>

#include <ez/direct_iterator.hpp>


#include <memory>

namespace caterpillar
{

struct abstract_storage_data
{
  uint32_t num_pis = 0u;
  uint32_t num_pos = 0u;
};
/*! 
 *
 * `data[0].h1`: Fan-out size
 * 
 * `data[0].h2`: Application-specific value (weight)
 * 
 * `data[1].h1`: Function literal in truth table cache
 * 
 * `data[2].h2`: Visited flags
 */
struct abstract_storage_node : mockturtle::mixed_fanin_node<2>
{
  bool operator==( abstract_storage_node const& other ) const
  {
    return children == other.children && data[0].h2 == other.data[0].h2;
  }
};

using abstract_storage = mockturtle::storage<abstract_storage_node, abstract_storage_data>;

/*! 
  General network in which each node is characterized by its input signals and an application-specific value.
*/
class abstract_network
{
public:
#pragma region Types and constructors

  static constexpr auto min_fanin_size = 1;
  /*! maximum fanin size
  */
  static constexpr auto max_fanin_size = 32;

  using base_type = abstract_network;
  using storage = std::shared_ptr<abstract_storage>;
  using node = uint64_t;
  using signal = uint64_t;

  abstract_network()
      : _storage( std::make_shared<abstract_storage>() )
  {
    _init();
  }

private:
  inline void _init()
  {
    /* reserve the second node for constant 1 */
    _storage->nodes.emplace_back();


    /* weights  for constants */
    _storage->nodes[0].data[0].h2 = 0;
    _storage->nodes[1].data[0].h2 = 0; 
  }
#pragma endregion

#pragma region Primary I / O and constants
public:
  /*! returns a constant signal */
  signal get_constant( bool value = false ) const
  {
    return value ? 1 : 0;
  }

  /*! adds a primary input and returns it */
  signal create_pi( std::string const& name = std::string() )
  {
    (void)name;

    const auto index = _storage->nodes.size();
    _storage->nodes.emplace_back();
    _storage->inputs.emplace_back( index );
		/* zero weights for pi */
    _storage->nodes[index].data[0].h2 = 0;
    return index;
  }

  /*! adds a primary output */
  void create_po( signal const& f, std::string const& name = std::string() )
  {
    (void)name;

    /* increase ref-count to children */
    _storage->nodes[f].data[0].h1++;

    _storage->outputs.emplace_back( f );
  }

  bool is_constant( node const& n ) const
  {
    return n <= 1;
  }

  /*! verifies if a node is a primary input */
  bool is_pi( node const& n ) const
  {
    return n > 1 && _storage->nodes[n].children.empty();
  }

  bool constant_value( node const& n ) const
  {
    return n == 1;
  }
#pragma endregion

#pragma region Create unary functions

  signal create_buf( signal const& a )
  {
    return a;
  }

  /*! creates a NOT node and returns its outcoming signal */
  signal create_not( signal const& a )
  {
    return create_node( {a}, 1 );
  }
#pragma endregion

#pragma region Create binary functions
  /*! creates an AND node and returns its outcoming signal */
  signal create_and( signal a, signal b )
  {
    return create_node( {a, b}, 2 );
  }
#pragma endregion

#pragma region Create arbitrary functions
  /*! creates an abstract node and returns its outcoming signal */
  signal create_node( std::vector<signal> const& children, uint32_t weight = 1 )
  {
    storage::element_type::node_type node;
    std::copy( children.begin(), children.end(), std::back_inserter( node.children ) );

    const auto index = _storage->nodes.size();
    _storage->nodes.push_back( node );

    /* increase ref-count to children */
    for ( auto c : children )
    {
      _storage->nodes[c].data[0].h1++;
    }

    set_weight( index, weight );

    return index;
  }


#pragma endregion  

#pragma region Structural properties
  /*! returns the number of nodes in the network */
  auto size() const
  {
    return static_cast<uint32_t>( _storage->nodes.size() );
  }
  /*! returns the number of primary inputs */
  auto num_pis() const
  {
    return static_cast<uint32_t>(_storage->inputs.size() );
  }
  /*! returns the number of primary outputs */
  auto num_pos() const
  {
    return static_cast<uint32_t>(_storage->outputs.size() );
  }
  /*! returns the number of actual gates (not considering primary inputs and constant nodes) */
  auto num_gates() const
  {
    return static_cast<uint32_t>(_storage->nodes.size() - _storage->inputs.size() - 2 );
  }
  /*! returns the number of incoming signals of a node */
  uint32_t fanin_size( node const& n ) const
  {
    return static_cast<uint32_t>( _storage->nodes[n].children.size() );
  }
  /*! returns the number of outcoming signals of a node */
  uint32_t fanout_size( node const& n ) const
  {
    return _storage->nodes[n].data[0].h1;
  }

#pragma endregion


#pragma region Nodes and signals
  node get_node( signal const& f ) const
  {
    return f;
  }

  signal make_signal( node const& n ) const
  {
    return n;
  }

  uint32_t node_to_index( node const& n ) const
  {
    return static_cast<uint32_t>( n );
  }

  node index_to_node( uint32_t index ) const
  {
    return index;
  }
#pragma endregion

#pragma region Node and signal iterators

  /*! executes a function for every node of the nework */
  template<typename Fn>
  void foreach_node( Fn&& fn ) const
  {
    mockturtle::detail::foreach_element( ez::make_direct_iterator<uint64_t>( 0 ),
                             ez::make_direct_iterator<uint64_t>( _storage->nodes.size() ),
                             fn );
  }
  /*! executes a function for every primary input of the nework */
  template<typename Fn>
  void foreach_pi( Fn&& fn ) const
  {
    mockturtle::detail::foreach_element( _storage->inputs.begin(), _storage->inputs.end(), fn );
  }
  /*! executes a function for every primary output of the nework */
  template<typename Fn>
  void foreach_po( Fn&& fn ) const
  {
    using IteratorType = decltype( _storage->outputs.begin() );
    mockturtle::detail::foreach_element_transform<IteratorType, uint32_t>( _storage->outputs.begin(), _storage->outputs.end(), []( auto o ) { return o.index; }, fn );
  }
  /*! executes a function for every gate of the nework */
  template<typename Fn>
  void foreach_gate( Fn&& fn ) const
  {
    mockturtle::detail::foreach_element_if( ez::make_direct_iterator<uint64_t>( 2 ), /* start from 2 to avoid constants */
                                ez::make_direct_iterator<uint64_t>( _storage->nodes.size() ),
                                [this]( auto n ) { return !is_pi( n ); },
                                fn );
  }
  /*! executes a function for every incoming signal of a node */
  template<typename Fn>
  void foreach_fanin( node const& n, Fn&& fn ) const
  {
    if ( n == 0 || is_pi( n ) )
      return;

    using IteratorType = decltype( _storage->outputs.begin() );
    mockturtle::detail::foreach_element_transform<IteratorType, uint32_t>( _storage->nodes[n].children.begin(), _storage->nodes[n].children.end(), []( auto f ) { return f.index; }, fn );
  }
#pragma endregion

#pragma region Custom node values
  /*! clears the application specific value for every node */
  void clear_weights() const
  {
    std::for_each( _storage->nodes.begin(), _storage->nodes.end(), []( auto& n ) { n.data[0].h2 = 0; } );
  }
  /*! returns the application specific value of a node */
  uint32_t get_weight( node const& n ) const
  {
    return _storage->nodes[n].data[0].h2;
  }
  /*! sets the application specific value of a node */
  void set_weight( node const& n, uint32_t v ) const
  {
    _storage->nodes[n].data[0].h2 = v;
  }

#pragma endregion

#pragma region Visited flags
  void clear_visited() const
  {
    std::for_each( _storage->nodes.begin(), _storage->nodes.end(), []( auto& n ) { n.data[1].h2 = 0; } );
  }

  auto visited( node const& n ) const
  {
    return _storage->nodes[n].data[1].h2;
  }

  void set_visited( node const& n, uint32_t v ) const
  {
    _storage->nodes[n].data[1].h2 = v;
  }

#pragma endregion


public:
  std::shared_ptr<abstract_storage> _storage;
};

} // namespace caterpillar
