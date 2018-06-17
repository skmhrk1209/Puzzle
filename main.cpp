#include "puzzle.hpp"
#include <random>
#include <numeric>
#include <chrono>
#include <thread>
#include <mutex>
#include <iostream>

int main()
{
	constexpr int N(3);
	std::array<int, N * N> begin;
	std::array<int, N * N> end;

	std::iota(begin.begin(), begin.end(), 0);
	std::iota(end.begin(), end.end(), 0);

	std::random_device seed;
	std::mt19937 engine(seed());

	std::shuffle(begin.begin(), begin.end(), engine);
	std::shuffle(end.begin(), end.end(), engine);

	Puzzle<N, decltype(begin)> puzzle1;
	Puzzle<N, decltype(begin)> puzzle2;

	std::vector<std::thread> threads;

	threads.emplace_back(&decltype(puzzle1)::solveWithAStar, std::ref(puzzle1), std::cref(begin), std::cref(end));
	threads.emplace_back(&decltype(puzzle2)::solveWithIDAStar, std::ref(puzzle2), std::cref(begin), std::cref(end));

	for (auto& thread : threads) thread.join();

	std::cout << "solve with A*" << std::endl;
	std::cout << puzzle1.info() << std::endl;

	std::cout << std::endl;

	std::cout << "solve with IDA*" << std::endl;
	std::cout << puzzle2.info() << std::endl;
}
