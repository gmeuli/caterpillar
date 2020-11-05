#pragma once

#include <chrono>
#include <mockturtle/utils/progress_bar.hpp>
#include <caterpillar/synthesis/strategies/action.hpp>
#include <caterpillar/solvers/z3_solver.hpp>
#include <type_traits>
#include <limits>


using namespace std::chrono;

namespace caterpillar
{

struct pebbling_mapping_strategy_params
{
  /*! \brief Show progress bar. */
  bool progress{false};

  /*! \brief Print solution. */
  bool verbose{false};

  /*! \brief Maximum number of pebbles to use, if supported by mapping strategy (0 means no limit). */
  uint32_t pebble_limit{0u};

  /*! \brief Maximum number of steps */
  uint32_t max_steps{std::numeric_limits<uint32_t>::max()};

  /*! \brief Conflict limit for the SAT solver (0 means no limit). */
  uint32_t conflict_limit{0};

  /*! \brief Timeout for the solver in milliseconds. */
  uint32_t solver_timeout{0};

  /*! \brief Timeout for the iterative quests in seconds. */
  uint32_t search_timeout{30};

  /*! \brief Increment pebble numbers, if a failure occurs. */
  bool increment_pebbles_on_failure{false};

  /*! \brief Decrement pebble numbers, if satisfiable. */
  bool decrement_pebbles_on_success{false};

  /*! \brief Decrement max weight, if satisfiable. */
  bool optimize_weight{false};

};

template<typename Ntk>
using Steps = std::vector<std::pair<typename Ntk::node, mapping_strategy_action>>;

template <typename Solver, typename Ntk>
inline Steps<Ntk> pebble (Ntk ntk, pebbling_mapping_strategy_params const& ps = {})
{
  assert( !ps.decrement_pebbles_on_success || !ps.increment_pebbles_on_failure );
  assert( !ps.decrement_pebbles_on_success || !ps.optimize_weight );
  assert( !ps.increment_pebbles_on_failure || !ps.optimize_weight );

  auto limit = ps.pebble_limit;
  
  auto start = high_resolution_clock::now(); 

  Steps<Ntk> steps;
  while ( true )
  {
    Solver solver( ntk, limit, ps.conflict_limit, ps.solver_timeout);
    typename Solver::result result;

    solver.init();

    mockturtle::progress_bar bar( 100, "|{0}| current step = {1}", ps.progress );

    do
    {
      if ( solver.current_step() >= ps.max_steps )
      {
        result = solver.unknown();
        break;
      }

      bar( std::min<uint32_t>( solver.current_step(), 100 ), solver.current_step() );

      solver.add_step();
      result = solver.solve(); 

    } while ( result == solver.unsat() && 
        duration_cast<seconds>(high_resolution_clock::now() - start).count() <= ps.search_timeout);

    if ( result == solver.unknown() || result == solver.unsat() )
    {
      if ( ps.increment_pebbles_on_failure )
      {
        limit++;
        continue;
      }
    }
    else if ( result == solver.sat() )
    {
      solver.save_model();
      #ifdef USE_Z3
             
      if(ps.optimize_weight)
      {
        if constexpr (std::is_same_v<Solver, z3_pebble_solver<Ntk>>)
        { 
          solver.optimize_solution();
        }
      }

      #endif

      steps = solver.extract_result();

      if ( ps.decrement_pebbles_on_success && limit > 1)
      {
        limit--;
        continue;
      }

    }

    return steps;
  }

}

}//caterpillar