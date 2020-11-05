/* mockturtle: C++ logic network library
 * Copyright (C) 2018-2019  EPFL
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*!
  \file experiments.hpp
  \brief Framework for simple experimental evaluation

  \author Mathias Soeken
*/

#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include <fmt/color.h>
#include <fmt/format.h>
#include <json.hpp>

#include <caterpillar/structures/stg_gate.hpp>
#include <caterpillar/verification/circuit_to_logic_network.hpp>
#include <caterpillar/details/depth_costs.hpp>
#include <tweedledum/networks/netlist.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/abstract_xag.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/algorithms/equivalence_checking.hpp>
#include <mockturtle/algorithms/miter.hpp>
#include <mockturtle/views/depth_view.hpp>

using namespace mockturtle;

namespace experiments
{

struct json_table
{
  explicit json_table( nlohmann::json const& data, std::vector<std::string> const& columns )
      : columns_( columns )
  {
    for ( auto const& column : columns )
    {
      max_widths_.push_back( column.size() );
    }
    entries_.push_back( columns );
    for ( auto const& row : data )
    {
      add_row( row );
    }
  }

  void print( std::ostream& os )
  {
    for ( const auto& entry : entries_ )
    {
      os << "|";
      for ( auto i = 0u; i < entry.size(); ++i )
      {
        os << fmt::format( " {1:>{0}} |", max_widths_[i], entry[i] );
      }
      os << "\n";
    }
  }

private:
  void add_row( nlohmann::json const& row )
  {
    std::vector<std::string> entry;
    uint32_t ctr{0u};
    for ( auto const& key : columns_ )
    {
      auto const& data = row[key];
      std::string cell;

      if ( data.is_string() )
      {
        cell = static_cast<std::string>( data );
      }
      else if ( data.is_number_integer() )
      {
        cell = std::to_string( static_cast<int>( data ) );
      }
      else if ( data.is_number() )
      {
        cell = fmt::format( "{:.2f}", static_cast<float>( data ) );
      }
      else if ( data.is_boolean() )
      {
        cell = fmt::format( "{}", static_cast<bool>( data ) );
      }

      max_widths_[ctr] = std::max<uint32_t>( max_widths_[ctr], cell.size() );
      ++ctr;
      entry.push_back( cell );
    }
    entries_.push_back( entry );
  }

private:
  std::vector<uint32_t> max_widths_;
  std::vector<std::string> columns_;
  std::vector<std::vector<std::string>> entries_;
};

static constexpr const char* use_github_revision = "##GITHUB##";

template<typename T, typename... Ts>
struct first_type
{
  using type = T;
};

template<typename... Ts>
using first_type_t = typename first_type<Ts...>::type;

template<typename... ColumnTypes>
class experiment
{
public:
  template<typename... T>
  explicit experiment( std::string_view name, T... column_names )
      : name_( name )
  {
    static_assert( ( sizeof...( ColumnTypes ) > 0 ), "at least one column must be specified" );
    static_assert( ( sizeof...( ColumnTypes ) == sizeof...( T ) ), "number of column names must match column types" );
    static_assert( ( std::is_constructible_v<std::string, T> && ... ), "all column names must be strings" );
    ( column_names_.push_back( column_names ), ... );

#ifndef EXPERIMENTS_PATH
    filename_ = fmt::format( "{}.json", name );
#else
    filename_ = fmt::format( "{}{}.json", EXPERIMENTS_PATH, name );
#endif

    std::ifstream in( filename_, std::ifstream::in );
    if ( in.good() )
    {
      data_ = nlohmann::json::parse( in );
    }
  }

