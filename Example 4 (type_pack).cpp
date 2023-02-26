#include <iostream>
#include <limits>
#include <algorithm>
#include <tuple>
#include <type_traits>


template <class... Ts>
struct type_pack{};

using empty_pack = type_pack<>;

template <class T>
struct just_type { using type = T; };

template <class... Ts, class... Us>
constexpr bool operator==(type_pack<Ts...>, type_pack<Us...>) { return false; }
template <class... Ts>
constexpr bool operator==(type_pack<Ts...>, type_pack<Ts...>) { return true; }

template <class T, class... Ts>
constexpr type_pack<Ts..., T> push_back(type_pack<Ts...>) { return {}; }

template <class... Ts, class T>
constexpr type_pack<Ts..., T> push_back(type_pack<Ts...>, just_type<T>) { return {}; };

template <class T,class... Ts>
constexpr type_pack<T, Ts...> push_front(type_pack<Ts...>) { return {}; };

template <class... Ts, class T>
constexpr type_pack<T, Ts...> push_front(type_pack<Ts...>, just_type<T>) { return {}; };

template <class T,class... Ts>
constexpr type_pack<Ts...> pop_front(type_pack<T, Ts...>) { return {}; };

template <class... Ts, class T>
constexpr type_pack<Ts...> pop_back(type_pack<Ts...,T>) { return {}; };

static_assert(push_back<int>(type_pack<double, char>{}) == type_pack<double, char, int>{});
static_assert(push_front(type_pack<double, char>{},just_type<int>{}) == type_pack<int, double, char>{});
static_assert(pop_front(type_pack<int, double, char>{}) == type_pack<double, char>{});



template <class T, class... Ts> 
constexpr bool contains(type_pack<Ts...>,just_type<T>) {
	return (... || std::is_same_v<T, Ts>);
}

static_assert(contains(type_pack<int, bool, float>{}, just_type<int>{} ));

template <class T, class... Ts>
constexpr int find(type_pack<Ts...>, just_type<T>) {
	bool bs[] = { std::is_same<T,Ts>::value... };
	int size_bs = sizeof...(Ts);
	for (int i = 0; i < size_bs; i++)
		if (bs[i])
			return i;
	return size_bs;
}

static_assert( find(type_pack<char, bool, float>{},just_type<int>{}) == 3);

template <template <class...> class F, class... Ts>
constexpr auto transform(type_pack<Ts...>) {
	return type_pack<typename F<Ts>::type...>{};
}

template <int First, int Last>
struct static_for {
	template <typename Lambda>
	static inline constexpr void apply(Lambda const& f) {
		if (First < Last) {
			f(std::integral_constant<int, First>{});
			static_for<First + 1, Last>::apply(f);
		}
	}
};
template <int N>
struct static_for<N, N> {
	template <typename Lambda>
	static inline constexpr void apply(Lambda const& f) {}
};


template <template <class...> class F, class T>
constexpr auto filter_one() {
	if constexpr (F<T>::value)
		return type_pack<T>{};
	else
		return empty_pack{};
}

template <class... Ts, class... Us>
constexpr auto operator+(type_pack<Ts...>, type_pack<Us...>) {
	return type_pack<Ts..., Us...>{};
}

template <template <class...> class F, class... Ts>
constexpr auto filter(type_pack<Ts...>) {
	return (empty_pack{} + ... + filter_one<F, Ts>());
}

template<class T,class U>
struct isSame {
	static constexpr bool value = false;
};
template<class T>
struct isSame <T,T> {
	static constexpr bool value = true;
};

template<class T>
struct isInt {
	static constexpr bool value = isSame<T, int>::value;
};


static_assert(filter<isInt>(type_pack<char, int, int*>{}) == type_pack<int>{});
static_assert(transform<std::add_pointer>(type_pack<int, double, char>{}) == type_pack<int*, double*, char*>{});

int main() {
	 
	//auto vec = push_back(type_pack<double, char>{}, just_type<int>{});
	//auto vec2 = push_back<int>(type_pack<double, char>{});
	//contains
}