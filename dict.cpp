
#include "dict.hpp"
#include <ranges>

using namespace python;
int main() {
    dict d = {{"a", 1}, {"b", 2}};

    d["a"] = 10;
    d["c"] = 3.3;


    std::cout << d;

}