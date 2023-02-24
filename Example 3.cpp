#include <iostream>
#include <limits>
#include <type_traits>
#include <string>


struct ToolTypeA {
    int t;
};

struct ToolTypeB {
    std::string b;
    std::string g;
};

struct ToolTypeC {
    float b;
    double g;
};

struct FabricStatic {
    template<class T>
    operator T() const {
        if constexpr (std::is_same_v<T,ToolTypeA>)
            return ToolTypeA{5};
        else if constexpr (std::is_same_v<T,ToolTypeB>)
            return ToolTypeB{"24","124"};
        else if constexpr (std::is_same_v<T,ToolTypeC>)
            return ToolTypeC{5.7,3.4};
    };
};

auto getToolStatic() {
    return FabricStatic{};
}

int main() {
    ToolTypeB b = getToolStatic(); // "1"
}