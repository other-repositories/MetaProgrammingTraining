#include <string>
#include <iostream>

//CRTP
template <class Derived>
struct BasicStrategy {
	void foo() {
		static_cast<const Derived*>(this)->foo();
	}
};

struct StrategyA : public BasicStrategy<StrategyA> {
	void foo() {
		std::cout << "test1";
	}
};

struct StrategyB : public BasicStrategy<StrategyB> {
	void foo() {
		std::cout << "test2";
	}
};

template <class Strat, typename = std::enable_if_t<std::is_base_of_v<BasicStrategy<Strat>,Strat> > >
class A {
public:
	void foo() {
		Strat _strat;
		_strat.foo();
	}
private:

};


void main() {
	A<StrategyA> out1;
	A<StrategyB> out2;
	out1.foo();
	out2.foo();
}