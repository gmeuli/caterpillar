#include<catch.hpp>

#include <caterpillar/structures/abstract_network.hpp>
#include <caterpillar/pebbling_solvers/z3_solver.hpp>

TEST_CASE("build a simple abstract graph", "[build simple abstract graph]")
{
    using namespace caterpillar;

    auto net = abstract_network();

    auto p1 = net.create_pi();
    auto p2 = net.create_pi();
    auto p3 = net.create_pi();
    
    auto n1 = net.create_node({p1, p2}, 1);
    auto n2 = net.create_node({n1, p3}, 2);

    net.create_po(n2);

    CHECK(net.num_pis() == 3);
    CHECK(net.num_pos() == 1);
    CHECK(net.num_gates() == 2);
}

TEST_CASE("build a simple abstract graph 3fi", "[build simple abstract graph 3fi]")
{
    using namespace caterpillar;

    auto net = abstract_network();

    auto p1 = net.create_pi();
    auto p2 = net.create_pi();
    auto p3 = net.create_pi();
    auto p4 = net.get_constant();
    
    auto n1 = net.create_node({p1, p2, p4}, 3);
    auto n2 = net.create_node({n1, p3}, 2);

    net.create_po(n2);

    CHECK(net.num_pis() == 3);
    CHECK(net.num_pos() == 1);
    CHECK(net.num_gates() == 2);


}

