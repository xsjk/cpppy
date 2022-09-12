#ifndef PYTHON_VAR_HPP
#define PYTHON_VAR_HPP
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include <iostream>
#include <typeinfo>
#include <any>
#include <cxxabi.h>
#include <initializer_list>

#define REG_TYPE(type) \
    if (typeid(T) == type()) { \
        return std::any_cast<T>(*this); \
    }\


namespace python {

class var;

std::string type(const var& v);
std::string type(const std::type_info& t) {
    return abi::__cxa_demangle(t.name(), 0, 0, 0);
}
template<typename T>
std::string type(T) {
    return abi::__cxa_demangle(typeid(T).name(), 0, 0, 0);
}

const char * basic_types[] = {
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

class any {
    const char* m_type;
    
    

};


class var : public  std::any {
public:
    var() = default;

    template <typename T>
    var(T value) : std::any(std::any_cast<T>(value)) {}

    bool operator==(const var& other) const {
        return type() == other.type();
    }

    

    
    template <typename T>
    operator T() const {
        if (typeid(T) == type()) {
            return std::any_cast<T>(*this);
        } else {
            // throw std::runtime_error("Type mismatch");
            return T();
        }
    }

    std::string __str__() {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    std::string __repr__() {
        if (type()==typeid(std::string) || type()==typeid(char*) || type()==typeid(const char*))
            return '\"' + __str__() + '\"';
        else if (type()==typeid(char))
            return '\'' + __str__() + '\'';
        else
            return __str__();
    }
    
    friend std::ostream& operator<<(std::ostream& os, const var& v) {
        #define ELIF(TYPE) else if (v.type()==typeid(TYPE)) { os << std::any_cast<TYPE>(v); }
        if (!v.has_value()) { os << "None"; }
        else if (v.type()==typeid(bool)) { os << std::any_cast<bool>(v) ? "True" : "False"; } 
        ELIF(std::string)   ELIF(char*)         ELIF(const char*)
        ELIF(char)          ELIF(signed char)   ELIF(unsigned char)
        ELIF(short)         ELIF(unsigned short)
        ELIF(int)           ELIF(unsigned int)
        ELIF(long)          ELIF(unsigned long)
        ELIF(long long)     ELIF(unsigned long long)
        ELIF(float)         ELIF(double)        ELIF(long double)
        else {
            auto t = python::type(v);
            auto i = t.find('<');
            if (i!=std::string::npos)
                t = t.substr(0,i);
            os << '(' << t << ')';
        }
        return os;
    }
};


// template <typename T>
// var::operator T() const {
//     auto t = type();
//         // cast by type_info
//         if (typeid(T) == type()) {
//             return std::any_cast<T>(*this);
//         } else {
//             std::cout << "\033[1;31m" << "Type mismatch:\n" 
//             << "Expect\t" << python::type(typeid(T)) << std::endl
//             << "Got\t"    << python::type(type()) << "\033[0m\n";
//             return 0;
//         }
// }



std::string type(const var& v) {    
    return abi::__cxa_demangle(v.type().name(), NULL, NULL, NULL);
}

template<typename T>
size_t len(const T& t) {
    return t.__len__();
}

// class Property{
// public:
//     Property(const std::string& name) : m_name(name) {}
//     virtual ~Property() {}
// private:
//     std::string m_name;
// };

// template< typename T >
// class TypedProperty : public Property{
// public:
//     TypedProperty (const std::string& name, const T& data) : Property(name), m_data(data);
// private:
//     T m_data;
// };

// typedef std::vector< std::shared_ptr<Property> > property_list_type;
}
#endif