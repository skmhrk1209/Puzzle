#pragma once

template<typename T, typename Container, typename Compare>
class PriorityQueue
{
public:

	using value_type = typename Container::value_type;
	using reference = typename Container::reference;
	using const_reference = typename Container::const_reference;
	using size_type = typename Container::size_type;
	using difference_type = typename Container::difference_type;
	using iterator = typename Container::iterator;
	using const_iterator = typename Container::const_iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	const_iterator cbegin() const { return mContainer.cbegin(); }
	const_iterator cend() const { return mContainer.cend(); }

	const_reverse_iterator rcbegin() const { return mContainer.rcbegin(); }
	const_reverse_iterator rcend() const { return mContainer.rcend(); }

	PriorityQueue() : mContainer(), mCompare() {}

	PriorityQueue(const Container& container) : mContainer(container), mCompare()
	{
		std::make_heap(mContainer.begin(), mContainer.end(), mCompare);
	}

	PriorityQueue(Container&& container) : mContainer(std::move(container)), mCompare()
	{
		std::make_heap(mContainer.begin(), mContainer.end(), mCompare);
	}

	template<class Iterator>
	PriorityQueue(Iterator first, Iterator last) : mContainer(first, last), mCompare()
	{
		std::make_heap(mContainer.begin(), mContainer.end(), mCompare);
	}

	void push(const value_type& value)
	{
		mContainer.push_back(value);
		std::push_heap(mContainer.begin(), mContainer.end(), mCompare);
	}

	void push(value_type&& value)
	{
		mContainer.push_back(std::move(value));
		std::push_heap(mContainer.begin(), mContainer.end(), mCompare);
	}

	template<class... Args>
	void emplace(Args&&... args)
	{
		mContainer.emplace_back(std::forward<Args>(args)...);
		std::push_heap(mContainer.begin(), mContainer.end(), mCompare);
	}

	void pop()
	{
		std::pop_heap(mContainer.begin(), mContainer.end(), mCompare);
		mContainer.pop_back();
	}

	const_reference top() const
	{
		return mContainer.front();
	}

	bool empty() const
	{
		return mContainer.empty();
	}

	size_type size() const
	{
		return mContainer.size();
	}

	void erase(const_iterator iterator)
	{
		erase(std::distance(mContainer.cbegin(), iterator));
	}

protected:

	iterator begin() { return mContainer.begin(); }
	iterator end() { return mContainer.end(); }
	const_iterator begin() const { return mContainer.begin(); }
	const_iterator end() const { return mContainer.end(); }

	reverse_iterator rbegin() { return mContainer.rbegin(); }
	reverse_iterator rend() { return mContainer.rend(); }
	const_reverse_iterator rbegin() const { return mContainer.rbegin(); }
	const_reverse_iterator rend() const { return mContainer.rend(); }

	void erase(size_type index)
	{
		if (index == mContainer.size() - 1)
		{
			mContainer.pop_back();
		}
		else
		{
			mContainer[index] = std::move(mContainer.back());
			mContainer.pop_back();

			upHeap(index);
			downHeap(index);
		}
	}

	void upHeap(size_type index)
	{
		while (index)
		{
			auto parentIndex((index - 1) >> 1);

			if (mCompare(mContainer[parentIndex], mContainer[index]))
			{
				std::swap(mContainer[parentIndex], mContainer[index]);
				index = parentIndex;
			}
			else break;
		}
	}

	void downHeap(size_type index)
	{
		while (true)
		{
			auto childIndexLeft((index << 1) + 1);
			auto childIndexRight((index << 1) + 2);

			if (childIndexLeft < mContainer.size() && childIndexRight < mContainer.size())
			{
				if (mCompare(mContainer[childIndexLeft], mContainer[childIndexRight]))
				{
					if (mCompare(mContainer[index], mContainer[childIndexRight]))
					{
						std::swap(mContainer[index], mContainer[childIndexRight]);
						index = childIndexRight;
					}
					else break;
				}
				else
				{
					if (mCompare(mContainer[index], mContainer[childIndexLeft]))
					{
						std::swap(mContainer[index], mContainer[childIndexLeft]);
						index = childIndexLeft;
					}
					else break;
				}
			}
			else if (childIndexLeft < mContainer.size())
			{
				if (mCompare(mContainer[index], mContainer[childIndexLeft]))
				{
					std::swap(mContainer[index], mContainer[childIndexLeft]);
					index = childIndexLeft;
				}
				else break;
			}
			else break;
		}
	}

	Container mContainer;
	Compare mCompare;
};