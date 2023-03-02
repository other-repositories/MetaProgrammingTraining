#include <string>
#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
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
    int get() const { return static_cast<int>(v); }
float v = 3;
};

class Int : public Base<Int> {
public:
    int get() const { return v; }
int v = 2;
};

class String : public Base<String> {
public:
    int get() const{ return std::atoi(str); }
    char str[2] = "1";
};

template <class... T>
int classGetSum(const T &... t) {
    return (... + t->get());
}

////////////////////////
// DYNAMIC POLYMORFIZM
////////////////////////


class Base2 {
public:
    virtual int get() = 0;
};

class Float2 : public Base2 {
public:
    int get() { return static_cast<int>(v); }
    float v = 3;
};

class Int2 : public Base2 {
public:
    int get() { return v; }
    int v = 2;
};

class String2 : public Base2 {
public:
    int get() { return std::atoi(str.c_str()); }
    std::string str = "1";
};

int classGetSum2(Base2 *bs1, Base2* bs2, Base2* bs3) {
    return bs1->get() + bs2->get() + bs3->get();
}

////////////////////////
// MIDDLE POLYMORPHISM (STATIC-DYNAMIC)
////////////////////////

struct Int3 {
    int value = 1;
};

struct Float3 {
    float value = 2.0;
};

struct String3 {
    char value[2] = "3";
};

struct ShapeConcept {
    virtual ~ShapeConcept() = default;
    virtual int get() const = 0;
    // ...
};

template<typename T>
struct ShapeModel : public ShapeConcept {
    T object;
    explicit ShapeModel(T&& shape) : object(std::move(shape)) {}
    explicit ShapeModel(const T& shape) : object(shape) {}
    int get() const override {
        return ::get(object); // Что за функция draw? См. после кода.
    }
};

int get(const Int3& s) {
    return s.value;
}

int get(const Float3& s) {
    return static_cast<int>(s.value);
}

int get(const String3 & s) {
    return std::atoi(s.value);
}


int classGetSum3(ShapeConcept *elem1, ShapeConcept*elem2, ShapeConcept *elem3) {

    return elem1->get() + elem2->get() + elem3->get();
}

//////////////////////
/// 
///////////////////////

std::chrono::steady_clock::time_point start;

void start_timer() {
    start = std::chrono::high_resolution_clock::now();
}

void end_timer() {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Execution time: " << duration << "ms" << std::endl;
}

void static_pol() {
    Base<Float>* a = new Float();
    Base<Int>* b = new Int();
    Base<String>* c = new String();
    int sum = 0;

    start_timer();

    for (int i = 0; i < 10'000'000; i++) {
        sum += classGetSum(a, b, c);
        sum += classGetSum(b, c, a);
        sum += classGetSum(b, b, b);
    }

    end_timer();
    delete a, b, c;
    std::cout << sum << " STATIC TESTS\n";
}

void mix_pol() {
    std::vector<ShapeConcept*> v;
    v.emplace_back(new ShapeModel<Int3>(Int3{  }));
    v.emplace_back(new ShapeModel<Float3>(Float3{  }));
    v.emplace_back(new ShapeModel<String3>(String3{ }));
    int sum = 0;

    start_timer();

    for (int i = 0; i < 10'000'000; i++) {

        sum += classGetSum3(v[0], v[1], v[2]);
        sum += classGetSum3(v[1], v[2], v[0]);
        sum += classGetSum3(v[1], v[1], v[1]);

    }

    end_timer();

    for (auto* elem : v)
        delete elem;
    std::cout << sum << " MIX TESTS\n";
}

void dynamic_pol() {
    auto* a = new Float2();
    auto* b = new Int2();
    auto* c = new String2();
    int sum = 0;

    start_timer();

    for (int i = 0; i < 10'000'000; i++) {
        sum += classGetSum2(a, b, c);
        sum += classGetSum2(b, c, a);
        sum += classGetSum2(b, b, b);
    }

    end_timer();
    delete a, b, c;
    std::cout << sum << " DYNAMIC TESTS\n";
}


int main() {
    static_pol();
    mix_pol();
    dynamic_pol();
}