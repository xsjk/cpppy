#include "var.hpp"
#include <iostream>
#include <vector>
using namespace python;

int f(int){
    return 0;
}

class C {
public:
    C(int i) :v(i)  {}
    int v;
    int f(int){
        return 0;
    }

};

int main(){
    var b=2.1;
    var c=3.1;
    var d=c;
    c = 3.1;
    // d.get<double>()=3.2;
    // var e = new int{10};

    d = 1;
    (d += 1) +=1;

    const auto ss = std::string("hello");
    c = ss;
    c[0] = 'W';
    std::cout << c << std::endl;
    std::cout << ss << std::endl;
    std::cout << ++d << std::endl;
    b = std::vector<int>{1,2,3,4,5};
    for(auto i:b)
        std::cout << i << '\t';
    // if (b<d){
    //     std::cout << b << " < " << d << std::endl;
    // } else {
    //     std::cout << b << " >= " << d << std::endl;
    // }


    // std::cout << (std::string("hello")<< std::string("world")) << std::endl;

    // for(int&i:d){
    //     std::cout << i << ' ';
    // }



    // delete (int*)e;

    // any a=1;
    // any b=2;

    // a = b;

    // any c = a;

    // a = std::string("Hello");
    // b = 3.14;

    // (std::string(a))[0] = 'h';


    // std::cout << std::string(a) << ' ' << a.data() << std::endl;
    // std::cout << double(b) << ' ' << b.data() << std::endl;
    // std::cout << int(c) << ' ' << c.data() << std::endl;
    // std::cout << sizeof(c) << ' ' << c.data() << std::endl;
    // std::cout << sizeof(std::any) << ' ' << c.data() << std::endl;


    

    return 0;
}