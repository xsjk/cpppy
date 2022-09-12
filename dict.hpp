#ifndef PYTHON_DICT_HPP
#define PYTHON_DICT_HPP

#include <map>
#include <memory>

#include "python.hpp"

namespace python {

  class dict {
    using value_type = var;
    using size_type = int;
    using m_type = std::map<value_type, value_type>;
    std::shared_ptr<m_type> m_data;
    public:
    dict() : m_data(std::make_shared<m_type>()) {}
    dict(const dict&) = default;
    dict& operator=(const dict&) = default;
    dict(m_type l) : m_data(std::make_shared<m_type>(l)) {}
    dict(std::initializer_list<std::pair<var,var>> l) : m_data(std::make_shared<m_type>(l.begin(), l.end())) {}

    value_type& operator[](value_type key) { return m_data->operator[](key); }

    // cpp methods
    value_type& at(value_type key) { return m_data->at(key); }
    const value_type& at(value_type key) const { return m_data->at(key); }
    bool contains(value_type key) const { return m_data->find(key) != m_data->end(); }
    size_type size() const { return m_data->size(); }
    bool empty() const { return m_data->empty(); }




    // python method
    void clear() { m_data->clear(); }


    friend std::ostream& operator<<(std::ostream& os, const dict& d) {
      os << "{";
      for (auto [k,v]: *d.m_data)
        os << k.__repr__() << ": " << v.__repr__() << ", ";
      if(!d.empty()) 
        os << "\b\b";
      os << "}";
      return os;
    }


  };






} // namespace python

#endif