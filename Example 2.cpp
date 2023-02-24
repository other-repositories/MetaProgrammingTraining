#include <iostream>
#include <limits>
#include <type_traits>
#include <string>

struct Impl {
    int value;
    template<class R>
    operator R() const {
        if constexpr (std::is_floating_point_v<R>)
            return (float)value;
        else if constexpr (std::is_same_v<R, std::string>)
            return std::to_string(value);
        else if constexpr (std::is_integral_v<R>)
            return value;
    };
};

auto func(int value) {
    return Impl{ value };
}

int main() {
    std::string s = func(1); // "1"
    float f = func(2);       // 2.0
    int i = func(3);         // 3
    bool g = func(3);
    std::cout << s;
}