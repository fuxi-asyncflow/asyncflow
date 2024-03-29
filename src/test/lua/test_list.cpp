#include "core/node.h"
#include "catch.hpp"
#include "core/custom_struct.h"
#include "core/node_data.h"

class TestNode : public LinkedNode
{
public:
	TestNode(int i) :value(i) {}
	int value;
};

TEST_CASE("test list")
{
	LinkedList list;
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

TEST_CASE("test list iterator")
{
	auto* node_data = new asyncflow::core::NodeData(0);
	asyncflow::core::NodeLinkedList list;
	auto* node_0 = new asyncflow::core::Node(nullptr, node_data);
	node_0->IncTrueCount();
	auto* node_1 = new asyncflow::core::Node(nullptr, node_data);
	node_1->IncTrueCount();
	node_1->IncTrueCount();
	auto* node_2 = new asyncflow::core::Node(nullptr, node_data);
	node_2->IncTrueCount();
	node_2->IncTrueCount();
	node_2->IncTrueCount();

	list.Push(node_0);
	list.Push(node_1);
	list.Push(node_2);

	int count = 0;
	for(auto* node: list)
	{
		count++;
	}
	REQUIRE(count == 3);

	// list.print();
	count = 0;
	int sum = 0;
	auto end = list.end();
	for (auto it = list.begin(); it != end;)
	{
		if ((*it)->GetTrueCount() == 2)
			it = list.Erase(it);
		else
		{
			sum += (*it)->GetTrueCount();
			++it;
		}
	}
	list.print();

	for (auto* node : list)
	{
		count++;
	}
	REQUIRE(count == 2);
	REQUIRE(sum == 4);

	asyncflow::core::NodeLinkedList new_list;
	new_list.swap(list);
	REQUIRE(list.empty());
	REQUIRE(new_list.size() == 2);

	REQUIRE(node_0->GetLink()->prev != nullptr);
	REQUIRE(node_0->GetLink()->next != nullptr);
	REQUIRE(node_1->GetLink()->prev == nullptr);
	REQUIRE(node_1->GetLink()->next == nullptr);
	REQUIRE(node_2->GetLink()->prev != nullptr);
	REQUIRE(node_2->GetLink()->next != nullptr);
	new_list.~NodeLinkedList();
	REQUIRE(node_0->GetLink()->prev == nullptr);
	REQUIRE(node_0->GetLink()->next == nullptr);
	REQUIRE(node_1->GetLink()->prev == nullptr);
	REQUIRE(node_1->GetLink()->next == nullptr);
	REQUIRE(node_2->GetLink()->prev == nullptr);
	REQUIRE(node_2->GetLink()->next == nullptr);
}

TEST_CASE("test list swap")
{
	auto* node_data = new asyncflow::core::NodeData(0);
	asyncflow::core::NodeLinkedList list;
	auto* node_0 = new asyncflow::core::Node(nullptr, node_data);	
	auto* node_1 = new asyncflow::core::Node(nullptr, node_data);	
	auto* node_2 = new asyncflow::core::Node(nullptr, node_data);

	asyncflow::core::NodeLinkedList lista;
	asyncflow::core::NodeLinkedList listb;

	lista.Push(node_0);
	lista.Push(node_1);
	listb.Push(node_2);

	REQUIRE(lista.size() == 2);
	REQUIRE(listb.size() == 1);

	REQUIRE(lista.front() == node_0->GetLink());
	REQUIRE(lista.back() == node_1->GetLink());
	REQUIRE(listb.front() == node_2->GetLink());
	REQUIRE(listb.back() == node_2->GetLink());

	lista.swap(listb);

	REQUIRE(listb.front() == node_0->GetLink());
	REQUIRE(listb.back() == node_1->GetLink());
	REQUIRE(lista.front() == node_2->GetLink());
	REQUIRE(lista.back() == node_2->GetLink());

	REQUIRE(lista.size() == 1);
	REQUIRE(listb.size() == 2);
}
