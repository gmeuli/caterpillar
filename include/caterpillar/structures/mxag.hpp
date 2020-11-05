/*-------------------------------------------------------------------------------------------------
| This file is distributed under the MIT License.
| See accompanying file /LICENSE for mockturtle::details.
| Author(s): Giulia Meuli
*------------------------------------------------------------------------------------------------*/


#pragma once

#include <memory>
#include <optional>
#include <stack>
#include <string>

#include <ez/direct_iterator.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/operators.hpp>

#include <mockturtle/traits.hpp>
#include <mockturtle/utils/algorithm.hpp>
#include <mockturtle/networks/detail/foreach.hpp>
#include <mockturtle/networks/events.hpp>
#include <mockturtle/networks/storage.hpp>

namespace caterpillar
{

using namespace mockturtle;

/*! \brief Hash function for mxags -- the same as for mxags (from ABC) */
template<class Node>
struct mxag_hash
{
  uint64_t operator()( Node const& n ) const
  {
    uint64_t seed = -2011;
    std::vector<uint64_t> elem1 = {7937, 2971, 5423, 1567, 988, 532, 3488};
    std::vector<uint64_t> elem2 = {911, 353, 763, 612, 850, 290, 315};
    for ( auto i = 0u; i < n.children.size(); i++)
    {
      seed += n.children[i].index * elem1[i%7];
    }
    for ( auto i = 0u; i < n.children.size(); i++)
    {
      seed += n.children[i].weight * elem2[i%7];
    }
    return seed;
  }
};

struct mxag_storage_data
{
  uint32_t num_pis = 0u;
  uint32_t num_pos = 0u;
  std::vector<int8_t> latches;
  uint32_t trav_id = 0;
};

/*! \brief mxag storage container

  mxags have nodes with fan-in 2.  We split of one bit of the index pointer to
  store a complemented attribute.  Every node has 64-bit of additional data
  used for the following purposes:

  `data[0].h1`: Fan-out size (we use MSB to indicate whether a node is dead)
  `data[0].h2`: Application-specific value
  `data[1].h1`: Visited flag
*/
using mxag_storage = mockturtle::storage<mockturtle::mixed_fanin_node<2, 1>,
                            mxag_storage_data,
                            mxag_hash<mockturtle::mixed_fanin_node<2, 1>>>;

class mxag_network
{
public:
#pragma region Types and constructors
  static constexpr auto min_fanin_size = 2u;
  static constexpr auto max_fanin_size = 2u;

  using base_type = mxag_network;
  using storage = std::shared_ptr<mxag_storage>;
  using node = uint64_t;

  struct signal
  {
    signal() = default;

    signal( uint64_t index, uint64_t complement )
        : complement( complement ), index( index )
    {
    }

    explicit signal( uint64_t data )
        : data( data )
    {
    }

    signal( mxag_storage::node_type::pointer_type const& p )
        : complement( p.weight ), index( p.index )
    {
    }

    union {
      struct
      {
        uint64_t complement : 1;
        uint64_t index : 63;
      };
      uint64_t data;
    };

    signal operator!() const
    {
      return signal( data ^ 1 );
    }

    signal operator+() const
    {
      return {index, 0};
    }

    signal operator-() const
    {
      return {index, 1};
    }

    signal operator^( bool complement ) const
    {
      return signal( data ^ ( complement ? 1 : 0 ) );
    }

    bool operator==( signal const& other ) const
    {
      return data == other.data;
    }

    bool operator!=( signal const& other ) const
    {
      return data != other.data;
    }

    bool operator<( signal const& other ) const
    {
      return data < other.data;
    }

    operator mxag_storage::node_type::pointer_type() const
    {
      return {index, complement};
    }
  };

  mxag_network()
      : _storage( std::make_shared<mxag_storage>() ),
        _events( std::make_shared<decltype( _events )::element_type>() )
  {
  }

  mxag_network( std::shared_ptr<mxag_storage> storage )
      : _storage( storage ),
        _events( std::make_shared<decltype( _events )::element_type>() )
  {
  }
#pragma endregion

#pragma region Primary I / O and constants
  signal get_constant( bool value ) const
  {
    return {0, static_cast<uint64_t>( value ? 1 : 0 )};
  }

  signal create_pi( std::string const& name = {} )
  {
    (void)name;

    const auto index = _storage->nodes.size();
    //auto& node = _storage->nodes.emplace_back();
    _storage->nodes.emplace_back();
    _storage->inputs.emplace_back( index );

    //node.children[0].data = node.children[1].data = _storage->inputs.size();
    //++_storage->data.num_pis;
    return {index, 0};
  }

