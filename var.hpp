#ifndef PYTHON_VAR_HPP
#define PYTHON_VAR_HPP
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include <iostream>
#include <typeinfo>
// #include <var>
// #include "my_any.hpp"
#include <cxxabi.h>
#include <initializer_list>
#include <variant>
#include <cstring>
#define REG_TYPE(type)              \
  if (typeid(T) == type())          \
  {                                 \
    return std::any_cast<T>(*this); \
  }

namespace python
{
  std::string type(const std::type_info &t) {
    return abi::__cxa_demangle(t.name(), 0, 0, 0);
  }
  template <typename T> std::string type(T) {
    return abi::__cxa_demangle(typeid(T).name(), 0, 0, 0);
  }

  class bad_cast : public std::bad_cast {
    std::string error;
  public:
    bad_cast(std::string t) : error(t) {}
    bad_cast(const std::type_info &t1, const std::type_info &t2) : bad_cast("\033[1;31mTypeError\033[0m:  implicit conversion from " + type(t1) + " to " + type(t2)) {}
    const char *what() const noexcept override { return error.c_str(); }
  };



  class var {

    using Storage = void*;
    using Type = const std::type_info;
    using Size = unsigned long;

    struct handle_base {
      bool is_ref;
      handle_base(bool is_ref) : is_ref(is_ref) {}

      virtual Storage copy(Storage) = 0;
      virtual Type* type() = 0;
      virtual Size size() = 0;
      virtual void destroy(Storage) = 0;

      virtual bool __bool__(Storage) = 0;
      virtual char __char__(Storage) = 0;
      virtual int __int__(Storage) = 0;
      virtual double __double__(Storage) = 0;     
      virtual std::string __str__(Storage) = 0;
      virtual std::string __repr__(Storage) = 0;

      virtual var __neg__(Storage) = 0;
      virtual var __pos__(Storage) = 0;  
      virtual var __dref__(Storage) = 0;
      virtual var __rinc__(Storage) = 0;
      virtual var __rdec__(Storage) = 0;
      virtual var __linc__(Storage) = 0;
      virtual var __ldec__(Storage) = 0;

      virtual var __getitem__(Storage, var) = 0;

      virtual var __eq__(Storage, Storage) = 0;
      virtual var __ne__(Storage, Storage) = 0;
      virtual var __lt__(Storage, Storage) = 0;
      virtual var __gt__(Storage, Storage) = 0;
      virtual var __le__(Storage, Storage) = 0;
      virtual var __ge__(Storage, Storage) = 0;
      virtual var __add__(Storage, Storage) = 0;
      virtual var __sub__(Storage, Storage) = 0;
      virtual var __mul__(Storage, Storage) = 0;
      virtual var __div__(Storage, Storage) = 0;
      virtual var __mod__(Storage, Storage) = 0;
      virtual var __and__(Storage, Storage) = 0;
      virtual var __or__(Storage, Storage) = 0;
      virtual var __xor__(Storage, Storage) = 0;
      virtual var __lshift__(Storage, Storage) = 0;
      virtual var __rshift__(Storage, Storage) = 0;

      virtual var __iadd__(Storage, Storage) = 0;
      virtual var __isub__(Storage, Storage) = 0;
      virtual var __imul__(Storage, Storage) = 0;
      virtual var __idiv__(Storage, Storage) = 0;
      virtual var __imod__(Storage, Storage) = 0;
      virtual var __iand__(Storage, Storage) = 0;
      virtual var __ior__(Storage, Storage) = 0;
      virtual var __ixor__(Storage, Storage) = 0;
      virtual var __ilshift__(Storage, Storage) = 0;
      virtual var __irshift__(Storage, Storage) = 0;


      virtual var __begin__(Storage) = 0;
      virtual var __end__(Storage) = 0;
    };


    #define DEF_TRY(TYPE,NAME,OP,TRY,RET,...) \
      template<typename S,typename=decltype(TRY)> \
      static TYPE __##NAME##__(__VA_ARGS__) { \
        return RET; \
      } \
      template<typename S> \
      static TYPE __##NAME##__(...) {  \
        throw std::runtime_error("\033[1;31mTypeError\033[0m: type " + std::string(abi::__cxa_demangle(typeid(S).name(), 0, 0, 0)) + " has no operator " #OP); \
      }

