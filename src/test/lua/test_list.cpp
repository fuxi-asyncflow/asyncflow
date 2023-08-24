#include "catch.hpp"
#include "util/list.hpp"

class TestNode : public LinkedNode<TestNode>
{
public:
	TestNode(int i) :value(i) {}
	int value;
};

TEST_CASE("test list")
{
	LinkedList<TestNode> list;
	// empty
	REQUIRE(list.empty());

	// push_front
	auto* node1 = new TestNode(1);
	list.push_front(node1);
	REQUIRE(!list.empty());
	REQUIRE(list.front() == node1);
	REQUIRE(list.back() == node1);

	// push_back
	auto* node2 = new TestNode(2);
	list.push_back(node2);
	REQUIRE(list.front() == node1);
	REQUIRE(list.back() == node2);

	auto* node0 = new TestNode(0);
	list.push_front(node0);
	REQUIRE(list.front() == node0);
	REQUIRE(list.back() == node2);

	REQUIRE(list.pop_front() == node0);
	REQUIRE(list.front() == node1);

	REQUIRE(list.pop_back() == node2);
	REQUIRE(list.back() == node1);

	REQUIRE(list.pop_back() == node1);
	REQUIRE(list.empty());

	REQUIRE(list.front() == nullptr);
	REQUIRE(list.back() == nullptr);
}
