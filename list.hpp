#ifndef PYTHON_LIST_HPP
#define PYTHON_LIST_HPP
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>
#include <memory>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <typeinfo>



#include "var.hpp"

namespace python {

class list {

public:
    using value_type = var;
    using size_type = int;
    using difference_type = int;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using iterator = std::vector<value_type>::iterator;
    using const_iterator = std::vector<value_type>::const_iterator;
    using reverse_iterator = std::vector<value_type>::reverse_iterator;
    using const_reverse_iterator = std::vector<value_type>::const_reverse_iterator;

    std::shared_ptr<std::vector<value_type>> m_data;

    #define IDX(i) ((i)<0 ? LEN+(i) : (i))
    #define LEN (m_data->size())
    #define OUT_OF_RANGE(i) (i<0 || i>=LEN) 
    #define CLIP(i) ((i)<0 ? 0 : (i)>LEN ? LEN : (i))

    list() : m_data(std::make_shared<std::vector<value_type>>()) {}
    list(const list&) = default;
    list& operator=(const list&) = default;
    template<typename T>
    list(std::vector<T> l) : m_data(std::make_shared<std::vector<value_type>>(l)) {}
    list(std::initializer_list<value_type> l) : m_data(std::make_shared<std::vector<value_type>>(l)) {}
    template <typename it> list(it iter) {
        m_data = std::make_shared<std::vector<value_type>>();
        for(auto i: iter) m_data->push_back(i);
    }



    // cpp methods
    reference at(size_type i) { return const_cast<reference>(static_cast<const list&>(*this).at(i)); }
    const_reference at(size_type i) const {
        i = IDX(i);
        if (i < 0 || i >= LEN) 
            throw std::out_of_range("\033[1;31mIndexError\033[0m: list index out of range");
        return (*m_data)[i];
    }
    reference front() { return const_cast<reference>(static_cast<const list&>(*this).front()); }
    const_reference front() const { return (*m_data)[0]; }
    reference back() { return const_cast<reference>(static_cast<const list&>(*this).back()); }
    const_reference back() const { return (*m_data)[-1]; }
    pointer data() { return const_cast<pointer>(static_cast<const list&>(*this).data()); }
    const_pointer data() const { return m_data->data(); }
    size_type size() const { return LEN; }
    bool empty() const { return m_data->empty(); }
    iterator begin() const { return m_data->begin(); }
    iterator end() const { return m_data->end(); }
    reverse_iterator rbegin() const { return m_data->rbegin(); }
    reverse_iterator rend() const { return m_data->rend(); }
    const_iterator cbegin() const { return m_data->cbegin(); }
    const_iterator cend() const { return m_data->cend(); }
    const_reverse_iterator crbegin() const { return m_data->crbegin(); }
    const_reverse_iterator crend() const { return m_data->crend(); }






    
    // python methods
    list& append(value_type value) {
        m_data->push_back(value);
        return *this;
    }
    template <typename ...U> 
    list& append(const value_type& v,U&& ...args) { return append(v),append(args...); }

    void clear() {
        m_data->clear();
    }
    list copy() {
        return list(*m_data);
    }
    size_type count(value_type value) const {
        return std::count(m_data->begin(), m_data->end(), value);
    }


    list& extend(const list& l) {
        m_data->insert(m_data->end(), l.begin(), l.end());
        return *this;
    }
    size_type index(value_type value, size_type start=0, size_type stop=-1) const {
        auto left = std::next(m_data->begin(), IDX(start));
        auto right = std::next(m_data->begin(), IDX(stop));
        auto it = std::find(left, right, value);
        if (it == m_data->end())
            throw std::out_of_range("\033[1;31mValueError\033[0m: "+value.__repr__()+" is not in list");
        return std::distance(m_data->begin(), it);
    }

    list& insert(size_type i, value_type value) {
        i = CLIP(IDX(i));
        m_data->insert(std::next(m_data->begin(), i), value);
        return *this;
    }

