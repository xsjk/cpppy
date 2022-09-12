/** @file include/any
 *  This is a Standard C++ Library header.
 */

#ifndef _GLIBCXX_ANY
#define _GLIBCXX_ANY 1

#include <typeinfo>
#include <new>
#include <utility>
#include <type_traits>

namespace std
{
  /**
   *  @brief Exception class thrown by a failed @c any_cast
   *  @ingroup exceptions
   */
  class bad_any_cast : public bad_cast
  {
  public:
    virtual const char *what() const noexcept { return "bad any_cast"; }
  };

#define __throw_bad_any_cast() throw bad_any_cast{};

  /**
   *  @brief A type-safe container of any type.
   *
   *  An @c any object's state is either empty or it stores a contained object
   *  of CopyConstructible type.
   */
  class any
  {
    // Holds either pointer to a heap object or the contained object itself.
    using _Storage = void*;

    template <typename _Tp>
    struct _Manager_external; // creates contained object on the heap

    template <typename _Tp>
    using _Manager = _Manager_external<_Tp>;

    template <typename _Tp, typename _VTp = decay_t<_Tp>>
    using _Decay_if_not_any = enable_if_t<!is_same_v<_VTp, any>, _VTp>;

    /// Emplace with an object created from @p __args as the contained object.
    template <typename _Tp, typename... _Args,
              typename _Mgr = _Manager<_Tp>>
    void __do_emplace(_Args &&...__args)
    {
      reset();
      _Mgr::_S_create(_M_storage, std::forward<_Args>(__args)...);
      _M_manager_func = &_Mgr::_S_manage;
    }

    /// Emplace with an object created from @p __il and @p __args as
    /// the contained object.
    template <typename _Tp, typename _Up, typename... _Args,
              typename _Mgr = _Manager<_Tp>>
    void __do_emplace(initializer_list<_Up> __il, _Args &&...__args)
    {
      reset();
      _Mgr::_S_create(_M_storage, __il, std::forward<_Args>(__args)...);
      _M_manager_func = &_Mgr::_S_manage;
    }

    template <typename _Res, typename _Tp, typename... _Args>
    using __any_constructible = enable_if<
      __and_<
        is_copy_constructible<_Tp>,
        is_constructible<_Tp, _Args...>
      >::value,
      
      _Res>;

    template <typename _Tp, typename... _Args>
    using __any_constructible_t = typename __any_constructible<bool, _Tp, _Args...>::type;

    template <typename _VTp, typename... _Args>
    using __emplace_t = typename __any_constructible<_VTp &, _VTp, _Args...>::type;

  public:
    // construct/destruct

    /// Default constructor, creates an empty object.
    any() noexcept : _M_manager_func(nullptr) {}

    /// Copy constructor, copies the state of @p __other
    any(const any &__other)
    {
      if (!__other.has_value())
        _M_manager_func = nullptr;
      else
      {
        _Arg __arg;
        __arg._M_any = this;
        __other._M_manager_func(_Op_clone, &__other, &__arg);
      }
    }

    /**
     * @brief Move constructor, transfer the state from @p __other
     *
     * @post @c !__other.has_value() (this postcondition is a GNU extension)
     */
    any(any &&__other) noexcept
    {
      if (!__other.has_value())
        _M_manager_func = nullptr;
      else
      {
        _Arg __arg;
        __arg._M_any = this;
        __other._M_manager_func(_Op_xfer, &__other, &__arg);
      }
    }

    /// Construct with a copy of @p __value as the contained object.
    template <typename _Tp, typename _VTp = _Decay_if_not_any<_Tp>,
              typename _Mgr = _Manager<_VTp>,
              enable_if_t<is_copy_constructible<_VTp>::value && !__is_in_place_type<_VTp>::value, bool> = true>
    any(_Tp &&__value)
        : _M_manager_func(&_Mgr::_S_manage)
    {
      _Mgr::_S_create(_M_storage, std::forward<_Tp>(__value));
    }

    /// Construct with an object created from @p __args as the contained object.
    template <typename _Tp, typename... _Args, typename _VTp = decay_t<_Tp>,
              typename _Mgr = _Manager<_VTp>,
              __any_constructible_t<_VTp, _Args &&...> = false>
    explicit any(in_place_type_t<_Tp>, _Args &&...__args)
        : _M_manager_func(&_Mgr::_S_manage)
    {
      _Mgr::_S_create(_M_storage, std::forward<_Args>(__args)...);
    }

