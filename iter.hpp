#ifndef PYTHON_ITER_HPP
#define PYTHON_ITER_HPP

#include "python.hpp"
#include <type_traits>
#include <vector>
#include <list>
#include <functional>

namespace python {


  class iter {
    class _iter{
      list l;
      public:
      _iter(iter& it) : l(it.l) {}
      var operator*() { return l[0]; }
      _iter& operator++() { if (*this) l.del(0); return *this; }
      operator bool() { return !l.empty(); }
    };
    list l;
    public:
    iter(list l) : l(l) {}
    _iter begin() { return *this; }
    bool end() { return false; }
  };

  /******************************************************************************
   * python basic iterables
   ******************************************************************************/

  template <typename T>
  class range {
    class range_iter {
      T v, m_stop, m_step;
    public:
      range_iter(range &r) : v(r.m_start), m_stop(r.m_stop), m_step(r.m_step) {}
      var operator*() { return v; }
      range_iter &operator++() { return v += m_step, *this; }
      operator bool() { return m_step > 0 ? v < m_stop : v > m_stop; }
    };
    T zero = 0, m_start = 0, m_stop = 0, m_step = 1;

  public:
    range(T start, T stop, T step = 1) : m_start(start), m_stop(stop), m_step(step) {}
    range(T stop) : m_stop(stop) {}
    range() = default;
    range_iter begin() { return *this; }
    bool end() { return false; }  // this is just a placeholder
  };

  class zip {
    class zip_iter {
      std::vector<var> iters;
    public:
      zip_iter(zip& self) : iters(self.m_iters) {}
      list operator*() {
        list l;
        for (auto &i:iters) 
          l.append(*i);
        return l;
      }
      zip_iter &operator++() {
        for (auto &i:iters) ++i;
        return *this;
      }
      operator bool() {
        for (auto i:iters)
          if (!i) return false;
        return true;
      }
    };
    std::vector<var> m_iters;
  public:
    template <typename... Iters>
    zip(Iters&&...i) : m_iters({std::forward<Iters>(i).begin()...}) {}
    zip_iter begin() { return *this; }
    bool end() { return false; }  // this is just a placeholder
  };

  class map {
    using func = std::function<var(var)>;
    class map_iter {
      func f;
      var it;
    public:
      map_iter(map& self) : f(self.m_func), it(self.m_it.begin()) {
      }
      var operator*() { return f(*it); }
      map_iter &operator++() { ++it; return *this; }
      operator bool() { return it;}
    };
    func m_func;
    var m_it;
  public:
    map(func f, var it) : m_it(it), m_func(f) {}
    map_iter begin() { return *this; }
    bool end() { return false; }  // this is just a placeholder
  };

  class filter {
    using func = std::function<bool(var)>;
    class filter_iter {
      func f;
      var it;
    public:
      filter_iter(filter& self) : f(self.m_func), it(self.m_it) {}
      var operator*() { return *it; }
      filter_iter &operator++() { while (!f(*++it)); return *this; }
      operator bool() { return it; }
    };
    func m_func;
    var m_it;
  public:
    filter(func f, var it) : m_it(it.begin()), m_func(f) { while (!f(*m_it)) ++m_it; }
    filter_iter begin() { return *this; }
    bool end() { return false; }  // this is just a placeholder

  };

  /*----------------------------------------------------------------------------
   * itertools: infinite iterables
   *----------------------------------------------------------------------------*/

  namespace itertools
  {
    class count {
      class count_iter {
        int v,step;
      public:
        count_iter(count& c) : v(c.m_start), step(c.m_step) {}
        var operator*() { return v; }
        count_iter &operator++() { return v+=step, *this; }
        operator bool() { return true; }
      };
      int m_start, m_step;
    public:
      count(int start = 0, int step = 1) : m_start(start), m_step(step) {}
      count_iter begin() { return *this; }
      bool end() { return false; }  // this is just a placeholder
    };

    class repeat {
      class repeat_iter {
        var v;
        int count;
      public:
        repeat_iter(repeat& r) : v(r.m_v), count(r.m_count) {}
        var operator*() { return v; }
        repeat_iter &operator++() { return --count, *this; }
        operator bool() { return count; }
      };
      var m_v;
      int m_count;
    public:
      repeat(var v, int count = -1) : m_v(v), m_count(count) {}
      repeat_iter begin() { return *this; }
      bool end() { return false; }  // this is just a placeholder
    };



