#pragma once

#include <array>
#include <vector>
#include <deque>
#include <stack>
#include <queue>
#include <unordered_set>
#include <memory>
#include <functional>
#include <string>
#include <sstream>
#include <boost/functional/hash.hpp>
#include "priority_queue.hpp"
#include "node.hpp"

#define DEBUG

template <int N, typename Key>
class Puzzle
{
	using Value = std::pair<int, int>;

public:

	bool solveWithAStar(const Key&, const Key&);
	bool solveWithIDAStar(const Key&, const Key&);

	std::string info() const;

private:

	struct Hash
	{
		auto operator()(const typename Node<Key, Value>::Ptr& nodePtr) const
		{
			return boost::hash_value(nodePtr->mKey);
		}
	};

	struct Equal
	{
		auto operator()(
			const typename Node<Key, Value>::Ptr& nodePtr1, 
			const typename Node<Key, Value>::Ptr& nodePtr2) const
		{
			return std::equal_to<Key>()(nodePtr1->mKey, nodePtr2->mKey);
		}
	};

	struct Greater
	{
		auto operator()(
			const typename Node<Key, Value>::Ptr& nodePtr1,
			const typename Node<Key, Value>::Ptr& nodePtr2) const
		{
			return std::greater<typename Value::first_type>()(
				nodePtr1->mValue.first + nodePtr1->mValue.second, 
				nodePtr2->mValue.first + nodePtr2->mValue.second);
		}
	};

	struct Less
	{
		auto operator()(
			const typename Node<Key, Value>::Ptr& nodePtr1,
			const typename Node<Key, Value>::Ptr& nodePtr2) const
		{
			return std::less<typename Value::first_type>()(
				nodePtr1->mValue.first + nodePtr1->mValue.second,
				nodePtr2->mValue.first + nodePtr2->mValue.second);
		}
	};

	struct Manhattan
	{
		auto operator()(const Key& key1, const Key& key2)
		{
			auto sum(0);

			for (auto i(0); i < N * N; ++i)
			{
				if (!key1[i]) continue;

				int x1(i % N);
				int y1(i / N);

				for (auto j(0); j < N * N; ++j)
				{
					if (key1[i] != key2[j]) continue;

					int x2(j % N);
					int y2(j / N);

					sum += abs(x2 - x1) + abs(y2 - y1);

					break;
				}
			}

			return sum;
		}
	};

	typename Node<Key, Value>::Ptr mRoot;
	typename Node<Key, Value>::Ptr mGoal;
};