    /// Construct with an object created from @p __il and @p __args as
    /// the contained object.
    template <typename _Tp, typename _Up, typename... _Args,
              typename _VTp = decay_t<_Tp>, typename _Mgr = _Manager<_VTp>,
              __any_constructible_t<_VTp, initializer_list<_Up> &,
                                    _Args &&...> = false>
    explicit any(in_place_type_t<_Tp>, initializer_list<_Up> __il, _Args &&...__args)
        : _M_manager_func(&_Mgr::_S_manage)
    {
      _Mgr::_S_create(_M_storage, __il, std::forward<_Args>(__args)...);
    }

    /// Destructor, calls @c reset()
    ~any() { reset(); }

    // assignments

    /// Copy the state of another object.
    any &
    operator=(const any &__rhs)
    {
      *this = any(__rhs);
      return *this;
    }

    /**
     * @brief Move assignment operator
     *
     * @post @c !__rhs.has_value() (not guaranteed for other implementations)
     */
    any &
    operator=(any &&__rhs) noexcept
    {
      if (!__rhs.has_value())
        reset();
      else if (this != &__rhs)
      {
        reset();
        _Arg __arg;
        __arg._M_any = this;
        __rhs._M_manager_func(_Op_xfer, &__rhs, &__arg);
      }
      return *this;
    }

    /// Store a copy of @p __rhs as the contained object.
    template <typename _Tp>
    enable_if_t<is_copy_constructible<_Decay_if_not_any<_Tp>>::value, any &>
    operator=(_Tp &&__rhs)
    {
      *this = any(std::forward<_Tp>(__rhs));
      return *this;
    }

    /// Emplace with an object created from @p __args as the contained object.
    template <typename _Tp, typename... _Args>
    __emplace_t<decay_t<_Tp>, _Args...>
    emplace(_Args &&...__args)
    {
      using _VTp = decay_t<_Tp>;
      __do_emplace<_VTp>(std::forward<_Args>(__args)...);
      any::_Arg __arg;
      this->_M_manager_func(any::_Op_access, this, &__arg);
      return *static_cast<_VTp *>(__arg._M_obj);
    }

    /// Emplace with an object created from @p __il and @p __args as
    /// the contained object.
    template <typename _Tp, typename _Up, typename... _Args>
    __emplace_t<decay_t<_Tp>, initializer_list<_Up> &, _Args &&...>
    emplace(initializer_list<_Up> __il, _Args &&...__args)
    {
      using _VTp = decay_t<_Tp>;
      __do_emplace<_VTp, _Up>(__il, std::forward<_Args>(__args)...);
      any::_Arg __arg;
      this->_M_manager_func(any::_Op_access, this, &__arg);
      return *static_cast<_VTp *>(__arg._M_obj);
    }

    // modifiers

    /// If not empty, destroy the contained object.
    void reset() noexcept
    {
      if (has_value())
      {
        _M_manager_func(_Op_destroy, this, nullptr);
        _M_manager_func = nullptr;
      }
    }

    /// Exchange state with another object.
    void swap(any &__rhs) noexcept
    {
      if (!has_value() && !__rhs.has_value())
        return;

      if (has_value() && __rhs.has_value())
      {
        if (this == &__rhs)
          return;

        any __tmp;
        _Arg __arg;
        __arg._M_any = &__tmp;
        __rhs._M_manager_func(_Op_xfer, &__rhs, &__arg);
        __arg._M_any = &__rhs;
        _M_manager_func(_Op_xfer, this, &__arg);
        __arg._M_any = this;
        __tmp._M_manager_func(_Op_xfer, &__tmp, &__arg);
      }
      else
      {
        any *__empty = !has_value() ? this : &__rhs;
        any *__full = !has_value() ? &__rhs : this;
        _Arg __arg;
        __arg._M_any = __empty;
        __full->_M_manager_func(_Op_xfer, __full, &__arg);
      }
    }

    // observers

    /// Reports whether there is a contained object or not.
    bool has_value() const noexcept { return _M_manager_func != nullptr; }

