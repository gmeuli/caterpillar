#include <algorithm>

#pragma once
namespace caterpillar
{
  template<class Ntk>
  struct xag_depth_cost
  {
    uint32_t operator()( Ntk const& ntk, typename Ntk::node const& node ) const
    {
      std::vector<typename Ntk::node> drivers;
      ntk.foreach_po( [&] (auto s)
      {
        drivers.push_back(ntk.get_node(s));
      });
      if(ntk.is_and(node) || std::find(drivers.begin(), drivers.end(), node) != drivers.end())
        return 1u;
      else return 0u;
    }
  };

  template<class Ntk>
  struct and_depth_cost
  {
    uint32_t operator()( Ntk const& ntk, typename Ntk::node const& node ) const
    {

      if(ntk.is_and(node))
        return 1u;
      else return 0u;
    }
  };
}