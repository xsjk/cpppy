#include <iostream>
#include <typeinfo>
#include <cstring>

#define PRINT_TYPE(x) std::cout << #x << std::string(20-strlen(#x),' ') << typeid(x).name() << '\n';

const char * types[] = {
    "signed char",
    "unsigned char",
    "char",
    "short",
    "unsigned short",
    "int",
    "unsigned int",
    "long",
    "unsigned long",
    "long long",
    "unsigned long long",
    "float",
    "double",
    "long double",
    "bool",
};

int main() {   

    PRINT_TYPE(signed char);
    PRINT_TYPE(bool);
    PRINT_TYPE(char);
    PRINT_TYPE(double);
    PRINT_TYPE(long double);
    PRINT_TYPE(float);
    PRINT_TYPE(unsigned char);
    PRINT_TYPE(int);
    PRINT_TYPE(unsigned int);
    PRINT_TYPE(long);
    PRINT_TYPE(unsigned long);
    PRINT_TYPE(short);
    PRINT_TYPE(unsigned short);
    PRINT_TYPE(long long);
    PRINT_TYPE(unsigned long long);

    
    //...
}