    /// The @c typeid of the contained object, or @c typeid(void) if empty.
    const type_info &type() const noexcept
    {
      if (!has_value())
        return typeid(void);
      _Arg __arg;
      _M_manager_func(_Op_get_type_info, this, &__arg);
      return *__arg._M_typeinfo;
    }

    template <typename _Tp>
    static constexpr bool __is_valid_cast()
    {
      return __or_<is_reference<_Tp>, is_copy_constructible<_Tp>>::value;
    }

  private:
    enum _Op
    {
      _Op_access,
      _Op_get_type_info,
      _Op_clone,
      _Op_destroy,
      _Op_xfer
    };

    union _Arg
    {
      /// void指针
      void *_M_obj;
      /// type_info指针
      const std::type_info *_M_typeinfo;
      /// any指针
      any *_M_any;
    };


    ///关键之处：用函数指针来指向不同的管理函数，这样就可以在不同的类型上调用不同的管理函数
    void (*_M_manager_func)(_Op, const any *, _Arg *);
    _Storage _M_storage;

    template <typename _Tp>
    friend void *__any_caster(const any *__any);

    // Manage external contained object.
    template <typename _Tp>
    struct _Manager_external
    {
      static void
      _S_manage(_Op __which, const any *__anyp, _Arg *__arg);

      template <typename _Up>
      static void
      _S_create(_Storage &__storage, _Up &&__value)
      {
        __storage = new _Tp(std::forward<_Up>(__value));
      }
      template <typename... _Args>
      static void
      _S_create(_Storage &__storage, _Args &&...__args)
      {
        __storage = new _Tp(std::forward<_Args>(__args)...);
      }
    };
  };

  /// Exchange the states of two @c any objects.
  inline void swap(any &__x, any &__y) noexcept { __x.swap(__y); }

  /// Create an any holding a @c _Tp constructed from @c __args.
  template <typename _Tp, typename... _Args>
  any make_any(_Args &&...__args)
  {
    return any(in_place_type<_Tp>, std::forward<_Args>(__args)...);
  }

  /// Create an any holding a @c _Tp constructed from @c __il and @c __args.
  template <typename _Tp, typename _Up, typename... _Args>
  any make_any(initializer_list<_Up> __il, _Args &&...__args)
  {
    return any(in_place_type<_Tp>, __il, std::forward<_Args>(__args)...);
  }

  /**
   * @brief Access the contained object.
   *
   * @tparam  _ValueType  A const-reference or CopyConstructible type.
   * @param   __any       The object to access.
   * @return  The contained object.
   * @throw   bad_any_cast If <code>
   *          __any.type() != typeid(remove_reference_t<_ValueType>)
   *          </code>
   */
  template <typename _ValueType>
  inline _ValueType any_cast(const any &__any)
  {
    //// 去除引用和const
    using _Up = __remove_cvref_t<_ValueType>;
    static_assert(any::__is_valid_cast<_ValueType>(),
                  "Template argument must be a reference or CopyConstructible type");
    static_assert(is_constructible_v<_ValueType, const _Up &>,
                  "Template argument must be constructible from a const value.");
    auto __p = any_cast<_Up>(&__any);
    if (__p)
      return static_cast<_ValueType>(*__p);
    __throw_bad_any_cast();
  }

  /**
   * @brief Access the contained object.
   *
   * @tparam  _ValueType  A reference or CopyConstructible type.
   * @param   __any       The object to access.
   * @return  The contained object.
   * @throw   bad_any_cast If <code>
   *          __any.type() != typeid(remove_reference_t<_ValueType>)
   *          </code>
   *
   * @{
   */
  template <typename _ValueType>
  inline _ValueType any_cast(any &__any)
  {
    using _Up = __remove_cvref_t<_ValueType>;
    static_assert(any::__is_valid_cast<_ValueType>(),
                  "Template argument must be a reference or CopyConstructible type");
    static_assert(is_constructible_v<_ValueType, _Up &>,
                  "Template argument must be constructible from an lvalue.");
    auto __p = any_cast<_Up>(&__any);
    if (__p)
      return static_cast<_ValueType>(*__p);
    __throw_bad_any_cast();
  }