template <int N, typename Key>
bool Puzzle<N, Key>::solveWithAStar(const Key& begin, const Key& end)
{
	// �q���[���X�e�B�b�N�֐��̓}���n�b�^��������p����
	auto heuristic(std::bind(Manhattan(), std::placeholders::_1, end));

	mRoot = Node<Key, Value>::create(begin, std::make_pair(0, heuristic(begin)));
	mGoal = Node<Key, Value>::create(end, std::make_pair(-1, heuristic(end)));

	// open���X�g�̃f�[�^�\���ɗD��x�t���L���[�Aclosed���X�g�̃f�[�^�\���Ƀn�b�V���Z�b�g��g��
	PriorityQueue<typename Node<Key, Value>::Ptr,
		std::deque<typename Node<Key, Value>::Ptr>, Greater> openList;
	std::unordered_set<typename Node<Key, Value>::Ptr, Hash, Equal> closedList;

	openList.push(mRoot);

	// open���X�g����ɂȂ�܂ŒT��
	while (!openList.empty())
	{
		// open���X�g���ŃR�X�g���ŏ��̂�̂�I��
		auto parent(openList.top());

		// open���X�g����폜���Aclosed���X�g�ɑ}��
		openList.pop();
		closedList.insert(parent);

		// �I��m�[�h���S�[���ł͂Ȃ��ꍇ
		if (parent->mKey != end)
		{
			std::pair<int, int> prev;

			// 0(��)���
			for (auto i(0); i < N * N; ++i)
			{
				if (parent->mKey[i]) continue;

				prev.first = i % N;
				prev.second = i / N;

				break;
			}

			// �ړ���̌��
			std::vector<std::pair<int, int>> nexts
			{
				{ prev.first + 1, prev.second },
				{ prev.first, prev.second + 1 },
				{ prev.first - 1, prev.second },
				{ prev.first, prev.second - 1 }
			};

			// �q�m�[�h�W�J
			for (const auto& next : nexts)
			{
				// �͈͊O�̓X�L�b�v
				if (next.first < 0 || next.first >= N ||
					next.second < 0 || next.second >= N) continue;

				// 0��ړ�������
				auto key(parent->mKey);
				std::swap(key[prev.first + prev.second * N], key[next.first + next.second * N]);

				// gValue�͈ړ��R�X�g1�𑫂�
				auto gValue(parent->mValue.first + 1);

				// hValue�͈ړ������}�X�̃}���n�b�^�������̂ݕω�����
				auto target(std::find(end.begin(), end.end(), key[prev.first + prev.second * N]));
				auto x(std::distance(end.begin(), target) % N);
				auto y(std::distance(end.begin(), target) / N);

				auto hValue(parent->mValue.second
					- abs(x - next.first) - abs(y - next.second)
					+ abs(x - prev.first) + abs(y - prev.second));

				// �q�m�[�h�쐬
				auto child(Node<Key, Value>::create(parent,
					std::move(key), std::make_pair(gValue, hValue)));

				// �q�m�[�h��closed���X�g�ɑ��݂��Ă��邩����
				auto it(closedList.find(child));

				// ���݂��Ȃ��ꍇ
				if (it == closedList.end())
				{
					// �q�m�[�h��open���X�g�ɑ��݂��Ă��邩����
					auto jt(find_if(openList.cbegin(), openList.cend(),
						std::bind(Equal(), std::placeholders::_1, child)));

					// ���݂��Ȃ��ꍇ
					if (jt == openList.cend())
					{
						// open���X�g�ɑ}��
						openList.push(child);
						parent->mChildren.push_back(child);
					}
					// ���݂���ꍇ
					else
					{
						// �R�X�g����菬������΃R�X�g��u��������
						if ((*jt)->mValue.first + (*jt)->mValue.second >
							child->mValue.first + child->mValue.second)
						{
							openList.erase(jt);
							openList.push(child);
							parent->mChildren.push_back(child);
						}
					}
				}
				// ���݂���ꍇ
				else
				{
					// �R�X�g����菬�������open���X�g�ɕ���
					if ((*it)->mValue.first + (*it)->mValue.second >
						child->mValue.first + child->mValue.second)
					{
						closedList.erase(it);
						openList.push(child);
						parent->mChildren.push_back(child);
					}
				}
			}
		}
		// �I��m�[�h���S�[���̏ꍇ
		else
		{
			mGoal = parent;
			return true;
		}
	}

	return false;
}