  uint32_t create_po( signal const& f, std::string const& name = {} )
  {
    (void)name;

    /* increase ref-count to children */
    _storage->nodes[f.index].data[0].h1++;
    auto const po_index = _storage->outputs.size();
    _storage->outputs.emplace_back( f.index, f.complement );
    //++_storage->data.num_pos;
    return po_index;
  }

/*
  int8_t latch_reset( uint32_t index ) const
  {
    assert( index < _storage->data.latches.size() );
    return _storage->data.latches[index];
  }

  bool is_combinational() const
  {
    return ( static_cast<uint32_t>( _storage->inputs.size() ) == _storage->data.num_pis &&
             static_cast<uint32_t>( _storage->outputs.size() ) == _storage->data.num_pos );
  }
*/
  bool is_constant( node const& n ) const
  {
    return n == 0;
  }

  

  bool is_pi( node const& n ) const
  {
    return n >= 1 && _storage->nodes[n].children.empty();
  }

 
  bool constant_value( node const& n ) const
  {
    (void)n;
    return false;
  }

#pragma endregion

#pragma region Create unary functions
  signal create_buf( signal const& a )
  {
    return a;
  }

  signal create_not( signal const& a )
  {
    return !a;
  }
#pragma endregion

#pragma region Create binary functions
  signal _create_node( signal a, signal b )
  {
    storage::element_type::node_type node;
    node.children.push_back(a);
    node.children.push_back(b);

    /* structural hashing */
    const auto it = _storage->hash.find( node );
    if ( it != _storage->hash.end() )
    {
      return {it->second, 0};
    }

    const auto index = _storage->nodes.size();

    if ( index >= .9 * _storage->nodes.capacity() )
    {
      _storage->nodes.reserve( static_cast<uint64_t>( 3.1415f * index ) );
      _storage->hash.reserve( static_cast<uint64_t>( 3.1415f * index ) );
    }

    _storage->nodes.push_back( node );

    _storage->hash[node] = index;

    /* increase ref-count to children */
    _storage->nodes[a.index].data[0].h1++;
    _storage->nodes[b.index].data[0].h1++;

    for ( auto const& fn : _events->on_add )
    {
      fn( index );
    }

    return {index, 0};
  }

  signal _create_multi_node( std::vector<signal> const& children )
  {
    storage::element_type::node_type node;
    std::copy( children.begin(), children.end(), std::back_inserter( node.children ) );

    /* structural hashing */
    const auto it = _storage->hash.find( node );
    if ( it != _storage->hash.end() )
    {
      return {it->second, 0};
    }

    const auto index = _storage->nodes.size();

    if ( index >= .9 * _storage->nodes.capacity() )
    {
      _storage->nodes.reserve( static_cast<uint64_t>( 3.1415f * index ) );
      _storage->hash.reserve( static_cast<uint64_t>( 3.1415f * index ) );
    }

    _storage->nodes.push_back( node );

    _storage->hash[node] = index;

    /* increase ref-count to children */
    for(auto s : children)
    {
      _storage->nodes[s.index].data[0].h1++;
    }

    for ( auto const& fn : _events->on_add )
    {
      fn( index );
    }

    return {index, 0};
  }

  signal create_and( signal a, signal b )
  {
    /* order inputs a < b it is a AND */
    if ( a.index > b.index )
    {
      std::swap( a, b );
    }
    if ( a.index == b.index )
    {
      return a.complement == b.complement ? a : get_constant( false );
    }
    else if ( a.index == 0 )
    {
      return a.complement == false ? get_constant( false ) : b;
    }
    return _create_node( a, b );
  }

  signal create_nand( signal const& a, signal const& b )
  {
    return !create_and( a, b );
  }

  signal create_or( signal const& a, signal const& b )
  {
    return !create_and( !a, !b );
  }

  signal create_nor( signal const& a, signal const& b )
  {
    return create_and( !a, !b );
  }

  signal create_multi_xor( std::vector<signal> children )
  {
    bool f_compl;
    assert( children.size() >= 2);
    /* order inputs a > b it is a XOR */
    if ( children[0].index < children[1].index )
    {
      std::swap(  children[0],  children[1] );
    }

    for( auto s : children)
    {
      f_compl = f_compl ^ s.complement;
      s.complement = false;
    }

    if( std::adjacent_find( children.begin(), children.end(), std::not_equal_to<>() ) == children.end())
    {
      return get_constant( f_compl );
    }
    else if ( children[1].index == 0 )
    {
      return children[0] ^ f_compl;
    }

    return _create_multi_node( children ) ^ f_compl;
  }

