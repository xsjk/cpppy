#include "iter.hpp"
#include <iostream>

#include <vector>
#include <map>
using namespace python;

// template<typename T>
// zip enumerate(T&& it){
//     return zip(itertools::count(), std::forward<T>(it));
// }

// zip f(_iter&& i1, _iter&& i2){
//     return zip(std::move(i1), std::move(i2));
// }

int main(){


    for(auto i:range(5,0,-1))
        std::cout<<i<<'\t';
    std::cout<<std::endl;

    // for(auto i:enumerate(range(5,0,-1)))
    //     std::cout<<i<<'\t';
    // std::cout<<std::endl;

    // for(auto i:zip(zip(range(5),range(5,0,-1)),range(5),map([](var a){return a*a;},range(-5,0))))
    //     std::cout<<i<<'\t';
    // std::cout<<std::endl;
    

    // for(auto i:filter([](var i){return int(i)%2==0;},range(5)))
    //     std::cout<<i<<'\t';
    // std::cout<<std::endl;

    // for(auto i:itertools::filterfalse([](var i){return int(i)%2==0;},range(5)))
    //     std::cout<<i<<'\t';
    // std::cout<<std::endl;


    // for(auto i:(itertools::chain(range(5),range(5,0,-1)))){
    //     std::cout<<i<<'\t';
    // }

    // for (auto i:iter({1,2,3,4}))
    //     std::cout << i << '\t';
    // std::cout << std::endl;

    // for(auto i:itertools::compress(range(5),{0,1,1}))
    //     std::cout<<i<<'\t';

    // for(auto i:itertools::dropwhile([](var i){return int(i)<3;},range(5)))
    //     std::cout<<i<<'\t';


    return 0;

}