  void save( std::string_view version = use_github_revision )
  {
    nlohmann::json entries;
    for ( auto const& row : rows_ )
    {
      auto it = column_names_.begin();
      nlohmann::json entry;
      std::apply(
          [&]( auto&&... args ) {
            ( ( entry[*it++] = args ), ... );
          },
          row );
      entries.push_back( entry );
    }

    std::string version_;
    version_ = version;
#ifdef GIT_SHORT_REVISION
    if ( version == experiments::use_github_revision )
    {
      version_ = GIT_SHORT_REVISION;
    }
#endif

    if ( !data_.empty() && data_.back()["version"] == version_ )
    {
      data_.erase( data_.size() - 1u );
    }

    data_.push_back( {{"version", version_},
                      {"entries", entries}} );

    std::ofstream os( filename_, std::ofstream::out );
    os << data_.dump( 2 ) << "\n";
  }

  void operator()( ColumnTypes... args )
  {
    rows_.emplace_back( args... );
  }

  nlohmann::json const& dataset( std::string const& version, nlohmann::json const& def ) const
  {
    if ( version.empty() )
    {
      return def;
    }
    else
    {
      if ( const auto it = std::find_if( data_.begin(), data_.end(), [&]( auto const& entry ) { return entry["version"] == version; } ); it != data_.end() )
      {
        return *it;
      }
      else
      {
        throw std::exception();
      }
    }
  }

  bool table( std::string const& version = {}, std::ostream& os = std::cout ) const
  {
    if ( data_.empty() )
    {
      fmt::print( "[w] no data available\n" );
      return false;
    }

    try
    {
      auto const& data = dataset( version, data_.back() );
      fmt::print( "[i] dataset " );
      fmt::print( fg( fmt::terminal_color::blue ), "{}\n", data["version"] );

      json_table( data["entries"], column_names_ ).print( os );
    }
    catch ( ... )
    {
      fmt::print( "[w] version {} not found\n", version );
      return false;
    }

    return true;
  }

  bool compare( std::string const& old_version = {},
                std::string const& current_version = {},
                std::vector<std::string> const& track_columns = {},
                std::ostream& os = std::cout )
  {
    if ( data_.size() < 2u )
    {
      fmt::print( "[w] dataset contains less than two entry sets\n" );
      return false;
    }

    try
    {
      auto const& data_old = dataset( old_version, data_[data_.size() - 2u] );
      auto const& data_cur = dataset( current_version, data_.back() );

      auto const& entries_old = data_old["entries"];
      auto const& entries_cur = data_cur["entries"];

      fmt::print( "[i] compare " );
      fmt::print( fg( fmt::terminal_color::blue ), "{}", data_old["version"] );
      fmt::print( " to " );
      fmt::print( fg( fmt::terminal_color::blue ), "{}\n", data_cur["version"] );

      /* collect keys */
      using first_t = first_type_t<ColumnTypes...>;
      std::vector<first_t> keys;
      for ( auto const& entry : entries_cur )
      {
        nlohmann::json const& j = entry[column_names_.front()];
        keys.push_back( j.get<first_t>() );
      }
      for ( auto const& entry : entries_old )
      {
        nlohmann::json const& j = entry[column_names_.front()];
        auto value = j.get<first_t>();
        if ( std::find( keys.begin(), keys.end(), value ) == keys.end() )
        {
          keys.push_back( value );
        }
      }

      /* track differences */
      std::unordered_map<std::string, uint32_t> differences;
      for ( auto const& column : track_columns )
      {
        differences[column] = 0u;
      }

      /* prepare entries */
      auto find_key = [&]( nlohmann::json const& entries, first_t const& key ) {
        return std::find_if( entries.begin(), entries.end(), [&]( auto const& entry ) {
          nlohmann::json const& j = entry[column_names_.front()];
          return j.get<first_t>() == key;
        } );
      };

      auto compare_columns = column_names_;
      std::transform( column_names_.begin() + 1, column_names_.end(), std::back_inserter( compare_columns ),
                      []( auto const& name ) { return name + "'"; } );

      nlohmann::json compare_entries;
      for ( auto const& key : keys )
      {
        nlohmann::json row;
        const auto it_old = find_key( entries_old, key );
        if ( it_old != entries_old.end() )
        {
          row = *it_old;
        }
        if ( auto const it = find_key( entries_cur, key ); it != entries_cur.end() )
        {
          if ( it_old == entries_old.end() )
          {
            row[column_names_[0]] = (*it)[column_names_[0]];
          }
          for ( auto i = 1u; i < column_names_.size(); ++i )
          {
            row[column_names_[i] + "'"] = (*it)[column_names_[i]];

            if ( it_old != entries_old.end() )
            {
              if ( const auto it_diff = differences.find( column_names_[i] ); it_diff != differences.end() && row[column_names_[i]] != row[column_names_[i] + "'"] )
              {
                it_diff->second++;
              }
            }
          }
        }
        compare_entries.push_back( row );
      }

      json_table( compare_entries, compare_columns ).print( os );

      for ( const auto& [k, v] : differences ) {
        if ( v == 0u ) {
          os << fmt::format( "[i] no differences in column '{}'\n", k );
        } else {
          os << fmt::format( "[i] {} differences in column '{}'\n", v, k );
        }
      }
    }
    catch ( ... )
    {
      fmt::print( "[w] dataset not found\n" );
      return false;
    }

    return true;
  }

private:
  std::string name_;
  std::string filename_;
  std::vector<std::string> column_names_;
  std::vector<std::tuple<ColumnTypes...>> rows_;