  signal create_xor( signal a, signal b )
  {
    /* order inputs a > b it is a XOR */
    if ( a.index < b.index )
    {
      std::swap( a, b );
    }

    bool f_compl = a.complement != b.complement;
    a.complement = b.complement = false;

    if ( a.index == b.index )
    {
      return get_constant( f_compl );
    }
    else if ( b.index == 0 )
    {
      return a ^ f_compl;
    }

    return _create_node( a, b ) ^ f_compl;
  }

  signal create_xnor( signal const& a, signal const& b )
  {
    return !create_xor( a, b );
  }
#pragma endregion

#pragma region Create ternary functions
  signal create_ite( signal cond, signal f_then, signal f_else )
  {
    bool f_compl{false};
    if ( f_then.index < f_else.index )
    {
      std::swap( f_then, f_else );
      cond.complement ^= 1;
    }
    if ( f_then.complement )
    {
      f_then.complement = 0;
      f_else.complement ^= 1;
      f_compl = true;
    }

    return create_xor( create_and( !cond, create_xor( f_then, f_else ) ), f_then ) ^ f_compl;
  }

  signal create_maj( signal const& a, signal const& b, signal const& c )
  {
    auto c1 = create_xor( a, b );
    auto c2 = create_xor( a, c );
    auto c3 = create_and( c1, c2 );
    return create_xor( a, c3 );
  }
#pragma endregion

#pragma region Create nary functions
  signal create_nary_and( std::vector<signal> const& fs )
  {
    return mockturtle::tree_reduce( fs.begin(), fs.end(), get_constant( true ), [this]( auto const& a, auto const& b ) { return create_and( a, b ); } );
  }

  signal create_nary_or( std::vector<signal> const& fs )
  {
    return mockturtle::tree_reduce( fs.begin(), fs.end(), get_constant( false ), [this]( auto const& a, auto const& b ) { return create_or( a, b ); } );
  }

  signal create_nary_xor( std::vector<signal> const& fs )
  {
    return mockturtle::tree_reduce( fs.begin(), fs.end(), get_constant( false ), [this]( auto const& a, auto const& b ) { return create_xor( a, b ); } );
  }
#pragma endregion

#pragma region Create arbitrary functions
  signal clone_node( mxag_network const& other, node const& source, std::vector<signal> const& children )
  {
    (void)other;
    (void)source;
    assert( children.size() == 2u );
    if ( other.is_and( source ) )
      //if ( children[0u].index < children[1u].index )
      return create_and( children[0u], children[1u] );
    else
      return create_xor( children[0u], children[1u] );
  }
#pragma endregion



#pragma region Structural properties
  auto
  size() const
  {
    return static_cast<uint32_t>( _storage->nodes.size() );
  }


  auto num_pis() const
  {
    return static_cast<uint32_t>(_storage->inputs.size() );
  }

  auto num_pos() const
  {
    return static_cast<uint32_t>(_storage->outputs.size() );
  }


  auto num_gates() const
  {
    return static_cast<uint32_t>( _storage->hash.size() );
  }

  uint32_t fanin_size( node const& n ) const
  {
    return static_cast<uint32_t>( _storage->nodes[n].children.size() );
  }

  uint32_t fanout_size( node const& n ) const
  {
    return _storage->nodes[n].data[0].h1 & UINT32_C( 0x7FFFFFFF );
  }

  uint32_t incr_fanout_size( node const& n ) const
  {
    return _storage->nodes[n].data[0].h1++ & UINT32_C( 0x7FFFFFFF );
  }

  uint32_t decr_fanout_size( node const& n ) const
  {
    return --_storage->nodes[n].data[0].h1 & UINT32_C( 0x7FFFFFFF );
  }

  bool is_and( node const& n ) const
  {
    return n > 0 && !is_pi( n ) && ( _storage->nodes[n].children[0].index < _storage->nodes[n].children[1].index );
  }

  bool is_or( node const& n ) const
  {
    (void)n;
    return false;
  }

  bool is_xor( node const& n ) const
  {
    return n > 0 && !is_pi( n ) && ( _storage->nodes[n].children[0].index > _storage->nodes[n].children[1].index );
  }

  bool is_maj( node const& n ) const
  {
    (void)n;
    return false;
  }