    struct handle_helper {

      #define DEF_rUNARY(NAME,OP) DEF_TRY(var,NAME,OP,std::declval<S&>() OP,(*(S*)o) OP,Storage o)
        DEF_rUNARY(rinc,++) DEF_rUNARY(rdec,--)
      #undef DEF_rUNARY

      #define DEF_lUNARY(NAME,OP) DEF_TRY(var,NAME,OP,OP std::declval<S&>(),OP (*(S*)o),Storage o)
        DEF_lUNARY(dref,*) DEF_lUNARY(linc,++) DEF_lUNARY(ldec,--) DEF_lUNARY(neg,-) DEF_lUNARY(pos,+)
      #undef DEF_lUNARY

      #define DEF_BINARY(NAME,OP) DEF_TRY(var,NAME,OP,std::declval<S&>() OP std::declval<S&>(),(*(S*)o1 OP *(S*)o2),Storage o1,Storage o2)
        DEF_BINARY(add,+) DEF_BINARY(sub,-) DEF_BINARY(mul,*) DEF_BINARY(div,/) DEF_BINARY(mod,%)
        DEF_BINARY(and,&) DEF_BINARY(or,|) DEF_BINARY(xor,^) DEF_BINARY(lshift,<<) DEF_BINARY(rshift,>>)
        DEF_BINARY(eq,==) DEF_BINARY(ne,!=) DEF_BINARY(lt,<) DEF_BINARY(le,<=) DEF_BINARY(gt,>) DEF_BINARY(ge,>=)
        DEF_BINARY(iadd,+=) DEF_BINARY(isub,-=) DEF_BINARY(imul,*=) DEF_BINARY(idiv,/=) DEF_BINARY(imod,%=)
        DEF_BINARY(iand,&=) DEF_BINARY(ior,|=) DEF_BINARY(ixor,^=) DEF_BINARY(ilshift,<<=) DEF_BINARY(irshift,>>=)
      #undef DEF_BINARY

      // #define DEF_CAST(TYPE) DEF_TRY(TYPE,TYPE,TYPE,(TYPE)(std::declval<S&>()),(TYPE)(*(S*)o),Storage o)
      #define DEF_CAST(TYPE) DEF_TRY(TYPE,TYPE,TYPE,static_cast<TYPE>(std::declval<S>()),static_cast<TYPE>(*static_cast<S*>(o)),Storage o)
        DEF_CAST(bool) DEF_CAST(int) DEF_CAST(double) DEF_CAST(char)
      #undef DEF_CAST


      template <typename S> static var __getitem__(...) {
        throw std::runtime_error("\033[1;31mTypeError\033[0m: type " + std::string(abi::__cxa_demangle(typeid(S).name(), 0, 0, 0)) + " has no operator []");
      }
      // template <typename S, typename=decltype(std::declval<S&>()[std::declval<var>()]),std::enable_if_t<!std::is_const<S>::value,int> = 0>
      // template <typename T,typename S=std::remove_const_t<T>,typename=decltype(std::declval<S&>()[std::declval<var>()])>
      // static var __getitem__(Storage o, var k) {
      //   return var( &((*(S*)o)[k]) ,new handle<S>(true));
      // }

      
      template<typename U> class can_getitem_ref {
          template<typename S,typename=decltype(std::addressof(std::declval<S>()[std::declval<var>()]))> 
            static std::true_type check(int); 
          template<typename> 
            static std::false_type check(...); 
          public: static constexpr bool value = decltype(check<U>(0))::value && !std::is_pointer<U>::value; 
      };
      template<typename U> static constexpr bool can_getitem_ref_v = can_getitem_ref<U>::value;
      

      // get a reference
      template <typename S, typename V=decltype(std::declval<S>()[std::declval<var>()]),
                std::enable_if_t<can_getitem_ref_v<S>,int> = 0>
      static var __getitem__(Storage o, var k) {
        return var(std::addressof((*static_cast<S*>(o))[k]) ,new handle<V>(true));
      }
      // get a copy
      template <typename S, typename V=decltype(std::declval<S>()[std::declval<var>()]),
                std::enable_if_t<!can_getitem_ref_v<S>,int> = 0>
      static var __getitem__(Storage o, var k) {
        return var((*static_cast<S*>(o))[k]);
      }