  nlohmann::json data_;
};

template <typename Ntk>
bool check_equivalence_tt(Ntk const& ntk, tweedledum::netlist<caterpillar::stg_gate> const& rev, std::vector<uint32_t> const pi_lines, std::vector<uint32_t> const po_lines)
{
  auto n_pis = ntk.num_pis();
  assert (n_pis == pi_lines.size());

  const auto ntk_rev = caterpillar::circuit_to_logic_network<Ntk, tweedledum::netlist<caterpillar::stg_gate>>( rev, pi_lines, po_lines );
  auto tt_ntk = simulate<kitty::dynamic_truth_table>( ntk, {n_pis} );
  auto tt_rev = simulate<kitty::dynamic_truth_table>( *ntk_rev, {n_pis} );
 
  return (tt_ntk == tt_rev);
}

template <typename Ntk>
std::optional<bool> check_equivalence_ntk(Ntk const& ntk, tweedledum::netlist<caterpillar::stg_gate> const& rev, std::vector<uint32_t> const pi_lines, std::vector<uint32_t> const po_lines)
{
  auto n_pis = ntk.num_pis();
  assert (n_pis == pi_lines.size());
  
  const auto ntk_rev = caterpillar::circuit_to_logic_network<Ntk, tweedledum::netlist<caterpillar::stg_gate>>( rev, pi_lines, po_lines );
  equivalence_checking_params ps;
  ps.verbose = true;
  if(auto mit = miter<Ntk>(ntk, *ntk_rev))
  { 
    return equivalence_checking(*mit, ps);
  }
  else 
  {
    return std::nullopt;
  }
}

template<typename Ntk>
struct xag_stats
{
  xag_stats(Ntk const& ntk)
  {
    depth_view<Ntk, caterpillar::and_depth_cost<Ntk>> xag {ntk};

    std::vector<uint32_t> lvls (xag.depth());

    xag.foreach_node( [&]( auto n ) {
      if( xag.is_and(n) ) 
      {
        lvls[xag.level(n)-1]++; 
        n_and++;
      }
      else if(ntk.is_nary_xor(n) || ntk.is_xor(n))
      {
        n_xor++;
      }
    });

    mult_depth = (n_and == 0) ? 0 : xag.depth();
    mult_width = (mult_depth == 0) ? 0 : lvls[std::max_element(lvls.begin(), lvls.end()) - lvls.begin()];
  }

  void print()
  {
    std::cout << fmt::format("AND = {}, XOR = {}, MD = {}, MW = {}\n", n_and, n_xor, mult_depth, mult_width);
  }

  uint32_t n_and = 0;
  uint32_t n_xor = 0;
  uint32_t mult_depth = 0;
  uint32_t mult_width = 0;
};


} // namespace experiments
