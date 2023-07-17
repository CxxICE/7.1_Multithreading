#include <windows.h>

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <random>
#include <execution>
#include <algorithm>

using namespace std::chrono_literals;
using duration_ms= std::chrono::duration<double, std::milli>;

void fill_vector(int size, std::vector<int> &vec)
{
	std::mt19937 gen;
	std::uniform_int_distribution<int> dist(0, 100);	
	std::generate(std::execution::par, vec.begin(), vec.end(), [&]() {
		return dist(gen);
		});
}

struct Result
{
	int threads_count = 0;
	duration_ms time_ms[4];
};

duration_ms multi_sum(int threads_count, std::vector<int> &vec1, std::vector<int> &vec2)
{
	std::vector<int> res(vec1.size());
	std::vector<std::thread> VT(threads_count);
	size_t j_begin = 0;
	size_t step = vec1.size() / threads_count;
	size_t j_end = step - 1;

	auto start = std::chrono::high_resolution_clock::now();

	for (size_t i = 0; i < threads_count; ++i)
	{		
		VT[i] = std::thread([&vec1, &vec2, &res, j_begin, j_end]()
			{
				for (size_t j = j_begin; j <= j_end; ++j)
				{
					res[j] = vec1[j] + vec2[j];
				}
			});
		j_begin = j_end + 1;
		if (i == threads_count)
		{
			j_end = vec1.size() - 1;
		}
		else
		{
			j_end += step;
		}
	}
	for (auto &vt : VT)
	{
		if (vt.joinable()) { vt.join(); }
	}
	
	auto end = std::chrono::high_resolution_clock::now();
	duration_ms delta = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
	return delta;
}

int main()
{
	setlocale(LC_ALL, "RU");
	Result res[4];

	//Вывод кол-ва ядер
	std::cout << "Количество аппаратных ядер: " << std::thread::hardware_concurrency() << std::endl;

	const int size1 = 1000;
	const int size2 = 10'000;
	const int size3 = 100'000;
	const int size4 = 1'000'000;

	std::vector<int> v11(size1);
	std::vector<int> v12(size1);
	std::vector<int> v21(size2);
	std::vector<int> v22(size2);
	std::vector<int> v31(size3);
	std::vector<int> v32(size3);
	std::vector<int> v41(size4);
	std::vector<int> v42(size4);

	fill_vector(size1, v11);
	fill_vector(size1, v12);
	fill_vector(size2, v21);
	fill_vector(size2, v22);
	fill_vector(size3, v31);
	fill_vector(size3, v32);
	fill_vector(size4, v41);
	fill_vector(size4, v42);

	std::cout << std::format("|{:^15}|{:^22}|{:^22}|{:^22}|{:^22}|\n", "Потоков", "1000", "10'000", "100'000", "1'000'000");
	for (size_t i = 0; i < 4; ++i)
	{
		res[i].threads_count = std::pow(2, i + 1);
		res[i].time_ms[0] = multi_sum(res[i].threads_count, v11, v12);
		res[i].time_ms[1] = multi_sum(res[i].threads_count, v21, v22);
		res[i].time_ms[2] = multi_sum(res[i].threads_count, v31, v32);
		res[i].time_ms[3] = multi_sum(res[i].threads_count, v41, v42);
		
		std::cout << std::format("|{0:^15}|{1:^22}|{2:^22}|{3:^22}|{4:^22}|\n", res[i].threads_count,
			res[i].time_ms[0], res[i].time_ms[1], res[i].time_ms[2], res[i].time_ms[3]);
	}

	return 0;
}

