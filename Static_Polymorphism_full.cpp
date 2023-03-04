#include <string>
#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
#include <variant>
#include <any>
#include <chrono>


////////////////////////
// STATIC POLYMORFIZM
////////////////////////

template<class Derived>
class Base {
public:
    int get() const {
        return static_cast<const Derived*>(this)->get();
    }
};

class Float : public Base<Float> {
public:
    Float(float value) :v(value) {}
    int get() const { return static_cast<int>(v); }
private:
    float v;
};

class Int : public Base<Int> {
public:
    Int(int value) : v(value) {}
    int get() const { return v; }
private:
    int v;
};

class String : public Base<String> {
public:
    String(const std::string& str) : v(str) {};
    int get() const { return std::atoi(v.c_str()); }
private:
    std::string v;
};

/////generic code
using BaseStatic = std::variant<std::shared_ptr<Base<Float>>, std::shared_ptr<Base<Int>>, std::shared_ptr<Base<String>>>;

template<class... Args >
constexpr int static_get(const std::variant<Args...> & t) {
    int out;
    std::visit([&](auto & arg) {
        using Type = std::decay_t<decltype(arg)>;
        constexpr bool ok[] = { std::is_same< Args, Type >::value...};
        for(int i=0;i<sizeof...(Args);i++)
            if (ok[i])
                out = arg->get();
    }, t);
    return out;
}
/////////////////////

void main() {
    std::vector<BaseStatic> v;
    v.push_back(std::make_shared<Float>(5));
    v.push_back(std::make_shared<Int>(6));
    v.push_back(std::make_shared<String>("10"));

    int sum = 0;
    for (auto& elem : v)
        sum += static_get(elem);
     
    std::cout << sum;
}