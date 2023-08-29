#pragma once

class LinkedNode
{
public:
	LinkedNode() : prev(nullptr), next(nullptr)
	{

	}
	LinkedNode* prev;
	LinkedNode* next;
};

class LinkedList
{
	using TNode = LinkedNode;
public:
	LinkedList()
	{
		init();
	}	

	LinkedList(LinkedList& l) = delete;
	LinkedList(LinkedList&& l) = delete;

	virtual ~LinkedList()
	{
		if (list_ == nullptr)
			return;
		clear();
		delete list_;
		list_ = nullptr;
	}

	void init()
	{
		list_ = new LinkedNode;
		list_->next = list_;
		list_->prev = list_;
	}

	void swap(LinkedList& l)
	{
		auto* tmp = list_;
		list_ = l.list_;
		l.list_ = tmp;
	}

protected:
	LinkedNode* list_;

public:
	void push_front(TNode* node)
	{
		insert(node, list_, list_->next);
	}

	void push_back(TNode* node)
	{
		insert(node, list_->prev, list_);
	}

	static void insert(TNode* node, TNode* prev, TNode* next)
	{
		next->prev = node;
		node->next = next;
		node->prev = prev;
		prev->next = node;
	}

	bool empty()
	{
		return list_->next == list_;
	}

	static void remove_safe(TNode* node)
	{
		if (is_in_list(node))
			remove(node);
	}

	static void remove(TNode* node)
	{
		node->next->prev = node->prev;
		node->prev->next = node->next;
		node->next = nullptr;
		node->prev = nullptr;
	}

	static bool is_in_list(TNode* node)
	{
		if (node->next == nullptr || node->prev == nullptr)
			return false;
		return true;
	}

	TNode* front()
	{
		return empty() ? nullptr : list_->next;
	}

	TNode* back()
	{
		return empty() ? nullptr : list_->prev;
	}

	TNode* pop_front()
	{
		if (empty())
			return nullptr;
		auto* ret = list_->next;
		remove(ret);
		return ret;
	}

	TNode* pop_back()
	{
		if (empty())
			return nullptr;
		auto* ret = list_->prev;
		remove(ret);
		return ret;
	}

	void clear()
	{
		for(auto* cur = list_->next; cur != list_;)
		{
			auto* next = cur->next;
			cur->prev = nullptr;
			cur->next = nullptr;
			cur = next;
		}
	}

	std::size_t size()
	{
		size_t count = 0;
		for (auto* cur = list_->next; cur != list_; cur = cur->next)
		{
			count++;
		}
		return count;
	}
};