    class cycle {
      class cycle_iter {
        var begin, it;
      public:
        cycle_iter(cycle& c) : begin(c.m_begin), it(begin) {}
        var operator*() { return *it; }
        cycle_iter &operator++() { ++it ? it : it = begin; return *this; }  
        operator bool() { return true; }
      };
      var m_begin;
    public:
      cycle(var i) : m_begin(i.begin()) {}
      cycle_iter begin() { return *this; }
      bool end() { return false; }  // this is just a placeholder
    };
        

    /*----------------------------------------------------------------------------
     * itertools: finite iterables
     *----------------------------------------------------------------------------*/

    class accumulate {
      class accumulate_iter {
        var it, sum;
      public:
        accumulate_iter(accumulate& a) : it(a.m_it.begin()), sum(a.m_start) {}
        var operator*() { return sum; }
        accumulate_iter &operator++() { sum += *++it; return *this; }
        operator bool() { return it; }
      };
      var m_it;
      var m_start;
    public:
      accumulate(var i, var start = 0) : m_it(i), m_start(start) {}
      accumulate_iter begin() { return *this; }
      bool end() { return false; }  // this is just a placeholder
    };

    class chain {
      class chain_iter {
        std::list<var> begins;
        var it;
      public:
        chain_iter(chain& self) : begins(self.m_begins), it(begins.front()) {}
        var operator*() { return *it; }
        chain_iter &operator++() {
          if (++it) {
            return *this;
          } else {
            begins.pop_front();
            if (!begins.empty())
              it = begins.front();
            return *this;
          }
        }
        operator bool() {
          return !begins.empty() || it;
        }
      };
      std::list<var> m_begins;
    public:
      template <typename... Iters>
      chain(Iters&&...i) : m_begins({std::forward<Iters>(i).begin()...}) {}
      chain_iter begin() { return *this; }
      bool end() { return false; }  // this is just a placeholder
    };

    class compress {
      class compress_iter {
        var data, selectors;
      public:
        compress_iter(compress& c) : data(c.m_data), selectors(c.m_selectors) {
          while (data && selectors && !*selectors) ++data, ++selectors;
        }
        var operator*() { return *data; }
        compress_iter &operator++() {
          while (++data && ++selectors && !*selectors);
          return *this;
        }
        operator bool() { return data&&selectors; }
      };
      var m_data, m_selectors;
    public:
      compress(var d, var s) : m_data(d.begin()), m_selectors(s.begin()) {}
      compress(var d, list s) : m_data(d.begin()), m_selectors(iter(s).begin()) {}
      compress_iter begin() { return *this; }
      bool end() { return false; }  // this is just a placeholder
    };

    class dropwhile {
      using func = std::function<bool(var)>;
      class dropwhile_iter {
        var it;
      public:
        dropwhile_iter(dropwhile& d) : it(d.m_it) {}
        var operator*() { return *it; }
        dropwhile_iter &operator++() { ++it; return *this; }
        operator bool() { return it; }
      };
      var m_it;
    public:
      dropwhile(func f, var i) : m_it(i.begin()) { while (m_it && f(*m_it)) ++m_it; }
      dropwhile_iter begin() { return *this; }
      bool end() { return false; }  // this is just a placeholder
    };

    class groupby {
      using func = std::function<var(var)>;
      var m_it;
    public:
      groupby(var it, func f=[](var v) { return v; }) {
        list keys;
        list res;
        

      }
      // groupby_iter begin() { return *this; }
      bool end() { return false; }  // this is just a placeholder
    };


    class filterfalse : public filter {
      public: filterfalse(std::function<bool(var)> f, var i) : filter([f](var x) { return !f(x); }, i) {}
    };

    class islice;

    class zip_longest;

    /*----------------------------------------------------------------------------
     * itertools: combinatoric iterables
     *----------------------------------------------------------------------------*/

    class product;

    class permutations;

    class combinations;

    class combinations_with_replacement;

  } // namespace itertools

  class enumerate : public zip {
  public:
    template <typename I>
    enumerate(I it,int start=0) : zip(itertools::count(0), it) {}
  };

} // namespace python
#endif // PYTHON_ITER_HPP