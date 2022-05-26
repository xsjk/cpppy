
#include <iostream>
#include <type_traits>
 
/**********************************************************/

#include "list.hpp"

/**********************************************************/

using namespace python;

int main() {

    list l={1,2,3.3,"1234",'H','e','l','l','o'};
    std::cout << len(l) << std::endl;
    
    
    std::cout << l << std::endl;

    auto l2 = list();
    l2.extend(2,4.44,4,5,'v').append("Helloworld");
    
    std::cout << l2(5,2,-1) << std::endl;
    std::cout << l2.pop() << std::endl;
    std::cout << l2 << std::endl;

    l2.insert(5,'2');
    std::cout << l2 << std::endl;


    return 0;
}