      template<typename S,typename=decltype(std::cout<<std::declval<S>())>
      static std::string __str__(Storage o) {
        std::stringstream ss;
        if (o) switch (std::is_pointer<S>::value) {
            case true:
              if (typeid(S) == typeid(char*) || typeid(S) == typeid(const char*))
                ss << *(char**)o;
              else
                ss << o;  
              break;
            case false: 
              if (typeid(S) == typeid(bool))
                ss << (*static_cast<bool*>(o) ? "True" : "False");
              else
                ss << *static_cast<S*>(o);
          }
        else
          ss << "None";
        return ss.str();
      }
      
      template<typename S>
        static std::string __str__(...) { 
          throw std::runtime_error("\033[1;31mTypeError\033[0m: type " + std::string(abi::__cxa_demangle(typeid(S).name(), 0, 0, 0)) + " can not be converted to string");
        }

      #define DEF_MEMBER_FUNC(NAME) \
      template<typename S,typename=decltype(std::declval<S>().NAME())> \
        static var __##NAME##__(Storage o) { \
          return static_cast<S*>(o)->NAME(); \
        } \
      template<typename S>  \
        static var __##NAME##__(...) { \
          throw std::runtime_error("\033[1;31mTypeError\033[0m: type " + std::string(abi::__cxa_demangle(typeid(S).name(), 0, 0, 0)) + " has no member function " #NAME); \
        }
      DEF_MEMBER_FUNC(begin) DEF_MEMBER_FUNC(end)
      #undef DEF_MEMBER_FUNC

    

    };


    template<typename U, typename T=std::decay_t<U>>
    struct handle : public handle_base {
      handle(bool is_ref=false) : handle_base(is_ref) {}

      Storage copy(Storage o) { return new T(*static_cast<T*>(o));}
      Type* type() override { return &typeid(T); }
      Size size() override { return sizeof(T); }
      void destroy(Storage o) override { if (!is_ref) delete static_cast<T*>(o); }

      #define DEF_UNARY(NAME) var __##NAME##__(Storage o) override { return handle_helper::__##NAME##__<T>(o); }
      DEF_UNARY(dref) DEF_UNARY(rinc) DEF_UNARY(rdec) DEF_UNARY(linc) DEF_UNARY(ldec) DEF_UNARY(neg) DEF_UNARY(pos)
      #undef DEF_UNARY


      #define DEF_BINARY(NAME) var __##NAME##__(Storage o1, Storage o2) override { return handle_helper::__##NAME##__<T>(o1,o2); }
      DEF_BINARY(add) DEF_BINARY(sub) DEF_BINARY(mul) DEF_BINARY(div) DEF_BINARY(mod)
      DEF_BINARY(and) DEF_BINARY(or) DEF_BINARY(xor) DEF_BINARY(lshift) DEF_BINARY(rshift)
      DEF_BINARY(eq) DEF_BINARY(ne) DEF_BINARY(lt) DEF_BINARY(le) DEF_BINARY(gt) DEF_BINARY(ge)
      DEF_BINARY(iadd) DEF_BINARY(isub) DEF_BINARY(imul) DEF_BINARY(idiv) DEF_BINARY(imod)
      DEF_BINARY(iand) DEF_BINARY(ior) DEF_BINARY(ixor) DEF_BINARY(ilshift) DEF_BINARY(irshift)
      #undef DEF_BINARY

      var __getitem__(Storage o, var k) override { return handle_helper::__getitem__<T>(o,k); }

      #define DEF_CAST(TYPE) TYPE __##TYPE##__(Storage o) override { return handle_helper::__##TYPE##__<T>(o); }
      DEF_CAST(bool) DEF_CAST(int) DEF_CAST(double) DEF_CAST(char)
      #undef DEF_CAST

      std::string __str__(Storage o) override { return handle_helper::__str__<T>(o); }
      std::string __repr__(Storage o) override {
        if (typeid(T) == typeid(char))
          return '\''+__str__(o)+'\'';
        else if (typeid(T) == typeid(char*) || typeid(T) == typeid(const char*) || typeid(T) == typeid(std::string))
          return '\"'+ __str__(o)+'\"';
        else
          return __str__(o);        
      }

      var __begin__(Storage o) override { return handle_helper::__begin__<T>(o); }
      var __end__(Storage o) override { return handle_helper::__end__<T>(o); }
      
    };
    
    template<typename U, typename T=std::decay_t<U>, typename A>   Storage new_(A && v)   { return new T(std::forward<U>(v)); }
    template<typename U, typename T=std::decay_t<U>, typename...A> Storage new_(A &&...a) { return new T(std::forward<A>(a)...); }


    Storage m_data;
    handle_base* m_handle;

    var(Storage o, handle_base* h) : m_data(o), m_handle(h) {}

    
    

  public:
    // APIs
    bool has_value() const { return m_handle != nullptr; }
    bool is_empty() const { return m_handle == nullptr; }
    bool is_ref() const { return m_handle ? m_handle->is_ref : false; }
    // bool is_ptr() const { return 
    Type& type() const { return m_handle ? *m_handle->type():  typeid(void) ; }
    Size size() const { return m_handle ? m_handle->size():0; }

    std::string __repr__() const {
      return m_handle ? m_handle->__repr__(m_data) : "None";
    }
    std::string __str__() const {
      return m_handle ? m_handle->__str__(m_data) : "None";
    }

    template<typename T> operator T() const {
      return get<T>();
    }
    template<typename T>
    T& get() {
      if (type() != typeid(T)) 
        throw bad_cast(type(), typeid(T));
      return *static_cast<T*>(m_data);
    }
    template<typename T>
    const T& get() const {
      if (type() != typeid(T)) 
        throw bad_cast(type(), typeid(T));
      return *static_cast<const T*>(m_data);
    }

    void swap(var &o) {
      std::swap(m_data, o.m_data);
      std::swap(m_handle, o.m_handle);
    }

    var begin() const {
      if (m_handle) return m_handle->__begin__(m_data);
      else throw std::runtime_error("TypeError: cannot call begin() on None");
    }
    var end() const {
      if (m_handle) return m_handle->__end__(m_data);
      else throw std::runtime_error("TypeError: cannot call end() on None");
    }


    // constructors
    var() : var(nullptr,nullptr) {}
    var(var &o) : m_data(o.m_handle ? o.m_handle->copy(o.m_data) : nullptr), m_handle(o.m_handle) {}
    var(const var &o) : m_data(o.m_handle ? o.m_handle->copy(o.m_data) : nullptr), m_handle(o.m_handle) {}
    var(var &&o) : m_data(o.m_data), m_handle(o.m_handle) { o.m_data = nullptr; o.m_handle = nullptr; }

    /// @note: the most important constructor that construct from var type
    template <typename T> var(T &&v) : m_data(new_<T>(std::forward<T>(v))), m_handle(new handle<T>()) {}
    // template <typename T, typename...A> var(A &&...a) : m_data(new_<T>(std::forward<A>(a)...)), m_handle(new handle<T>()) {}
    // /// @note: call var(something) explicitly to construct by reference
    // template <typename T, std::enable_if_t<!std::is_const<T>::value>> 
    // explicit var(T &v) : m_data(&v), m_handle(new handle<T>(true)) {
    //   std::cout << "var(T&) called" << std::endl;
    // }



    // destructor
    ~var() { if (m_handle) { m_handle->destroy(m_data); } }

    // assignment
    var& operator=(const var &o) { var(o).swap(*this); return *this; }
    var& operator=(var &o) { var(o).swap(*this); return *this; }
    var& operator=(var &&o) { var(std::move(o)).swap(*this);  return *this; }
    // template <typename T,typename=decltype(*(T*)nullptr = std::declval<T>())>
    template <typename T>
    var& operator=(T &&v) {
      
      if (type() != typeid(T) && !is_ref()) {
        var(std::forward<T>(v)).swap(*this);
      } else {
        *((T*)m_data) = std::forward<T>(v);
      }
      return *this; 
    }

    template<typename T>
    var& operator=(const T &v) {
      if (type() != typeid(T) && !is_ref()) {
        var(v).swap(*this);
      } else {
        *((T*)m_data) = v;
      }
      return *this; 
    }

    // template <typename T, typename...A> var& operator=(A &&...a) { var(std::forward<A>(a)...).swap(*this);  return *this; }

    bool operator==(const var& o) const {
      if (type() == o.type())
        return m_handle->__eq__(m_data, o.m_data);
      return false;
    }
    #define DEF_CALC(OP,NAME) \
      var operator OP(const var& o) const { \
        if (type() == o.type()) \
          return m_handle->__##NAME##__(m_data, o.m_data); \
        throw std::runtime_error("\033[1;31mType mismatch\033[0m: got "+python::type(type())+ " " #OP " " +python::type(o.type()) + ", but only operation of the same type is allowed"); \
      }
    // comparison
    DEF_CALC(!=,ne) DEF_CALC(<,lt) DEF_CALC(<=,le) DEF_CALC(>,gt) DEF_CALC(>=,ge)
    

    // arithmetic operators
    DEF_CALC(+,add) DEF_CALC(-,sub) DEF_CALC(*,mul) DEF_CALC(/,div) DEF_CALC(%,mod)
    DEF_CALC(&,and) DEF_CALC(|,or) DEF_CALC(^,xor) DEF_CALC(<<,lshift) DEF_CALC(>>,rshift)
    #undef DEF_CALC

    // self-arithmetic operators
    #define DEF_SELF_CALC(OP,NAME)\
      var& operator OP(const var& o) {  \
        if (type() == o.type()) { \
          m_handle->__##NAME##__(m_data, o.m_data); \
          return *this; \
        } \
        throw std::runtime_error("\033[1;31mType mismatch\033[0m: got "+python::type(type())+ " " #OP " " +python::type(o.type()) + " only operation of the same type is allowed"); \
      }
    DEF_SELF_CALC(+=,iadd) DEF_SELF_CALC(-=,isub) DEF_SELF_CALC(*=,imul) DEF_SELF_CALC(/=,idiv) DEF_SELF_CALC(%=,imod)
    DEF_SELF_CALC(&=,iand) DEF_SELF_CALC(|=,ior) DEF_SELF_CALC(^=,ixor) DEF_SELF_CALC(<<=,ilshift) DEF_SELF_CALC(>>=,irshift)
    #undef DEF_SELF_CALC



    #define DEF_UNARY(OP,NAME) \
      var operator OP() const { \
        return m_handle->__##NAME##__(m_data); \
      }
    DEF_UNARY(+,pos) DEF_UNARY(-,neg) DEF_UNARY(*,dref)
    #undef DEF_UNARY

    var operator++(int) {
      return m_handle->__rinc__(m_data);
    }
    var operator--(int) {
      return m_handle->__rdec__(m_data);
    }
    var& operator++() {
      m_handle->__linc__(m_data);
      return *this;
    }
    var& operator--() {
      m_handle->__ldec__(m_data);
      return *this;
    }


    // cast
    operator bool() const {
      return m_handle ? m_handle->__bool__(m_data) : false;
    }
    #define DEF_CAST(TYPE) operator TYPE() const { return m_handle ? m_handle->__##TYPE##__(m_data) : TYPE(); }
    DEF_CAST(int) DEF_CAST(double) DEF_CAST(char)
    #undef DEF_CAST

    operator unsigned long long() const { return int(*this); }

    operator std::string() const { return m_handle ? m_handle->__str__(m_data) : ""; }
    

    var operator[](var key) const {
      return m_handle->__getitem__(m_data, key);
    }

    Storage operator&() { return m_data; }

    // stream
    friend std::ostream& operator<<(std::ostream &os, const var &o) {
      return os << o.__str__();
    }
  };


  std::string type(const var &v) {
    return abi::__cxa_demangle(v.type().name(), NULL, NULL, NULL);
  }

  template <typename T>
  size_t len(const T &t) {
    return t.__len__();
  }



}

#endif