  bool is_ite( node const& n ) const
  {
    (void)n;
    return false;
  }

  bool is_xor3( node const& n ) const
  {
    (void)n;
    return false;
  }
#pragma endregion

#pragma region Functional properties
  kitty::dynamic_truth_table node_function( const node& n ) const
  {
    kitty::dynamic_truth_table _func( 2 );
    if ( _storage->nodes[n].children[0u].index < _storage->nodes[n].children[1u].index )
    {
      _func._bits[0] = 0x8;
      return _func;
    }
    else
    {
      _func._bits[0] = 0x6;
      return _func;
    }
  }
#pragma endregion

#pragma region Nodes and signals
  node get_node( signal const& f ) const
  {
    return f.index;
  }

  signal make_signal( node const& n ) const
  {
    return signal( n, 0 );
  }

  bool is_complemented( signal const& f ) const
  {
    return f.complement;
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
  template<typename Fn>
  void foreach_node( Fn&& fn ) const
  {
    mockturtle::detail::foreach_element( ez::make_direct_iterator<uint64_t>( 0 ),
                                ez::make_direct_iterator<uint64_t>( _storage->nodes.size() ),
                                fn );
  }


  template<typename Fn>
  void foreach_pi( Fn&& fn ) const
  {
    detail::foreach_element( _storage->inputs.begin(), _storage->inputs.end(), fn );
  }

  template<typename Fn>
  void foreach_po( Fn&& fn ) const
  {
    detail::foreach_element( _storage->outputs.begin(), _storage->outputs.end(), fn );
  }

  template<typename Fn>
  void foreach_po_index( Fn&& fn ) const
  {
    using IteratorType = decltype( _storage->outputs.begin() );
    detail::foreach_element_transform<IteratorType, uint32_t>( _storage->outputs.begin(), _storage->outputs.end(), []( auto o ) { return o.index; }, fn );
  }


  template<typename Fn>
  void foreach_gate( Fn&& fn ) const
  {
    mockturtle::detail::foreach_element_if( ez::make_direct_iterator<uint64_t>( 1 ), /* start from 1 to avoid constant */
                                ez::make_direct_iterator<uint64_t>( _storage->nodes.size() ),
                                [this]( auto n ) { return !is_pi( n ); },
                                fn );
  }

  template<typename Fn>
  void foreach_fanin( node const& n, Fn&& fn ) const
  {
    if ( n == 0 || is_pi( n ) )
      return;

    detail::foreach_element( _storage->nodes[n].children.begin(), _storage->nodes[n].children.end(), fn );
    
  }
#pragma endregion



#pragma region Custom node values
  void clear_values() const
  {
    std::for_each( _storage->nodes.begin(), _storage->nodes.end(), []( auto& n ) { n.data[0].h2 = 0; } );
  }

  auto value( node const& n ) const
  {
    return _storage->nodes[n].data[0].h2;
  }

  void set_value( node const& n, uint32_t v ) const
  {
    _storage->nodes[n].data[0].h2 = v;
  }

  auto incr_value( node const& n ) const
  {
    return _storage->nodes[n].data[0].h2++;
  }

  auto decr_value( node const& n ) const
  {
    return --_storage->nodes[n].data[0].h2;
  }
#pragma endregion

#pragma region Visited flags
  void clear_visited() const
  {
    std::for_each( _storage->nodes.begin(), _storage->nodes.end(), []( auto& n ) { n.data[1].h1 = 0; } );
  }

  auto visited( node const& n ) const
  {
    return _storage->nodes[n].data[1].h1;
  }

  void set_visited( node const& n, uint32_t v ) const
  {
    _storage->nodes[n].data[1].h1 = v;
  }

  uint32_t trav_id() const
  {
    return _storage->data.trav_id;
  }

  void incr_trav_id() const
  {
    ++_storage->data.trav_id;
  }
#pragma endregion

#pragma region General methods
  auto& events() const
  {
    return *_events;
  }
#pragma endregion

public:
  std::shared_ptr<mxag_storage> _storage;
  std::shared_ptr<network_events<base_type>> _events;
};

} // namespace mockturtle

namespace std
{

template<>
struct hash<caterpillar::mxag_network::signal>
{
  uint64_t operator()( caterpillar::mxag_network::signal const& s ) const noexcept
  {
    uint64_t k = s.data;
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccd;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53;
    k ^= k >> 33;
    return k;
  }
}; /* hash */

} // namespace std
