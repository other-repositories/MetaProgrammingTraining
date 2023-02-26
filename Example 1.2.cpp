#include <iostream>
#include <limits>
#include <algorithm>
#include <tuple>
#include <type_traits>
#include <utility>
#include <chrono>

template<class T>
int getValue(const T & val) {
	int sum = 0;
	if constexpr (std::is_floating_point_v<T>) {
		sum = val;
	}
	if constexpr (std::is_integral_v<T>) {
		sum = (int)val;
	}
	if constexpr (std::is_same_v<T,std::string>) {
		sum = (int)std::atoi(val.c_str());
	}
	return sum;
}

int main() {
	std::tuple tuple(1, 2.0, std::string("3"));
	int sum = 0;
	
	auto start = std::chrono::high_resolution_clock::now();
	for(int i=0;i<1'000'000;++i)
		sum += std::apply([](const auto &... v) { return (getValue(v) + ...); }, tuple);

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cout << "Execution time: " << duration << "ms" << std::endl;
	std::cout << sum;
}