    value_type pop(size_type i=-1) {
        if(empty()) throw std::out_of_range("\033[1;31mIndexError\033[0m: pop from empty list");
        i = IDX(i);
        if (OUT_OF_RANGE(i)) throw std::out_of_range("\033[1;31mIndexError\033[0m: pop index out of range");
        value_type v = (*m_data)[i];
        m_data->erase(std::next(m_data->begin(), i));
        return v;
    }

    list& remove(value_type value) {
        auto it = std::find(m_data->begin(), m_data->end(), value);
        if (it == m_data->end())
            throw std::out_of_range("\033[1;31mValueError\033[0m: "+value.__repr__()+" is not in list");
        m_data->erase(it);
        return *this;
    }

    list& reverse() {
        std::reverse(m_data->begin(), m_data->end());
        return *this;
    }

    list& sort(bool reverse = false) {
        switch (reverse) {
            case true: return sort(std::greater<value_type>());
            case false: return sort(std::less<value_type>());
        }
    }
    list& sort(std::function<bool(value_type, value_type)> cmp) {
        std::sort(m_data->begin(), m_data->end(), cmp);
        return *this;
    }

    void del(size_type i) {
        i = IDX(i);
        if (i < 0 || i >= LEN) 
            throw std::out_of_range("\033[1;31mIndexError\033[0m: list index out of range");
        m_data->erase(std::next(m_data->begin(), i));
    }

    // numpy
    bool all() const {
        for (auto& v : *m_data)
            if (!v) return false;
        return true;
    }

    bool any() const {
        for (auto& v : *m_data)
            if (v) return true;
        return false;
    }

    size_type argmax() const {
        if (empty()) throw std::out_of_range("\033[1;31mIndexError\033[0m: argmax from empty list");
        return std::distance(m_data->begin(), std::max_element(m_data->begin(), m_data->end()));
    }

    size_type argmin() const {
        if (empty()) throw std::out_of_range("\033[1;31mIndexError\033[0m: argmin from empty list");
        return std::distance(m_data->begin(), std::min_element(m_data->begin(), m_data->end()));
    }

    list argsort() const {
        list l;
        l.sort(std::less<value_type>());
        return l;
    }


    // other

    template<typename T>
    T get(size_type i) const {
        return (m_data->at(i)).get<T>();
    }
    list& set(size_type i, value_type value) {
        // m_data->at(i) = value;
        return *this;
    }

    size_type count_if(std::function<bool(value_type)> f) {
        return std::count_if(m_data->begin(), m_data->end(), f);
    }

    list slice(size_type start,size_type end,difference_type step=1) const {
        list l;
        if (step==0) {
            throw std::invalid_argument("\033[1;31mValueError\033[0m: slice step cannot be zero");
        } else if (step>0) {
            for (size_type i=start;i<end;i+=step) {
                if (i>=0 && i<LEN)
                    l.append(at(i));
                else
                    break;
            }
        } else {
            for (size_type i=start;i>end;i+=step) {
                if (i>=0 && i<LEN)
                    l.append(at(i));
                else
                    break;
            }
        }
        return l;
    }

    list& for_each(std::function<void(value_type&)> f) {
        for (auto& v : *m_data) f(v);
        return *this;
    }



    reference operator[](size_type i) { return at(i); }
    const_reference operator[](size_type i) const { return at(i); }
    const_reference operator()(size_type i) const { return at(i); }
    list operator()() { return copy(); }
    list operator()(size_type start, size_type end, difference_type step=1) { return slice(start, end, step); }
    list& operator+= (const list& l) {
        extend(l);
        return *this;
    }
    list& operator*= (size_type n) {
        list l = copy();
        m_data->resize(n*LEN);
        for (size_type i=0;i<n;i++) { operator+=(l); }
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& os, const list& l) {
        os << '[';
        for (auto& item : l)
            os << item.__repr__() << ", ";
        if (!l.empty())
            os << "\b\b";
        os << ']';
        return os;
    }



    // magic 
    size_type __len__() const { return LEN; }
    std::string __str__() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }
    std::string __repr__() const {
        return __str__();
    }
    
    


};


}
#endif