  template <typename _ValueType>
  inline _ValueType any_cast(any &&__any)
  {
    using _Up = __remove_cvref_t<_ValueType>;
    static_assert(any::__is_valid_cast<_ValueType>(),
                  "Template argument must be a reference or CopyConstructible type");
    static_assert(is_constructible_v<_ValueType, _Up>,
                  "Template argument must be constructible from an rvalue.");
    auto __p = any_cast<_Up>(&__any);
    if (__p)
      return static_cast<_ValueType>(std::move(*__p));
    __throw_bad_any_cast();
  }
  /// @}

  /// @cond undocumented
  template <typename _Tp>
  void *__any_caster(const any *__any)
  {
    // any_cast<T> returns non-null if __any->type() == typeid(T) and
    // typeid(T) ignores cv-qualifiers so remove them:
    using _Up = remove_cv_t<_Tp>;
    // The contained value has a decayed type, so if decay_t<U> is not U,
    // then it's not possible to have a contained value of type U:
    if constexpr (!is_same_v<decay_t<_Up>, _Up>)
      return nullptr;
    // Only copy constructible types can be used for contained values:
    else if constexpr (!is_copy_constructible_v<_Up>)
      return nullptr;
    // First try comparing function addresses, which works without RTTI
    else if (__any->_M_manager_func == &any::_Manager<_Up>::_S_manage
           || __any->type() == typeid(_Tp)
    )
    {
      any::_Arg __arg;
      __any->_M_manager_func(any::_Op_access, __any, &__arg);
      return __arg._M_obj;
    }
    return nullptr;
  }
  /// @endcond

  /**
   * @brief Access the contained object.
   *
   * @tparam  _ValueType  The type of the contained object.
   * @param   __any       A pointer to the object to access.
   * @return  The address of the contained object if <code>
   *          __any != nullptr && __any.type() == typeid(_ValueType)
   *          </code>, otherwise a null pointer.
   *
   * @{
   */
  template <typename _ValueType>
  inline const _ValueType *any_cast(const any *__any) noexcept
  {
    if constexpr (is_object_v<_ValueType>)
      if (__any)
        return static_cast<_ValueType *>(__any_caster<_ValueType>(__any));
    return nullptr;
  }

  template <typename _ValueType>
  inline _ValueType *any_cast(any *__any) noexcept
  {
    if constexpr (is_object_v<_ValueType>)
      if (__any)
        return static_cast<_ValueType *>(__any_caster<_ValueType>(__any));
    return nullptr;
  }
  /// @}

  /// 操作__any，如果要返回则放在__arg中
  template <typename _Tp> void any::_Manager_external<_Tp>::
      _S_manage(_Op __which, const any *__any, _Arg *__arg)
  {
    //// _M_storage 其实是 void*
    auto __ptr = static_cast<const _Tp *>(__any->_M_storage);
    std::cout << typeid(__ptr).name() << std::endl;
    //// _M_obj
    switch (__which)
    {
    /// 获取storage 放到 _Arg
    case _Op_access:
      __arg->_M_obj = const_cast<_Tp *>(__ptr);
      break;
    /// 把typeid 放到 _Arg
    case _Op_get_type_info:
      __arg->_M_typeinfo = &typeid(_Tp);
      break;
    /// 把_M_any 的副本 放进 _Arg
    case _Op_clone:
      __arg->_M_any->_M_storage = new _Tp(*__ptr);
      __arg->_M_any->_M_manager_func = __any->_M_manager_func;
      break;
    /// 销毁_M_any里面的_storage
    case _Op_destroy:
      delete __ptr;
      break;
    /// 把_M_any 移动到 _Arg 
    case _Op_xfer:
      __arg->_M_any->_M_storage = __any->_M_storage;
      __arg->_M_any->_M_manager_func = __any->_M_manager_func;
      const_cast<any *>(__any)->_M_manager_func = nullptr;
      break;
    }
  }

  /// @}

  namespace __detail::__variant
  {
    template <typename>
    struct _Never_valueless_alt; // see <variant>

    // Provide the strong exception-safety guarantee when emplacing an
    // any into a variant.
    template <>
    struct _Never_valueless_alt<std::any>
        : std::true_type
    {
    };
  } // namespace __detail::__variant

  _GLIBCXX_END_NAMESPACE_VERSION
} // namespace std

#endif // C++17
