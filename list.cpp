
#include <iostream>
#include <type_traits>
 
/**********************************************************/

#include "list.hpp"
#include "iter.hpp"

/**********************************************************/

using namespace python;

int main() {

    list l={1,2,3.3,"1234",'H','e','l','l','o'};
    std::cout << len(l) << std::endl;
    
    
    std::cout << l << std::endl;

    auto l2 = list();
    std::cout << l2.extend({2,4.44,4,5,'v'}).append("Helloworld")(5,2,-1) << std::endl;
    l2.pop();
    l2.del(-1);
    l2[1] = 5.5;
    l2.insert(5,2.5);
    std::cout << l2 << std::endl;
    l2.for_each([](var &a){a=double(a);});
    std::cout << l2.argmax() << std::endl;
    l2.sort();
    std::cout << l2 << std::endl;

    l2[0] = list(range(5));
    std::cout << l2 << std::endl;



    return 0;
}
