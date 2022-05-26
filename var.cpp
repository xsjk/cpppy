#include "var.hpp"
#include <iostream>
#include <vector>
using namespace python;

int f(int){
    return 0;
}

class C {
public:
    int v;
    int f(int){
        return 0;
    }
    int operator[](int i){
        return v;
    }
};

int main(){
    var b=2;
    var c=3.1;
    var d=c;
    c = 3.2;
    c = 3.1;
    // d.get<double>()=3.2;
    // var e = new int{10};

    int i = 100;

    b = C{10};

    int cc = b[0];

    c = std::string("world");
    d = std::string("hello");
    // d.get<std::string>()[0] = 'H';
    // std::cout << (void*)&(d.get<std::string>()[0]) << std::endl;
    // std::cout << (void*)&((*(std::string*)(&d))[0]) << std::endl;
    // std::cout << &d[0] << std::endl;
    std::cout << d[0].is_ref() << std::endl;
    std::cout << d << std::endl;

    // c = i;
    d = 1;
    (d += 1) +=1;

    c[0] = 'W';
    std::cout << c << std::endl;
    std::cout << ++d << std::endl;
    std::cout << var() << std::endl;
    if (b==d){
        std::cout << "b==d" << std::endl;
    } else {
        std::cout << "b!=d" << std::endl;
    }


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