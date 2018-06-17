#pragma once

#include <memory>
#include <vector>

template <typename Key, typename Value>
class Node
{
public:

	using Ptr = std::shared_ptr<Node<Key, Value>>;
	using WeakPtr = std::weak_ptr<Node<Key, Value>>;

	template <typename _Key, typename _Value>
	Node(_Key&& key, _Value&& value) :
		mKey(std::forward<_Key>(key)), mValue(std::forward<_Value>(value)) {}

	template <typename _Key, typename _Value>
	Node(const Ptr& parent, _Key&& key, _Value&& value) :
		mParent(parent), mKey(std::forward<_Key>(key)), mValue(std::forward<_Value>(value)) {}

	template <typename _Key, typename _Value>
	static Ptr create(_Key&& key, _Value&& value)
	{
		return std::make_shared<Node<Key, Value>>(std::forward<_Key>(key), std::forward<_Value>(value));
	}

	template <typename _Key, typename _Value>
	static Ptr create(const Ptr& parent, _Key&& key, _Value&& value)
	{
		return std::make_shared<Node<Key, Value>>(parent, std::forward<_Key>(key), std::forward<_Value>(value));
	}

	Key mKey;
	Value mValue;

	WeakPtr mParent;
	std::vector<Ptr> mChildren;
};