template <int N, typename Key>
bool Puzzle<N, Key>::solveWithIDAStar(const Key& begin, const Key& end)
{
	// �q���[���X�e�B�b�N�֐��̓}���n�b�^��������p����
	auto heuristic(std::bind(Manhattan(), std::placeholders::_1, end));

	auto beginZero(std::find(begin.begin(), begin.end(), 0));
	auto endZero(std::find(end.begin(), end.end(), 0));

	auto x1(std::distance(begin.begin(), beginZero) % N);
	auto y1(std::distance(begin.begin(), beginZero) / N);

	auto x2(std::distance(end.begin(), endZero) % N);
	auto y2(std::distance(end.begin(), endZero) / N);

	auto distance(abs(x2 - x1) + abs(y2 - y1));

	auto alignParity([](auto x, auto y)
	{
		if (x % 2 == y % 2) return x;
		else return x + 1;
	});

	// limit��2�Â���������(15�p�Y���̍Œ��萔��80��炵��)
	for (auto limit(alignParity(heuristic(begin), distance)); limit <= 60; limit += 2)
	{
		mRoot = Node<Key, Value>::create(begin, std::make_pair(0, heuristic(begin)));
		mGoal = Node<Key, Value>::create(end, std::make_pair(-1, heuristic(end)));

		// open���X�g�̃f�[�^�\���ɃX�^�b�N�Aclosed���X�g�̃f�[�^�\���Ƀn�b�V���Z�b�g��g��
		std::stack<typename Node<Key, Value>::Ptr, std::deque<typename Node<Key, Value>::Ptr>> openList;
		std::unordered_set<typename Node<Key, Value>::Ptr, Hash, Equal> closedList;

		openList.push(mRoot);

		// open���X�g����ɂȂ�܂ŒT��
		while (!openList.empty())
		{
			// open���X�g���ŃR�X�g���ŏ��̂�̂�I��
			auto parent(openList.top());

			// open���X�g����폜���Aclosed���X�g�ɑ}��
			openList.pop();
			closedList.insert(parent);

			// �I��m�[�h���S�[���ł͂Ȃ��ꍇ
			if (parent->mKey != end)
			{
				std::pair<int, int> prev;

				// 0(��)���
				for (auto i(0); i < N * N; ++i)
				{
					if (parent->mKey[i]) continue;

					prev.first = i % N;
					prev.second = i / N;

					break;
				}

				// �ړ���̌��
				std::vector<std::pair<int, int>> nexts
				{
					{ prev.first + 1, prev.second },
					{ prev.first, prev.second + 1 },
					{ prev.first - 1, prev.second },
					{ prev.first, prev.second - 1 }
				};

				// �R�X�g�̑傫���m�[�h����X�^�b�N�ɐςނ��߂̈ꎞ�̈�
				std::vector<typename Node<Key, Value>::Ptr> children;

				// �q�m�[�h�W�J
				for (const auto& next : nexts)
				{
					// �͈͊O�̓X�L�b�v
					if (next.first < 0 || next.first >= N ||
						next.second < 0 || next.second >= N) continue;

					// 0��ړ�������
					auto key(parent->mKey);
					std::swap(key[prev.first + prev.second * N], key[next.first + next.second * N]);

					// gValue�͈ړ��R�X�g1�𑫂�
					auto gValue(parent->mValue.first + 1);

					// hValue�͈ړ������}�X�̃}���n�b�^�������̂ݕω�����
					auto target(std::find(end.begin(), end.end(), key[prev.first + prev.second * N]));
					auto x(std::distance(end.begin(), target) % N);
					auto y(std::distance(end.begin(), target) / N);

					auto hValue(parent->mValue.second
						- abs(x - next.first) - abs(y - next.second)
						+ abs(x - prev.first) + abs(y - prev.second));

					// �R�X�g�����~�b�g�𒴂�����̂̓X�L�b�v
					if (gValue + hValue > limit) continue;

					// �q�m�[�h�쐬
					auto child(Node<Key, Value>::create(parent,
						std::move(key), std::make_pair(gValue, hValue)));

					// �q�m�[�h��closed���X�g�ɑ��݂��Ă��邩����
					auto it(closedList.find(child));

					// ���݂��Ȃ��ꍇ
					if (it == closedList.end())
					{
						// temp���X�g�ɑ}��
						children.push_back(child);
					}
				}

				// �R�X�g�̍~���Ƀ\�[�g
				std::sort(children.begin(), children.end(), Greater());

				for(const auto& child : children)
				{
					// open���X�g�ɑ}��
					openList.push(child);
					parent->mChildren.push_back(child);
				}
			}
			// �I��m�[�h���S�[���̏ꍇ
			else
			{
				mGoal = parent;
				return true;
			}
		}
	}

	return false;
}

template <int N, typename Key>
std::string Puzzle<N, Key>::info() const
{
	auto print([](auto& stream, const auto& key)
	{
		auto numDigits([](auto n)
		{
			return n ? static_cast<int>(log10(n)) + 1 : 1;
		});

		for (auto i(0); i < N * N; ++i)
		{
			for (auto j(0); j < numDigits(N * N - 1) - numDigits(key[i]); ++j)
			{
				stream << " ";
			}

			stream << key[i] << " ";

			if (i % N == N - 1) stream << std::endl;
		}
	});

	std::stringstream sstream;

	sstream << std::endl;
	sstream << "begin:" << std::endl;

	sstream << std::endl;
	print(sstream, mRoot->mKey);

	sstream << std::endl;
	sstream << "end:" << std::endl;

	sstream << std::endl;
	print(sstream, mGoal->mKey);

	if (mGoal->mParent.lock())
	{
		std::vector<std::reference_wrapper<Key>> solution;

		// �e��ɂ��ǂ��ăX�^�b�N�ɐς�
		auto nodePtr(mGoal);
		while (nodePtr)
		{
			solution.push_back(nodePtr->mKey);
			nodePtr = nodePtr->mParent.lock();
		}

		sstream << std::endl;
		sstream << "solution:" << std::endl;

		// �X�^�b�N���珇�Ɏ��o���Ă����΂��ꂪ��@�ł���
		while (!solution.empty())
		{
			sstream << std::endl;
			print(sstream, solution.back().get());

			solution.pop_back();
		}
	}
	else
	{
		sstream << std::endl;
		sstream << "solution not found" << std::endl;
	}

	return sstream.str();
}