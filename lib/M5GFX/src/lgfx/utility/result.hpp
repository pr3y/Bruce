////////////////////////////////////////////////////////////////////////////////
/// \file result.hpp
///
/// \brief This header contains the 'result' monadic type for indicating
///        possible error conditions
////////////////////////////////////////////////////////////////////////////////

/*
  The MIT License (MIT)

  Copyright (c) 2017-2021 Matthew Rodusek All rights reserved.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef RESULT_RESULT_HPP
#define RESULT_RESULT_HPP

#include "../internal/memory.h" // std::address_of

#include <stddef.h>     // size_t
#include <type_traits>  // std::enable_if, std::is_constructible, etc
#include <new>          // placement-new
#include <functional>   // std::reference_wrapper, std::invoke
#include <utility>      // std::in_place_t, std::forward
#include <initializer_list> // std::initializer_list
#include <string>       // std::string (for exception message)

#if defined(RESULT_EXCEPTIONS_DISABLED)
# include <stdio.h> // std::fprintf, stderr
#else
# include <stdexcept> // std::logic_error
#endif

#if __cplusplus >= 201402L
# define RESULT_CPP14_CONSTEXPR constexpr
#else
# define RESULT_CPP14_CONSTEXPR
#endif

#if __cplusplus >= 201703L
# define RESULT_CPP17_INLINE inline
#else
# define RESULT_CPP17_INLINE
#endif

#if defined(__clang__) && defined(_MSC_VER)
# define RESULT_INLINE_VISIBILITY __attribute__((visibility("hidden")))
#elif defined(__clang__) || defined(__GNUC__)
# define RESULT_INLINE_VISIBILITY __attribute__((visibility("hidden"), always_inline))
#elif defined(_MSC_VER)
# define RESULT_INLINE_VISIBILITY __forceinline
#else
# define RESULT_INLINE_VISIBILITY
#endif

// // [[clang::warn_unused_result]] is more full-featured than gcc's variant, since
// // it supports being applied to class objects.
// #if __cplusplus >= 201703L
// # define RESULT_NODISCARD [[nodiscard]]
// # define RESULT_WARN_UNUSED [[nodiscard]]
// #elif defined(__clang__) && ((__clang_major__ > 3) || ((__clang_major__ == 3) && (__clang_minor__ >= 9)))
// # define RESULT_NODISCARD [[clang::warn_unused_result]]
// # define RESULT_WARN_UNUSED [[clang::warn_unused_result]]
// #elif defined(__GNUC__)
// # define RESULT_NODISCARD
// # define RESULT_WARN_UNUSED [[gnu::warn_unused_result]]
// #else
# define RESULT_WARN_UNUSED
# define RESULT_NODISCARD
// #endif

#if defined(RESULT_NAMESPACE)
# define RESULT_NAMESPACE_INTERNAL RESULT_NAMESPACE
#else
# define RESULT_NAMESPACE_INTERNAL cpp
#endif
#define RESULT_NS_IMPL RESULT_NAMESPACE_INTERNAL::bitwizeshift

// clang's `-Wdocumentation-unknown-command` flag is bugged and does not
// understand `\copydoc` tags, despite this being a valid doxygen tag.
#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#endif

namespace RESULT_NAMESPACE_INTERNAL {
inline namespace bitwizeshift {

  //===========================================================================
  // utilities : constexpr forward
  //===========================================================================

  // std::forward is not constexpr until C++14
  namespace detail {
#if __cplusplus >= 201402L
    using std::forward;
#else
    template <typename T>
    inline RESULT_INLINE_VISIBILITY constexpr
    auto forward(typename std::remove_reference<T>::type& t)
      noexcept -> T&&
    {
      return static_cast<T&&>(t);
    }

    template <typename T>
    inline RESULT_INLINE_VISIBILITY constexpr
    auto forward(typename std::remove_reference<T>::type&& t)
      noexcept -> T&&
    {
      return static_cast<T&&>(t);
    }
#endif
  } // namespace detail


  //===========================================================================
  // utilities : invoke / invoke_result
  //===========================================================================

  // std::invoke was introduced in C++17

  namespace detail {
#if __cplusplus >= 201703L
    using std::invoke;
    using std::invoke_result;
    using std::invoke_result_t;
#else
    template<typename T>
    struct is_reference_wrapper : std::false_type{};

    template<typename U>
    struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type{};

    //-------------------------------------------------------------------------

    template <typename Base, typename T, typename Derived, typename... Args,
              typename = typename std::enable_if<
                std::is_function<T>::value &&
                std::is_base_of<Base, typename std::decay<Derived>::type>::value
              >::type>
    inline RESULT_INLINE_VISIBILITY constexpr
    auto invoke(T Base::*pmf, Derived&& ref, Args&&... args)
      noexcept(noexcept((::RESULT_NS_IMPL::detail::forward<Derived>(ref).*pmf)(::RESULT_NS_IMPL::detail::forward<Args>(args)...)))
      -> decltype((::RESULT_NS_IMPL::detail::forward<Derived>(ref).*pmf)(::RESULT_NS_IMPL::detail::forward<Args>(args)...))
    {
      return (RESULT_NS_IMPL::detail::forward<Derived>(ref).*pmf)(RESULT_NS_IMPL::detail::forward<Args>(args)...);
    }

    template <typename Base, typename T, typename RefWrap, typename... Args,
              typename = typename std::enable_if<
                std::is_function<T>::value &&
                is_reference_wrapper<typename std::decay<RefWrap>::type>::value
              >::type>
    inline RESULT_INLINE_VISIBILITY constexpr
    auto invoke(T Base::*pmf, RefWrap&& ref, Args&&... args)
      noexcept(noexcept((ref.get().*pmf)(std::forward<Args>(args)...)))
      -> decltype((ref.get().*pmf)(RESULT_NS_IMPL::detail::forward<Args>(args)...))
    {
      return (ref.get().*pmf)(RESULT_NS_IMPL::detail::forward<Args>(args)...);
    }

    template <typename Base, typename T, typename Pointer, typename... Args,
              typename = typename std::enable_if<
                std::is_function<T>::value &&
                !is_reference_wrapper<typename std::decay<Pointer>::type>::value &&
                !std::is_base_of<Base, typename std::decay<Pointer>::type>::value
              >::type>
    inline RESULT_INLINE_VISIBILITY constexpr
    auto invoke(T Base::*pmf, Pointer&& ptr, Args&&... args)
      noexcept(noexcept(((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...)))
      -> decltype(((*RESULT_NS_IMPL::detail::forward<Pointer>(ptr)).*pmf)(RESULT_NS_IMPL::detail::forward<Args>(args)...))
    {
      return ((*RESULT_NS_IMPL::detail::forward<Pointer>(ptr)).*pmf)(RESULT_NS_IMPL::detail::forward<Args>(args)...);
    }

    template <typename Base, typename T, typename Derived,
              typename = typename std::enable_if<
                !std::is_function<T>::value &&
                std::is_base_of<Base, typename std::decay<Derived>::type>::value
              >::type>
    inline RESULT_INLINE_VISIBILITY constexpr
    auto invoke(T Base::*pmd, Derived&& ref)
      noexcept(noexcept(std::forward<Derived>(ref).*pmd))
      -> decltype(RESULT_NS_IMPL::detail::forward<Derived>(ref).*pmd)
    {
      return RESULT_NS_IMPL::detail::forward<Derived>(ref).*pmd;
    }

    template <typename Base, typename T, typename RefWrap,
              typename = typename std::enable_if<
                !std::is_function<T>::value &&
                is_reference_wrapper<typename std::decay<RefWrap>::type>::value
              >::type>
    inline RESULT_INLINE_VISIBILITY constexpr
    auto invoke(T Base::*pmd, RefWrap&& ref)
      noexcept(noexcept(ref.get().*pmd))
      -> decltype(ref.get().*pmd)
    {
      return ref.get().*pmd;
    }

    template <typename Base, typename T, typename Pointer,
              typename = typename std::enable_if<
                !std::is_function<T>::value &&
                !is_reference_wrapper<typename std::decay<Pointer>::type>::value &&
                !std::is_base_of<Base, typename std::decay<Pointer>::type>::value
              >::type>
    inline RESULT_INLINE_VISIBILITY constexpr
    auto invoke(T Base::*pmd, Pointer&& ptr)
      noexcept(noexcept((*std::forward<Pointer>(ptr)).*pmd))
      -> decltype((*RESULT_NS_IMPL::detail::forward<Pointer>(ptr)).*pmd)
    {
      return (*RESULT_NS_IMPL::detail::forward<Pointer>(ptr)).*pmd;
    }

    template <typename F, typename... Args,
              typename = typename std::enable_if<!std::is_member_pointer<typename std::decay<F>::type>::value>::type>
    inline RESULT_INLINE_VISIBILITY constexpr
    auto invoke(F&& f, Args&&... args)
        noexcept(noexcept(std::forward<F>(f)(std::forward<Args>(args)...)))
      -> decltype(RESULT_NS_IMPL::detail::forward<F>(f)(RESULT_NS_IMPL::detail::forward<Args>(args)...))
    {
      return RESULT_NS_IMPL::detail::forward<F>(f)(RESULT_NS_IMPL::detail::forward<Args>(args)...);
    }

    template<typename Fn, typename...Args>
    struct is_invocable
    {
      template <typename Fn2, typename...Args2>
      static auto test( Fn2&&, Args2&&... )
        -> decltype(invoke(std::declval<Fn2>(), std::declval<Args2>()...), std::true_type{});

      static auto test(...)
        -> std::false_type;

      using type = decltype(test(std::declval<Fn>(), std::declval<Args>()...));
      static constexpr bool value = type::value;
    };

    template <bool B, typename Fn, typename...Args>
    struct invoke_result_impl {
      using type = decltype(RESULT_NS_IMPL::detail::invoke(std::declval<Fn>(), std::declval<Args>()...));
    };
    template <typename Fn, typename...Args>
    struct invoke_result_impl<false, Fn, Args...>{};

    template <typename Fn, typename...Args>
    struct invoke_result
      : invoke_result_impl<is_invocable<Fn,Args...>::value, Fn, Args...>{};

    template <typename Fn, typename...Args>
    using invoke_result_t = typename invoke_result<Fn, Args...>::type;
#endif
  }

  //===========================================================================
  // struct : in_place_t
  //===========================================================================

#if __cplusplus >= 201703L
  using std::in_place_t;
  using std::in_place;
#else
  /// \brief A structure for representing in-place construction
  struct in_place_t
  {
    explicit in_place_t() = default;
  };
  RESULT_CPP17_INLINE constexpr auto in_place = in_place_t{};
#endif

  //===========================================================================
  // struct : in_place_t
  //===========================================================================

  /// \brief A structure for representing in-place construction of an error type
  struct in_place_error_t
  {
    explicit in_place_error_t() = default;
  };

  RESULT_CPP17_INLINE constexpr auto in_place_error = in_place_error_t{};

  //===========================================================================
  // forward-declarations
  //===========================================================================

  template <typename>
  class failure;

  template <typename, typename>
  class result;

  template <typename>
  class bad_result_access;

  //===========================================================================
  // traits
  //===========================================================================

  template <typename T>
  struct is_failure : std::false_type{};
  template <typename E>
  struct is_failure<failure<E>> : std::true_type{};

  template <typename T>
  struct is_result : std::false_type{};
  template <typename T, typename E>
  struct is_result<result<T,E>> : std::true_type{};

  //===========================================================================
  // trait : detail::wrapped_result_type
  //===========================================================================

  namespace detail {

    template <typename T>
    using wrapped_result_type = typename std::conditional<
      std::is_lvalue_reference<T>::value,
      std::reference_wrapper<
        typename std::remove_reference<T>::type
      >,
      typename std::remove_const<T>::type
    >::type;

  } // namespace detail

#if !defined(RESULT_DISABLE_EXCEPTIONS)

  //===========================================================================
  // class : bad_result_access<E>
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief An exception thrown when result::value is accessed without
  ///        a contained value
  /////////////////////////////////////////////////////////////////////////////
  template <typename E>
  class bad_result_access : public std::logic_error
  {
    //-------------------------------------------------------------------------
    // Constructor / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs this exception using the underlying error type for
    ///        the error type
    ///
    /// \param error the underlying error
    template <typename E2,
              typename = typename std::enable_if<std::is_constructible<E,E2>::value>::type>
    explicit bad_result_access(E2&& error);

    /// \{
    /// \brief Constructs this exception using the underlying error type for
    ///        the error and a message
    ///
    /// \param what_arg the message for the failure
    /// \param error the underlying error
    template <typename E2,
              typename = typename std::enable_if<std::is_constructible<E,E2>::value>::type>
    bad_result_access(const char* what_arg, E2&& error);
    template <typename E2,
              typename = typename std::enable_if<std::is_constructible<E,E2>::value>::type>
    bad_result_access(const std::string& what_arg, E2&& error);
    /// \}

    bad_result_access(const bad_result_access& other) = default;
    bad_result_access(bad_result_access&& other) = default;

    //-------------------------------------------------------------------------

    auto operator=(const bad_result_access& other) -> bad_result_access& = default;
    auto operator=(bad_result_access&& other) -> bad_result_access& = default;

    /// \{
    /// \brief Gets the underlying error
    ///
    /// \return the error
    auto error() & noexcept -> E&;
    auto error() && noexcept -> E&&;
    auto error() const & noexcept -> const E&;
    auto error() const && noexcept -> const E&&;
    /// \}

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    E m_error;
  };

#endif

  namespace detail {

    template <typename E, typename E2>
    using failure_is_value_convertible = std::integral_constant<bool,(
      std::is_constructible<E, E2&&>::value &&
      !std::is_same<typename std::decay<E2>::type, in_place_t>::value &&
      !is_failure<typename std::decay<E2>::type>::value &&
      !is_result<typename std::decay<E2>::type>::value
    )>;

    template <typename E, typename E2>
    using failure_is_explicit_value_convertible = std::integral_constant<bool,(
      failure_is_value_convertible<E, E2>::value &&
      !std::is_convertible<E2, E>::value
    )>;

    template <typename E, typename E2>
    using failure_is_implicit_value_convertible = std::integral_constant<bool,(
      failure_is_value_convertible<E, E2>::value &&
      std::is_convertible<E2, E>::value
    )>;

    template <typename E, typename E2>
    using failure_is_value_assignable = std::integral_constant<bool,(
      !is_result<typename std::decay<E2>::type>::value &&
      !is_failure<typename std::decay<E2>::type>::value &&
      std::is_assignable<wrapped_result_type<E>&,E2>::value
    )>;

  } // namespace detail

  //===========================================================================
  // class : failure_type
  //===========================================================================

  //////////////////////////////////////////////////////////////////////////////
  /// \brief A semantic type used for distinguishing failure values in an
  ///        API that returns result types
  ///
  /// \tparam E the error type
  //////////////////////////////////////////////////////////////////////////////
  template <typename E>
  class failure
  {
    static_assert(
      !is_result<typename std::decay<E>::type>::value,
      "A (possibly CV-qualified) result 'E' type is ill-formed."
    );
    static_assert(
      !is_failure<typename std::decay<E>::type>::value,
      "A (possibly CV-qualified) failure 'E' type is ill-formed."
    );
    static_assert(
      !std::is_void<typename std::decay<E>::type>::value,
      "A (possibly CV-qualified) 'void' 'E' type is ill-formed."
    );
    static_assert(
      !std::is_rvalue_reference<E>::value,
      "rvalue references for 'E' type is ill-formed. "
      "Only lvalue references are valid."
    );

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using error_type = E;

    //-------------------------------------------------------------------------
    // Constructors / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a failure via default construction
    failure() = default;

    /// \brief Constructs a failure by delegating construction to the
    ///        underlying constructor
    ///
    /// \param args the arguments to forward to E's constructor
    template <typename...Args,
              typename = typename std::enable_if<std::is_constructible<E,Args...>::value>::type>
    constexpr failure(in_place_t, Args&&...args)
      noexcept(std::is_nothrow_constructible<E, Args...>::value);

    /// \brief Constructs a failure by delegating construction to the
    ///        underlying constructor
    ///
    /// \param ilist the initializer list
    /// \param args the arguments to forward to E's constructor
    template <typename U, typename...Args,
              typename = typename std::enable_if<std::is_constructible<E,std::initializer_list<U>,Args...>::value>::type>
    constexpr failure(in_place_t, std::initializer_list<U> ilist, Args&&...args)
      noexcept(std::is_nothrow_constructible<E, std::initializer_list<U>, Args...>::value);

    /// \{
    /// \brief Constructs a failure from the given error
    ///
    /// \param error the error to create a failure from
    template <typename E2,
              typename std::enable_if<detail::failure_is_implicit_value_convertible<E,E2>::value,int>::type = 0>
    constexpr failure(E2&& error)
      noexcept(std::is_nothrow_constructible<E,E2>::value);
    template <typename E2,
              typename std::enable_if<detail::failure_is_explicit_value_convertible<E,E2>::value,int>::type = 0>
    constexpr explicit failure(E2&& error)
      noexcept(std::is_nothrow_constructible<E,E2>::value);
    /// \}

    /// \brief Constructs this failure by copying the contents of an existing
    ///        one
    ///
    /// \param other the other failure to copy
    /* implicit */ failure(const failure& other) = default;

    /// \brief Constructs this failure by moving the contents of an existing
    ///        one
    ///
    /// \param other the other failure to move
    /* implicit */ failure(failure&& other) = default;

    /// \brief Constructs this failure by copy-converting \p other
    ///
    /// \param other the other failure to copy
    template <typename E2,
              typename = typename std::enable_if<std::is_constructible<E,const E2&>::value>::type>
    constexpr /* implicit */ failure(const failure<E2>& other)
      noexcept(std::is_nothrow_constructible<E,const E2&>::value);

    /// \brief Constructs this failure by move-converting \p other
    ///
    /// \param other the other failure to copy
    template <typename E2,
              typename = typename std::enable_if<std::is_constructible<E,E2&&>::value>::type>
    constexpr /* implicit */ failure(failure<E2>&& other)
      noexcept(std::is_nothrow_constructible<E,E2&&>::value);

    //--------------------------------------------------------------------------

    /// \brief Assigns the value of \p error to this failure through
    ///        move-assignment
    ///
    /// \param error the value to assign
    /// \return reference to `(*this)`
    template <typename E2,
              typename = typename std::enable_if<detail::failure_is_value_assignable<E,E2>::value>::type>
    RESULT_CPP14_CONSTEXPR
    auto operator=(E2&& error)
      noexcept(std::is_nothrow_assignable<E,E2>::value || std::is_lvalue_reference<E>::value) -> failure&;

    /// \brief Assigns the contents of \p other to this by copy-assignment
    ///
    /// \param other the other failure to copy
    /// \return reference to `(*this)`
    auto operator=(const failure& other) -> failure& = default;

    /// \brief Assigns the contents of \p other to this by move-assignment
    ///
    /// \param other the other failure to move
    /// \return reference to `(*this)`
    auto operator=(failure&& other) -> failure& = default;

    /// \brief Assigns the contents of \p other to this by copy conversion
    ///
    /// \param other the other failure to copy-convert
    /// \return reference to `(*this)`
    template <typename E2,
              typename = typename std::enable_if<std::is_assignable<E&,const E2&>::value>::type>
    RESULT_CPP14_CONSTEXPR
    auto operator=(const failure<E2>& other)
      noexcept(std::is_nothrow_assignable<E,const E2&>::value) -> failure&;

    /// \brief Assigns the contents of \p other to this by move conversion
    ///
    /// \param other the other failure to move-convert
    /// \return reference to `(*this)`
    template <typename E2,
              typename = typename std::enable_if<std::is_assignable<E&,E2&&>::value>::type>
    RESULT_CPP14_CONSTEXPR
    auto operator=(failure<E2>&& other)
      noexcept(std::is_nothrow_assignable<E,E2&&>::value) -> failure&;

    //--------------------------------------------------------------------------
    // Observers
    //--------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Gets the underlying error
    ///
    /// \return the underlying error
    RESULT_CPP14_CONSTEXPR
    auto error() & noexcept
      -> typename std::add_lvalue_reference<E>::type;
    RESULT_CPP14_CONSTEXPR
    auto error() && noexcept
      -> typename std::add_rvalue_reference<E>::type;
    constexpr auto error() const & noexcept
      -> typename std::add_lvalue_reference<typename std::add_const<E>::type>::type;
    constexpr auto error() const && noexcept
      -> typename std::add_rvalue_reference<typename std::add_const<E>::type>::type;
    /// \}

    //-------------------------------------------------------------------------
    // Private Member Types
    //-------------------------------------------------------------------------
  private:

    using underlying_type = detail::wrapped_result_type<E>;

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    underlying_type m_failure;
  };

#if __cplusplus >= 201703L
  template <typename T>
  failure(std::reference_wrapper<T>) -> failure<T&>;

  template <typename T>
  failure(T&&) -> failure<typename std::decay<T>::type>;
#endif

  //===========================================================================
  // non-member functions : class : failure
  //===========================================================================

  //---------------------------------------------------------------------------
  // Comparison
  //---------------------------------------------------------------------------

  template <typename E1, typename E2>
  constexpr auto operator==(const failure<E1>& lhs,
                            const failure<E2>& rhs) noexcept -> bool;
  template <typename E1, typename E2>
  constexpr auto operator!=(const failure<E1>& lhs,
                            const failure<E2>& rhs) noexcept -> bool;
  template <typename E1, typename E2>
  constexpr auto operator<(const failure<E1>& lhs,
                           const failure<E2>& rhs) noexcept -> bool;
  template <typename E1, typename E2>
  constexpr auto operator>(const failure<E1>& lhs,
                           const failure<E2>& rhs) noexcept -> bool;
  template <typename E1, typename E2>
  constexpr auto operator<=(const failure<E1>& lhs,
                            const failure<E2>& rhs) noexcept -> bool;
  template <typename E1, typename E2>
  constexpr auto operator>=(const failure<E1>& lhs,
                            const failure<E2>& rhs) noexcept -> bool;

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \brief Deduces and constructs a failure type from \p e
  ///
  /// \param e the failure value
  /// \return a constructed failure value
  template <typename E>
  RESULT_WARN_UNUSED
  constexpr auto fail(E&& e)
    noexcept(std::is_nothrow_constructible<typename std::decay<E>::type,E>::value)
    -> failure<typename std::decay<E>::type>;

  /// \brief Deduces a failure reference from a reverence_wrapper
  ///
  /// \param e the failure value
  /// \return a constructed failure reference
  template <typename E>
  RESULT_WARN_UNUSED
  constexpr auto fail(std::reference_wrapper<E> e)
    noexcept -> failure<E&>;

  /// \brief Constructs a failure type from a series of arguments
  ///
  /// \tparam E the failure type
  /// \param args the arguments to forward to E's constructor
  /// \return a constructed failure type
  template <typename E, typename...Args,
            typename = typename std::enable_if<std::is_constructible<E,Args...>::value>::type>
  RESULT_WARN_UNUSED
  constexpr auto fail(Args&&...args)
    noexcept(std::is_nothrow_constructible<E, Args...>::value)
    -> failure<E>;

  /// \brief Constructs a failure type from an initializer list and series of
  ///        arguments
  ///
  /// \tparam E the failure type
  /// \param args the arguments to forward to E's constructor
  /// \return a constructed failure type
  template <typename E, typename U, typename...Args,
            typename = typename std::enable_if<std::is_constructible<E,std::initializer_list<U>,Args...>::value>::type>
  RESULT_WARN_UNUSED
  constexpr auto fail(std::initializer_list<U> ilist, Args&&...args)
    noexcept(std::is_nothrow_constructible<E, std::initializer_list<U>, Args...>::value)
    -> failure<E>;

  /// \brief Swaps the contents of two failure values
  ///
  /// \param lhs the left failure
  /// \param rhs the right failure
  template <typename E>
  auto swap(failure<E>& lhs, failure<E>& rhs)
#if __cplusplus >= 201703L
    noexcept(std::is_nothrow_swappable<E>::value) -> void;
#else
    noexcept(std::is_nothrow_move_constructible<E>::value) -> void;
#endif

  namespace detail {

    //=========================================================================
    // class : unit
    //=========================================================================

    /// \brief A standalone monostate object (effectively std::monostate). This
    ///        exists to allow for `void` specializations
    struct unit {};

    //=========================================================================
    // non-member functions : class : unit
    //=========================================================================

    constexpr auto operator==(unit, unit) noexcept -> bool { return true; }
    constexpr auto operator!=(unit, unit) noexcept -> bool { return false; }
    constexpr auto operator<(unit, unit) noexcept -> bool { return false; }
    constexpr auto operator>(unit, unit) noexcept -> bool { return false; }
    constexpr auto operator<=(unit, unit) noexcept -> bool { return true; }
    constexpr auto operator>=(unit, unit) noexcept -> bool { return true; }

    //=========================================================================
    // class : detail::result_union<T, E, IsTrivial>
    //=========================================================================

    ///////////////////////////////////////////////////////////////////////////
    /// \brief A basic utility that acts as a union containing the T and E
    ///        types
    ///
    /// This is specialized on the case that both T and E are trivial, in which
    /// case `result_union` is also trivial
    ///
    /// \tparam T the value type result to be returned
    /// \tparam E the error type returned on failure
    /// \tparam IsTrivial Whether or not both T and E are trivial
    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename E,
              bool IsTrivial = std::is_trivially_destructible<T>::value &&
                               std::is_trivially_destructible<E>::value>
    struct result_union
    {
      //-----------------------------------------------------------------------
      // Public Member Types
      //-----------------------------------------------------------------------

      using underlying_value_type = wrapped_result_type<T>;
      using underlying_error_type = E;

      //-----------------------------------------------------------------------
      // Constructors / Assignment
      //-----------------------------------------------------------------------

      /// \brief Constructs an empty object
      ///
      /// This is for use with conversion constructors, since it allows a
      /// temporary unused object to be set
      result_union(unit) noexcept;

      /// \brief Constructs the underlying value from the specified \p args
      ///
      /// \param args the arguments to forward to T's constructor
      template <typename...Args>
      constexpr result_union(in_place_t, Args&&...args)
        noexcept(std::is_nothrow_constructible<T, Args...>::value);

      /// \brief Constructs the underlying error from the specified \p args
      ///
      /// \param args the arguments to forward to E's constructor
      template <typename...Args>
      constexpr result_union(in_place_error_t, Args&&...args)
        noexcept(std::is_nothrow_constructible<E, Args...>::value);

      result_union(const result_union&) = default;
      result_union(result_union&&) = default;

      //-----------------------------------------------------------------------

      auto operator=(const result_union&) -> result_union& = default;
      auto operator=(result_union&&) -> result_union& = default;

      //-----------------------------------------------------------------------
      // Modifiers
      //-----------------------------------------------------------------------

      /// \brief A no-op for trivial types
      auto destroy() const noexcept -> void;

      //-----------------------------------------------------------------------
      // Public Members
      //-----------------------------------------------------------------------

      union {
        underlying_value_type m_value;
        underlying_error_type m_error;
        unit m_empty;
      };
      bool m_has_value;
    };

    //-------------------------------------------------------------------------

    template <typename T, typename E>
    struct result_union<T, E, false>
    {
      //-----------------------------------------------------------------------
      // Public Member Types
      //-----------------------------------------------------------------------

      using underlying_value_type = wrapped_result_type<T>;
      using underlying_error_type = E;

      //-----------------------------------------------------------------------
      // Constructors / Assignment / Destructor
      //-----------------------------------------------------------------------

      /// \brief Constructs an empty object
      ///
      /// This is for use with conversion constructors, since it allows a
      /// temporary unused object to be set
      result_union(unit) noexcept;

      /// \brief Constructs the underlying value from the specified \p args
      ///
      /// \param args the arguments to forward to T's constructor
      template <typename...Args>
      constexpr result_union(in_place_t, Args&&...args)
        noexcept(std::is_nothrow_constructible<T, Args...>::value);

      /// \brief Constructs the underlying error from the specified \p args
      ///
      /// \param args the arguments to forward to E's constructor
      template <typename...Args>
      constexpr result_union(in_place_error_t, Args&&...args)
        noexcept(std::is_nothrow_constructible<E, Args...>::value);

      result_union(const result_union&) = default;
      result_union(result_union&&) = default;

      //-----------------------------------------------------------------------

      /// \brief Destroys the underlying stored object
      ~result_union()
        noexcept(std::is_nothrow_destructible<T>::value &&
                 std::is_nothrow_destructible<E>::value);

      //-----------------------------------------------------------------------

      auto operator=(const result_union&) -> result_union& = default;
      auto operator=(result_union&&) -> result_union& = default;

      //-----------------------------------------------------------------------
      // Modifiers
      //-----------------------------------------------------------------------

      /// \brief Destroys the underlying stored object
      auto destroy() -> void;

      //-----------------------------------------------------------------------
      // Public Members
      //-----------------------------------------------------------------------

      union {
        underlying_value_type m_value;
        underlying_error_type m_error;
        unit m_empty;
      };
      bool m_has_value;
    };

    //=========================================================================
    // class : result_construct_base<T, E>
    //=========================================================================

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Base class of assignment to enable construction and assignment
    ///
    /// This class is used with several pieces of construction to ensure
    /// trivial constructibility and assignability:
    ///
    /// * `result_trivial_copy_ctor_base`
    /// * `result_trivial_move_ctor_base`
    /// * `result_copy_assign_base`
    /// * `result_move_assign_base`
    ///
    /// \tparam T the value type
    /// \tparam E the error type
    ///////////////////////////////////////////////////////////////////////////
    template <typename T, typename E>
    struct result_construct_base
    {
      //-----------------------------------------------------------------------
      // Constructors / Assignment
      //-----------------------------------------------------------------------

      /// \brief Constructs an empty object
      ///
      /// This is for use with conversion constructors, since it allows a
      /// temporary unused object to be set
      result_construct_base(unit) noexcept;

      /// \brief Constructs the underlying value from the specified \p args
      ///
      /// \param args the arguments to forward to T's constructor
      template <typename...Args>
      constexpr result_construct_base(in_place_t, Args&&...args)
        noexcept(std::is_nothrow_constructible<T, Args...>::value);

      /// \brief Constructs the underlying error from the specified \p args
      ///
      /// \param args the arguments to forward to E's constructor
      template <typename...Args>
      constexpr result_construct_base(in_place_error_t, Args&&...args)
        noexcept(std::is_nothrow_constructible<E, Args...>::value);

      result_construct_base(const result_construct_base&) = default;
      result_construct_base(result_construct_base&&) = default;

      auto operator=(const result_construct_base&) -> result_construct_base& = default;
      auto operator=(result_construct_base&&) -> result_construct_base& = default;

      //-----------------------------------------------------------------------
      // Construction / Assignment
      //-----------------------------------------------------------------------

      /// \brief Constructs the value type from \p args
      ///
      /// \note This is an implementation detail only meant to be used during
      ///       construction
      ///
      /// \pre there is no contained value or error at the time of construction
      ///
      /// \param args the arguments to forward to T's constructor
      template <typename...Args>
      auto construct_value(Args&&...args)
        noexcept(std::is_nothrow_constructible<T,Args...>::value) -> void;

      /// \brief Constructs the error type from \p args
      ///
      /// \note This is an implementation detail only meant to be used during
      ///       construction
      ///
      /// \pre there is no contained value or error at the time of construction
      ///
      /// \param args the arguments to forward to E's constructor
      template <typename...Args>
      auto construct_error(Args&&...args)
        noexcept(std::is_nothrow_constructible<E,Args...>::value) -> void;

      /// \brief Constructs the underlying error from the \p other result
      ///
      /// If \p other contains a value, then the T type will be
      /// default-constructed.
      ///
      /// \note This is an implementation detail only meant to be used during
      ///       construction of `result<void, E>` types
      ///
      /// \pre there is no contained value or error at the time of construction
      ///
      /// \param other the other result to construct
      template <typename Result>
      auto construct_error_from_result(Result&& other) -> void;

      /// \brief Constructs the underlying type from a result object
      ///
      /// \note This is an implementation detail only meant to be used during
      ///       construction
      ///
      /// \pre there is no contained value or error at the time of construction
      ///
      /// \param other the other result to construct
      template <typename Result>
      auto construct_from_result(Result&& other) -> void;

      //-----------------------------------------------------------------------

      template <typename Value>
      auto assign_value(Value&& value)
        noexcept(std::is_nothrow_assignable<T, Value>::value) -> void;

      template <typename Error>
      auto assign_error(Error&& error)
        noexcept(std::is_nothrow_assignable<E, Error>::value) -> void;

      template <typename Result>
      auto assign_from_result(Result&& other) -> void;

      //-----------------------------------------------------------------------

      template <typename ReferenceWrapper>
      auto construct_value_from_result_impl(std::true_type, ReferenceWrapper&& reference)
        noexcept -> void;

      template <typename Value>
      auto construct_value_from_result_impl(std::false_type, Value&& value)
        noexcept(std::is_nothrow_constructible<T,Value>::value) -> void;

      template <typename Result>
      auto assign_value_from_result_impl(std::true_type, Result&& other) -> void;

      template <typename Result>
      auto assign_value_from_result_impl(std::false_type, Result&& other) -> void;

      //-----------------------------------------------------------------------
      // Public Members
      //-----------------------------------------------------------------------

      using storage_type = result_union<T, E>;

      storage_type storage;
    };

    //=========================================================================
    // class : result_trivial_copy_ctor_base
    //=========================================================================

    template <typename T, typename E>
    struct result_trivial_copy_ctor_base_impl : result_construct_base<T,E>
    {
      using base_type = result_construct_base<T,E>;
      using base_type::base_type;

      result_trivial_copy_ctor_base_impl(const result_trivial_copy_ctor_base_impl& other)
        noexcept(std::is_nothrow_copy_constructible<T>::value &&
                 std::is_nothrow_copy_constructible<E>::value);
      result_trivial_copy_ctor_base_impl(result_trivial_copy_ctor_base_impl&& other) = default;

      auto operator=(const result_trivial_copy_ctor_base_impl& other) -> result_trivial_copy_ctor_base_impl& = default;
      auto operator=(result_trivial_copy_ctor_base_impl&& other) -> result_trivial_copy_ctor_base_impl& = default;
    };

    template <bool Condition, typename Base>
    using conditionally_nest_type = typename std::conditional<
      Condition,
      typename Base::base_type,
      Base
    >::type;

    template <typename T, typename E>
    using result_trivial_copy_ctor_base = conditionally_nest_type<
      std::is_trivially_copy_constructible<T>::value &&
      std::is_trivially_copy_constructible<E>::value,
      result_trivial_copy_ctor_base_impl<T,E>
    >;

    //=========================================================================
    // class : result_trivial_move_ctor_base
    //=========================================================================

    template <typename T, typename E>
    struct result_trivial_move_ctor_base_impl : result_trivial_copy_ctor_base<T,E>
    {
      using base_type = result_trivial_copy_ctor_base<T,E>;
      using base_type::base_type;

      result_trivial_move_ctor_base_impl(const result_trivial_move_ctor_base_impl& other) = default;
      result_trivial_move_ctor_base_impl(result_trivial_move_ctor_base_impl&& other)
        noexcept(std::is_nothrow_move_constructible<T>::value &&
                 std::is_nothrow_move_constructible<E>::value);

      auto operator=(const result_trivial_move_ctor_base_impl& other) -> result_trivial_move_ctor_base_impl& = default;
      auto operator=(result_trivial_move_ctor_base_impl&& other) -> result_trivial_move_ctor_base_impl& = default;
    };

    template <typename T, typename E>
    using result_trivial_move_ctor_base = conditionally_nest_type<
      std::is_trivially_move_constructible<T>::value &&
      std::is_trivially_move_constructible<E>::value,
      result_trivial_move_ctor_base_impl<T,E>
    >;

    //=========================================================================
    // class : result_trivial_copy_assign_base
    //=========================================================================

    template <typename T, typename E>
    struct result_trivial_copy_assign_base_impl
      : result_trivial_move_ctor_base<T, E>
    {
      using base_type = result_trivial_move_ctor_base<T,E>;
      using base_type::base_type;

      result_trivial_copy_assign_base_impl(const result_trivial_copy_assign_base_impl& other) = default;
      result_trivial_copy_assign_base_impl(result_trivial_copy_assign_base_impl&& other) = default;

      auto operator=(const result_trivial_copy_assign_base_impl& other)
        noexcept(std::is_nothrow_copy_constructible<T>::value &&
                 std::is_nothrow_copy_constructible<E>::value &&
                 std::is_nothrow_copy_assignable<T>::value &&
                 std::is_nothrow_copy_assignable<E>::value)
        -> result_trivial_copy_assign_base_impl&;
      auto operator=(result_trivial_copy_assign_base_impl&& other)
        -> result_trivial_copy_assign_base_impl& = default;
    };

    template <typename T, typename E>
    using result_trivial_copy_assign_base = conditionally_nest_type<
      std::is_trivially_copy_constructible<T>::value &&
      std::is_trivially_copy_constructible<E>::value &&
      std::is_trivially_copy_assignable<T>::value &&
      std::is_trivially_copy_assignable<E>::value &&
      std::is_trivially_destructible<T>::value &&
      std::is_trivially_destructible<E>::value,
      result_trivial_copy_assign_base_impl<T,E>
    >;

    //=========================================================================
    // class : result_trivial_move_assign_base
    //=========================================================================

    template <typename T, typename E>
    struct result_trivial_move_assign_base_impl
      : result_trivial_copy_assign_base<T, E>
    {
      using base_type = result_trivial_copy_assign_base<T,E>;
      using base_type::base_type;

      result_trivial_move_assign_base_impl(const result_trivial_move_assign_base_impl& other) = default;
      result_trivial_move_assign_base_impl(result_trivial_move_assign_base_impl&& other) = default;

      auto operator=(const result_trivial_move_assign_base_impl& other)
        -> result_trivial_move_assign_base_impl& = default;
      auto operator=(result_trivial_move_assign_base_impl&& other)
        noexcept(std::is_nothrow_move_constructible<T>::value &&
                 std::is_nothrow_move_constructible<E>::value &&
                 std::is_nothrow_move_assignable<T>::value &&
                 std::is_nothrow_move_assignable<E>::value)
        -> result_trivial_move_assign_base_impl&;
    };

    template <typename T, typename E>
    using result_trivial_move_assign_base = conditionally_nest_type<
      std::is_trivially_move_constructible<T>::value &&
      std::is_trivially_move_constructible<E>::value &&
      std::is_trivially_move_assignable<T>::value &&
      std::is_trivially_move_assignable<E>::value &&
      std::is_trivially_destructible<T>::value &&
      std::is_trivially_destructible<E>::value,
      result_trivial_move_assign_base_impl<T,E>
    >;

    //=========================================================================
    // class : disable_copy_ctor
    //=========================================================================

    template <typename T, typename E>
    struct disable_copy_ctor : result_trivial_move_assign_base<T,E>
    {
      using base_type = result_trivial_move_assign_base<T,E>;
      using base_type::base_type;

      disable_copy_ctor(const disable_copy_ctor& other) = delete;
      disable_copy_ctor(disable_copy_ctor&& other) = default;

      auto operator=(const disable_copy_ctor& other)
        -> disable_copy_ctor& = default;
      auto operator=(disable_copy_ctor&& other)
        -> disable_copy_ctor& = default;
    };

    template <typename T, typename E>
    using result_copy_ctor_base = conditionally_nest_type<
      std::is_copy_constructible<T>::value &&
      std::is_copy_constructible<E>::value,
      disable_copy_ctor<T,E>
    >;

    //=========================================================================
    // class : disable_move_ctor
    //=========================================================================

    template <typename T, typename E>
    struct disable_move_ctor : result_copy_ctor_base<T,E>
    {
      using base_type = result_copy_ctor_base<T,E>;
      using base_type::base_type;

      disable_move_ctor(const disable_move_ctor& other) = default;
      disable_move_ctor(disable_move_ctor&& other) = delete;

      auto operator=(const disable_move_ctor& other)
        -> disable_move_ctor& = default;
      auto operator=(disable_move_ctor&& other)
        -> disable_move_ctor& = default;
    };

    template <typename T, typename E>
    using result_move_ctor_base = conditionally_nest_type<
      std::is_move_constructible<T>::value &&
      std::is_move_constructible<E>::value,
      disable_move_ctor<T,E>
    >;

    //=========================================================================
    // class : disable_move_assignment
    //=========================================================================

    template <typename T, typename E>
    struct disable_move_assignment
      : result_move_ctor_base<T, E>
    {
      using base_type = result_move_ctor_base<T, E>;
      using base_type::base_type;

      disable_move_assignment(const disable_move_assignment& other) = default;
      disable_move_assignment(disable_move_assignment&& other) = default;

      auto operator=(const disable_move_assignment& other)
        -> disable_move_assignment& = delete;
      auto operator=(disable_move_assignment&& other)
        -> disable_move_assignment& = default;
    };

    template <typename T, typename E>
    using result_copy_assign_base = conditionally_nest_type<
      std::is_nothrow_copy_constructible<T>::value &&
      std::is_nothrow_copy_constructible<E>::value &&
      std::is_copy_assignable<wrapped_result_type<T>>::value &&
      std::is_copy_assignable<E>::value,
      disable_move_assignment<T,E>
    >;

    //=========================================================================
    // class : disable_copy_assignment
    //=========================================================================

    template <typename T, typename E>
    struct disable_copy_assignment
      : result_copy_assign_base<T, E>
    {
      using base_type = result_copy_assign_base<T, E>;
      using base_type::base_type;

      disable_copy_assignment(const disable_copy_assignment& other) = default;
      disable_copy_assignment(disable_copy_assignment&& other) = default;

      auto operator=(const disable_copy_assignment& other)
        -> disable_copy_assignment& = default;
      auto operator=(disable_copy_assignment&& other)
        -> disable_copy_assignment& = delete;
    };

    template <typename T, typename E>
    using result_move_assign_base = conditionally_nest_type<
      std::is_nothrow_move_constructible<T>::value &&
      std::is_nothrow_move_constructible<E>::value &&
      std::is_move_assignable<wrapped_result_type<T>>::value &&
      std::is_move_assignable<E>::value,
      disable_copy_assignment<T,E>
    >;

    //=========================================================================
    // alias : result_storage
    //=========================================================================

    template <typename T, typename E>
    using result_storage = result_move_assign_base<T, E>;

    //=========================================================================
    // traits : result
    //=========================================================================

    template <typename T1, typename E1, typename T2, typename E2>
    using result_is_convertible = std::integral_constant<bool,(
      // T1 constructible from result<T2,E2>
      std::is_constructible<T1, result<T2,E2>&>:: value ||
      std::is_constructible<T1, const result<T2,E2>&>:: value ||
      std::is_constructible<T1, result<T2,E2>&&>:: value ||
      std::is_constructible<T1, const result<T2,E2>&&>:: value ||

      // E1 constructible from result<T2,E2>
      std::is_constructible<E1, result<T2,E2>&>:: value ||
      std::is_constructible<E1, const result<T2,E2>&>:: value ||
      std::is_constructible<E1, result<T2,E2>&&>:: value ||
      std::is_constructible<E1, const result<T2,E2>&&>:: value ||

      // result<T2,E2> convertible to T1
      std::is_convertible<result<T2,E2>&, T1>:: value ||
      std::is_convertible<const result<T2,E2>&, T1>:: value ||
      std::is_convertible<result<T2,E2>&&, T1>::value ||
      std::is_convertible<const result<T2,E2>&&, T1>::value ||

      // result<T2,E2> convertible to E2
      std::is_convertible<result<T2,E2>&, E1>:: value ||
      std::is_convertible<const result<T2,E2>&, E1>:: value ||
      std::is_convertible<result<T2,E2>&&, E1>::value ||
      std::is_convertible<const result<T2,E2>&&, E1>::value
    )>;

    //-------------------------------------------------------------------------

    template <typename T1, typename E1, typename T2, typename E2>
    using result_is_copy_convertible = std::integral_constant<bool,(
      !result_is_convertible<T1,E1,T2,E2>::value &&
      std::is_constructible<T1, const T2&>::value &&
      std::is_constructible<E1, const E2&>::value
    )>;

    template <typename T1, typename E1, typename T2, typename E2>
    using result_is_implicit_copy_convertible = std::integral_constant<bool,(
      result_is_copy_convertible<T1,E1,T2,E2>::value &&
      std::is_convertible<const T2&, T1>::value &&
      std::is_convertible<const E2&, E1>::value
    )>;

    template <typename T1, typename E1, typename T2, typename E2>
    using result_is_explicit_copy_convertible = std::integral_constant<bool,(
      result_is_copy_convertible<T1,E1,T2,E2>::value &&
      (!std::is_convertible<const T2&, T1>::value ||
       !std::is_convertible<const E2&, E1>::value)
    )>;

    //-------------------------------------------------------------------------

    template <typename T1, typename E1, typename T2, typename E2>
    using result_is_move_convertible = std::integral_constant<bool,(
      !result_is_convertible<T1,E1,T2,E2>::value &&
      std::is_constructible<T1, T2&&>::value &&
      std::is_constructible<E1, E2&&>::value
    )>;

    template <typename T1, typename E1, typename T2, typename E2>
    using result_is_implicit_move_convertible = std::integral_constant<bool,(
      result_is_move_convertible<T1,E1,T2,E2>::value &&
      std::is_convertible<T2&&, T1>::value &&
      std::is_convertible<E2&&, E1>::value
    )>;

    template <typename T1, typename E1, typename T2, typename E2>
    using result_is_explicit_move_convertible = std::integral_constant<bool,(
      result_is_move_convertible<T1,E1,T2,E2>::value &&
      (!std::is_convertible<T2&&, T1>::value ||
       !std::is_convertible<E2&&, E1>::value)
    )>;

    //-------------------------------------------------------------------------

    template <typename T, typename U>
    using result_is_value_convertible = std::integral_constant<bool,(
      std::is_constructible<T, U&&>::value &&
      !std::is_same<typename std::decay<U>::type, in_place_t>::value &&
      !std::is_same<typename std::decay<U>::type, in_place_error_t>::value &&
      !is_result<typename std::decay<U>::type>::value
    )>;

    template <typename T, typename U>
    using result_is_explicit_value_convertible = std::integral_constant<bool,(
      result_is_value_convertible<T, U>::value &&
      !std::is_convertible<U&&, T>::value
    )>;

    template <typename T, typename U>
    using result_is_implicit_value_convertible = std::integral_constant<bool,(
      result_is_value_convertible<T, U>::value &&
      std::is_convertible<U&&, T>::value
    )>;

    //-------------------------------------------------------------------------

    template <typename T1, typename E1, typename T2, typename E2>
    using result_is_convert_assignable = std::integral_constant<bool,(
      result_is_convertible<T1,E1,T2,E2>::value &&

      std::is_assignable<T1&, result<T2,E2>&>::value &&
      std::is_assignable<T1&, const result<T2,E2>&>::value &&
      std::is_assignable<T1&, result<T2,E2>&&>::value &&
      std::is_assignable<T1&, const result<T2,E2>&&>::value &&

      std::is_assignable<E1&, result<T2,E2>&>::value &&
      std::is_assignable<E1&, const result<T2,E2>&>::value &&
      std::is_assignable<E1&, result<T2,E2>&&>::value &&
      std::is_assignable<E1&, const result<T2,E2>&&>::value
    )>;

    template <typename T1, typename E1, typename T2, typename E2>
    using result_is_copy_convert_assignable = std::integral_constant<bool,(
      !result_is_convert_assignable<T1,E1,T2,E2>::value &&

      std::is_nothrow_constructible<T1, const T2&>::value &&
      std::is_assignable<wrapped_result_type<T1>&, const T2&>::value &&
      std::is_nothrow_constructible<E1, const E2&>::value &&
      std::is_assignable<E1&, const E2&>::value
    )>;

    template <typename T1, typename E1, typename T2, typename E2>
    using result_is_move_convert_assignable = std::integral_constant<bool,(
      !result_is_convert_assignable<T1,E1,T2,E2>::value &&

      std::is_nothrow_constructible<T1, T2&&>::value &&
      std::is_assignable<T1&, T2&&>::value &&
      std::is_nothrow_constructible<E1, E2&&>::value &&
      std::is_assignable<E1&, E2&&>::value
    )>;

    //-------------------------------------------------------------------------

    template <typename T, typename U>
    using result_is_value_assignable = std::integral_constant<bool,(
      !is_result<typename std::decay<U>::type>::value &&
      !is_failure<typename std::decay<U>::type>::value &&
      std::is_nothrow_constructible<T,U>::value &&
      std::is_assignable<wrapped_result_type<T>&,U>::value &&
      (
        !std::is_same<typename std::decay<U>::type,typename std::decay<T>::type>::value ||
        !std::is_scalar<T>::value
      )
    )>;

    template <typename E, typename E2>
    using result_is_failure_assignable = std::integral_constant<bool,(
      std::is_nothrow_constructible<E,E2>::value &&
      std::is_assignable<E&,E2>::value
    )>;

    // Friending 'extract_error" below was causing some compilers to incorrectly
    // identify `exp.m_storage.m_error` as being an access violation despite the
    // friendship. Using a type name instead seems to be ubiquitous across
    // compilers
    struct result_error_extractor
    {
      template <typename T, typename E>
      static constexpr auto get(const result<T,E>& exp) noexcept -> const E&;
      template <typename T, typename E>
      static constexpr auto get(result<T,E>& exp) noexcept -> E&;
    };

    template <typename T, typename E>
    constexpr auto extract_error(const result<T,E>& exp) noexcept -> const E&;

    template <typename E>
    [[noreturn]]
    auto throw_bad_result_access(E&& error) -> void;

    template <typename String, typename E>
    [[noreturn]]
    auto throw_bad_result_access_message(String&& message, E&& error) -> void;

  } // namespace detail

  /////////////////////////////////////////////////////////////////////////////
  /// \brief The class template `result` manages result results from APIs,
  ///        while encoding possible failure conditions.
  ///
  /// A common use-case for result is the return value of a function that
  /// may fail. As opposed to other approaches, such as `std::pair<T,bool>`
  /// or `std::optional`, `result` more accurately conveys the intent of the
  /// user along with the failure condition to the caller. This effectively
  /// produces an orthogonal error handling mechanism that allows for exception
  /// safety while also allowing discrete testability of the return type.
  ///
  /// `result<T,E>` types may contain a `T` value, which signifies that an
  /// operation succeeded in producing the result value of type `T`. If an
  /// `result` does not contain a `T` value, it will always contain an `E`
  /// error condition instead.
  ///
  /// An `result<T,E>` can always be queried for a possible error case by
  /// calling the `error()` function, even if it contains a value.
  /// In the case that a `result<T,E>` contains a value object, this will
  /// simply return an `E` object constructed through default aggregate
  /// construction, as if through the expression `E{}`, which is assumed to be
  /// a "valid" (no-error) state for an `E` type.
  /// For example:
  ///
  /// * `std::error_code{}` produces a default-construct error-code, which is
  ///   the "no error" state,
  /// * integral (or enum) error codes produce a `0` value (no error), thanks to
  ///   zero-initialization,
  /// * `std::exception_ptr{}` produces a null-pointer,
  /// * `std::string{}` produces an empty string `""`,
  /// * etc.
  ///
  /// When a `result<T,E>` contains either a value or error, the storage for
  /// that object is guaranteed to be allocated as part of the result
  /// object's footprint, i.e. no dynamic memory allocation ever takes place.
  /// Thus, a result object models an object, not a pointer, even though the
  /// `operator*()` and `operator->()` are defined.
  ///
  /// When an object of type `result<T,E>` is contextually converted to
  /// `bool`, the conversion returns `true` if the object contains a value and
  /// `false` if it contains an error.
  ///
  /// `result` objects do not have a "valueless" state like `variant`s do.
  /// Once a `result` has been constructed with a value or error, the
  /// active underlying type can only be changed through assignment which may
  /// is only enabled if construction is guaranteed to be *non-throwing*. This
  /// ensures that a valueless state cannot occur naturally.
  ///
  /// Example Use:
  /// \code
  /// auto to_string(int x) -> result<std::string>
  /// {
  ///   try {
  ///     return std::stoi(x);
  ///   } catch (const std::invalid_argument&) {
  ///     return fail(std::errc::invalid_argument);
  ///   } catch (const std::std::out_of_range&) {
  ///     return fail(std::errc::result_out_of_range);
  ///   }
  /// }
  /// \endcode
  ///
  /// \note If using C++17 or above, `fail` can be replaced with
  ///       `failure{...}` thanks to CTAD.
  ///
  /// \tparam T the underlying value type
  /// \tparam E the underlying error type
  ///////////////////////////////////////////////////////////////////////////
  template <typename T, typename E>
  class RESULT_NODISCARD result
  {
    // Type requirements

    static_assert(
      !std::is_abstract<T>::value,
      "It is ill-formed for T to be abstract type"
    );
    static_assert(
      !std::is_same<typename std::decay<T>::type, in_place_t>::value,
      "It is ill-formed for T to be a (possibly CV-qualified) in_place_t type"
    );
    static_assert(
      !is_result<typename std::decay<T>::type>::value,
      "It is ill-formed for T to be a (possibly CV-qualified) 'result' type"
    );
    static_assert(
      !is_failure<typename std::decay<T>::type>::value,
      "It is ill-formed for T to be a (possibly CV-qualified) 'failure' type"
    );
    static_assert(
      !std::is_rvalue_reference<T>::value,
      "It is ill-formed for T to be an rvalue 'result type. "
      "Only lvalue references are valid."
    );

    static_assert(
      !std::is_abstract<E>::value,
      "It is ill-formed for E to be abstract type"
    );
    static_assert(
      !std::is_void<typename std::decay<E>::type>::value,
      "It is ill-formed for E to be (possibly CV-qualified) void type"
    );
    static_assert(
      !is_result<typename std::decay<E>::type>::value,
      "It is ill-formed for E to be a (possibly CV-qualified) 'result' type"
    );
    static_assert(
      !is_failure<typename std::decay<E>::type>::value,
      "It is ill-formed for E to be a (possibly CV-qualified) 'failure' type"
    );
    static_assert(
      !std::is_same<typename std::decay<E>::type, in_place_t>::value,
      "It is ill-formed for E to be a (possibly CV-qualified) in_place_t type"
    );
    static_assert(
      !std::is_reference<E>::value,
      "It is ill-formed for E to be a reference type. "
      "Only T types may be lvalue references"
    );

    // Friendship

    friend detail::result_error_extractor;

    template <typename T2, typename E2>
    friend class result;

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using value_type = T; ///< The value type of this result
    using error_type = E; ///< The error type of this result
    using failure_type = failure<E>; ///< The failure type

    template <typename U>
    using rebind = result<U,E>; ///< Rebinds the result type

    //-------------------------------------------------------------------------
    // Constructor / Destructor / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Default-constructs a result with the underlying value type
    ///        active
    ///
    /// This constructor is only enabled if `T` is default-constructible
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// assert(cpp::result<std::string,int>{} == std::string{});
    /// ```
    template <typename U=T,
              typename = typename std::enable_if<std::is_constructible<U>::value>::type>
    constexpr result()
      noexcept(std::is_nothrow_constructible<U>::value);

    /// \brief Copy constructs this result
    ///
    /// If \p other contains a value, initializes the contained value as if
    /// direct-initializing (but not direct-list-initializing) an object of
    /// type `T` with the expression *other.
    ///
    /// If other contains an error, constructs an object that contains a copy
    /// of that error.
    ///
    /// \note This constructor is defined as deleted if
    ///       `std::is_copy_constructible<T>::value` or
    ///       `std::is_copy_constructible<E>::value` is `false`
    ///
    /// \note This constructor is defined as trivial if both
    ///       `std::is_trivially_copy_constructible<T>::value` and
    ///       `std::is_trivially_copy_constructible<E>::value` are `true`
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// const auto r = cpp::result<int,int>{42};
    /// const auto s = r;
    ///
    /// assert(r == s);
    /// ```
    ///
    /// \param other the result to copy
    constexpr result(const result& other) = default;

    /// \brief Move constructs a result
    ///
    /// If other contains a value, initializes the contained value as if
    /// direct-initializing (but not direct-list-initializing) an object
    /// of type T with the expression `std::move(*other)` and does not make
    /// other empty: a moved-from result still contains a value, but the
    /// value itself is moved from.
    ///
    /// If other contains an error, move-constructs this result from that
    /// error.
    ///
    /// \note This constructor is defined as deleted if
    ///       `std::is_move_constructible<T>::value` or
    ///       `std::is_move_constructible<E>::value` is `false`
    ///
    /// \note This constructor is defined as trivial if both
    ///       `std::is_trivially_move_constructible<T>::value` and
    ///       `std::is_trivially_move_constructible<E>::value` are `true`
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<std::string,int>{"hello world"};
    /// auto s = std::move(r);
    ///
    /// assert(s == "hello world");
    /// ```
    ///
    /// \param other the result to move
    constexpr result(result&& other) = default;

    /// \{
    /// \brief Converting copy constructor
    ///
    /// If \p other contains a value, constructs a result object
    /// that contains a value, initialized as if direct-initializing
    /// (but not direct-list-initializing) an object of type `T` with the
    /// expression `*other`.
    ///
    /// If \p other contains an error, constructs a result object that
    /// contains an error, initialized as if direct-initializing
    /// (but not direct-list-initializing) an object of type `E`.
    ///
    /// \note This constructor does not participate in overload resolution
    ///       unless the following conditions are met:
    ///       - `std::is_constructible_v<T, const U&>` is `true`
    ///       - T is not constructible or convertible from any expression
    ///         of type (possibly const) `result<T2,E2>`
    ///       - E is not constructible or convertible from any expression
    ///         of type (possible const) `result<T2,E2>`
    ///
    /// \note This constructor is explicit if and only if
    ///       `std::is_convertible_v<const T2&, T>` or
    ///       `std::is_convertible_v<const E2&, E>` is `false`
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// const auto r = cpp::result<int,int>{42};
    /// const auto s = cpp::result<long,long>{r};
    ///
    /// assert(r == s);
    /// ```
    ///
    /// \param other the other type to convert
    template <typename T2, typename E2,
              typename std::enable_if<detail::result_is_implicit_copy_convertible<T,E,T2,E2>::value,int>::type = 0>
    result(const result<T2,E2>& other)
      noexcept(std::is_nothrow_constructible<T,const T2&>::value &&
               std::is_nothrow_constructible<E,const E2&>::value);
    template <typename T2, typename E2,
              typename std::enable_if<detail::result_is_explicit_copy_convertible<T,E,T2,E2>::value,int>::type = 0>
    explicit result(const result<T2,E2>& other)
      noexcept(std::is_nothrow_constructible<T,const T2&>::value &&
               std::is_nothrow_constructible<E,const E2&>::value);
    /// \}

    /// \{
    /// \brief Converting move constructor
    ///
    /// If \p other contains a value, constructs a result object
    /// that contains a value, initialized as if direct-initializing
    /// (but not direct-list-initializing) an object of type T with the
    /// expression `std::move(*other)`.
    ///
    /// If \p other contains an error, constructs a result object that
    /// contains an error, initialized as if direct-initializing
    /// (but not direct-list-initializing) an object of type E&&.
    ///
    /// \note This constructor does not participate in overload resolution
    ///       unless the following conditions are met:
    ///       - `std::is_constructible_v<T, const U&>` is `true`
    ///       - T is not constructible or convertible from any expression
    ///         of type (possibly const) `result<T2,E2>`
    ///       - E is not constructible or convertible from any expression
    ///         of type (possible const) `result<T2,E2>`
    ///
    /// \note This constructor is explicit if and only if
    ///       `std::is_convertible_v<const T2&, T>` or
    ///       `std::is_convertible_v<const E2&, E>` is `false`
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<std::unique_ptr<Derived>,int>{
    ///   std::make_unique<Derived>()
    /// };
    /// const auto s = cpp::result<std::unique_ptr<Base>,long>{
    ///   std::move(r)
    /// };
    /// ```
    ///
    /// \param other the other type to convert
    template <typename T2, typename E2,
              typename std::enable_if<detail::result_is_implicit_move_convertible<T,E,T2,E2>::value,int>::type = 0>
    result(result<T2,E2>&& other)
      noexcept(std::is_nothrow_constructible<T,T2&&>::value &&
               std::is_nothrow_constructible<E,E2&&>::value);
    template <typename T2, typename E2,
              typename std::enable_if<detail::result_is_explicit_move_convertible<T,E,T2,E2>::value,int>::type = 0>
    explicit result(result<T2,E2>&& other)
      noexcept(std::is_nothrow_constructible<T,T2&&>::value &&
               std::is_nothrow_constructible<E,E2&&>::value);
    /// \}

    //-------------------------------------------------------------------------

    /// \brief Constructs a result object that contains a value
    ///
    /// The value is initialized as if direct-initializing (but not
    /// direct-list-initializing) an object of type `T` from the arguments
    /// `std::forward<Args>(args)...`
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<std::string,int>{
    ///   cpp::in_place, "Hello world"
    /// };
    /// ```
    ///
    /// \param args the arguments to pass to T's constructor
    template <typename...Args,
              typename = typename std::enable_if<std::is_constructible<T,Args...>::value>::type>
    constexpr explicit result(in_place_t, Args&&... args)
      noexcept(std::is_nothrow_constructible<T, Args...>::value);

    /// \brief Constructs a result object that contains a value
    ///
    /// The value is initialized as if direct-initializing (but not
    /// direct-list-initializing) an object of type `T` from the arguments
    /// `std::forward<std::initializer_list<U>>(ilist)`,
    /// `std::forward<Args>(args)...`
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<std::string,int>{
    ///   cpp::in_place, {'H','e','l','l','o'}
    /// };
    /// ```
    ///
    /// \param ilist An initializer list of entries to forward
    /// \param args  the arguments to pass to T's constructor
    template <typename U, typename...Args,
              typename = typename std::enable_if<std::is_constructible<T, std::initializer_list<U>&, Args...>::value>::type>
    constexpr explicit result(in_place_t,
                              std::initializer_list<U> ilist,
                              Args&&...args)
      noexcept(std::is_nothrow_constructible<T, std::initializer_list<U>, Args...>::value);

    //-------------------------------------------------------------------------

    /// \brief Constructs a result object that contains an error
    ///
    /// the value is initialized as if direct-initializing (but not
    /// direct-list-initializing) an object of type `E` from the arguments
    /// `std::forward<Args>(args)...`
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<int,std::string>{
    ///   cpp::in_place_error, "Hello world"
    /// };
    /// ```
    ///
    /// \param args the arguments to pass to E's constructor
    template <typename...Args,
              typename = typename std::enable_if<std::is_constructible<E,Args...>::value>::type>
    constexpr explicit result(in_place_error_t, Args&&... args)
      noexcept(std::is_nothrow_constructible<E, Args...>::value);

    /// \brief Constructs a result object that contains an error
    ///
    /// The value is initialized as if direct-initializing (but not
    /// direct-list-initializing) an object of type `E` from the arguments
    /// `std::forward<std::initializer_list<U>>(ilist)`,
    /// `std::forward<Args>(args)...`
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<int,std::string>{
    ///   cpp::in_place_error, {'H','e','l','l','o'}
    /// };
    /// ```
    ///
    /// \param ilist An initializer list of entries to forward
    /// \param args  the arguments to pass to Es constructor
    template <typename U, typename...Args,
              typename = typename std::enable_if<std::is_constructible<E, std::initializer_list<U>&, Args...>::value>::type>
    constexpr explicit result(in_place_error_t,
                              std::initializer_list<U> ilist,
                              Args&&...args)
      noexcept(std::is_nothrow_constructible<E, std::initializer_list<U>, Args...>::value);

    //-------------------------------------------------------------------------

    /// \{
    /// \brief Constructs the underlying error of this result
    ///
    /// \note This constructor only participates in overload resolution if
    ///       `E` is constructible from \p e
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// cpp::result<int,int> r = cpp::fail(42);
    ///
    /// auto get_error_result() -> cpp::result<int,std::string> {
    ///   return cpp::fail("hello world!");
    /// }
    /// ```
    ///
    /// \param e the failure error
    template <typename E2,
              typename = typename std::enable_if<std::is_constructible<E,const E2&>::value>::type>
    constexpr /* implicit */ result(const failure<E2>& e)
      noexcept(std::is_nothrow_constructible<E,const E2&>::value);
    template <typename E2,
              typename = typename std::enable_if<std::is_constructible<E,E2&&>::value>::type>
    constexpr /* implicit */ result(failure<E2>&& e)
      noexcept(std::is_nothrow_constructible<E,E2&&>::value);
    /// \}

    /// \{
    /// \brief Constructs a result object that contains a value
    ///
    /// The value is initialized as if direct-initializing (but not
    /// direct-list-initializing) an object of type T with the expression
    /// value.
    ///
    /// \note This constructor is constexpr if the constructor of T
    ///       selected by direct-initialization is constexpr
    ///
    /// \note This constructor does not participate in overload
    ///       resolution unless `std::is_constructible_v<T, U&&>` is true
    ///       and `decay_t<U>` is neither `in_place_t`, `in_place_error_t`,
    ///       nor a `result` type.
    ///
    /// \note This constructor is explicit if and only if
    ///       `std::is_convertible_v<U&&, T>` is `false`
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// cpp::result<int,int> r = 42;
    ///
    /// auto get_value() -> cpp::result<std::string,int> {
    ///   return "hello world!"; // implicit conversion
    /// }
    /// ```
    ///
    /// \param value the value to copy
    template <typename U,
              typename std::enable_if<detail::result_is_explicit_value_convertible<T,U>::value,int>::type = 0>
    constexpr explicit result(U&& value)
      noexcept(std::is_nothrow_constructible<T,U>::value);
    template <typename U,
              typename std::enable_if<detail::result_is_implicit_value_convertible<T,U>::value,int>::type = 0>
    constexpr /* implicit */ result(U&& value)
      noexcept(std::is_nothrow_constructible<T,U>::value);
    /// \}

    //-------------------------------------------------------------------------

    /// \brief Copy assigns the result stored in \p other
    ///
    /// \note This assignment operator only participates in overload resolution
    ///       if the following conditions are met:
    ///       - `std::is_nothrow_copy_constructible_v<T>` is `true`, and
    ///       - `std::is_nothrow_copy_constructible_v<E>` is `true`
    ///       this restriction guarantees that no '
    ///
    /// \note This assignment operator is defined as trivial if the following
    ///       conditions are all `true`:
    ///       - `std::is_trivially_copy_constructible<T>::value`
    ///       - `std::is_trivially_copy_constructible<E>::value`
    ///       - `std::is_trivially_copy_assignable<T>::value`
    ///       - `std::is_trivially_copy_assignable<E>::value`
    ///       - `std::is_trivially_destructible<T>::value`
    ///       - `std::is_trivially_destructible<E>::value`
    ///
    /// \param other the other result to copy
    auto operator=(const result& other) -> result& = default;

    /// \brief Move assigns the result stored in \p other
    ///
    /// \note This assignment operator only participates in overload resolution
    ///       if the following conditions are met:
    ///       - `std::is_nothrow_copy_constructible_v<T>` is `true`, and
    ///       - `std::is_nothrow_copy_constructible_v<E>` is `true`
    ///       this restriction guarantees that no 'valueless_by_exception` state
    ///       may occur.
    ///
    /// \note This assignment operator is defined as trivial if the following
    ///       conditions are all `true`:
    ///       - `std::is_trivially_move_constructible<T>::value`
    ///       - `std::is_trivially_move_constructible<E>::value`
    ///       - `std::is_trivially_move_assignable<T>::value`
    ///       - `std::is_trivially_move_assignable<E>::value`
    ///       - `std::is_trivially_destructible<T>::value`
    ///       - `std::is_trivially_destructible<E>::value`
    ///
    /// \param other the other result to copy
    auto operator=(result&& other) -> result& = default;

    /// \brief Copy-converts the state of \p other
    ///
    /// If both `*this` and \p other contain either values or errors, the
    /// underlying value is constructed as if through assignment.
    ///
    /// Otherwise if `*this` contains a value, but \p other contains an error,
    /// then the contained value is destroyed by calling its destructor. `*this`
    /// will no longer contain a value after the call, and will now contain `E`
    /// constructed as if direct-initializing (but not direct-list-initializing)
    /// an object with an argument of `const E2&`.
    ///
    /// If \p other contains a value and `*this` contains an error, then the
    /// contained error is destroyed by calling its destructor. `*this` now
    /// contains a value constructed as if direct-initializing (but not
    /// direct-list-initializing) an object with an argument of `const T2&`.
    ///
    /// \note The function does not participate in overload resolution unless
    ///       - `std::is_nothrow_constructible_v<T, const T2&>`,
    ///         `std::is_assignable_v<T&, const T2&>`,
    ///         `std::is_nothrow_constructible_v<E, const E2&>`,
    ///         `std::is_assignable_v<E&, const E2&>` are all true.
    ///       - T is not constructible, convertible, or assignable from any
    ///         expression of type (possibly const) `result<T2,E2>`
    ///
    /// \param other the other result object to convert
    /// \return reference to `(*this)`
    template <typename T2, typename E2,
              typename = typename std::enable_if<detail::result_is_copy_convert_assignable<T,E,T2,E2>::value>::type>
    auto operator=(const result<T2,E2>& other)
      noexcept(std::is_nothrow_assignable<T,const T2&>::value &&
               std::is_nothrow_assignable<E,const E2&>::value) -> result&;

    /// \brief Move-converts the state of \p other
    ///
    /// If both `*this` and \p other contain either values or errors, the
    /// underlying value is constructed as if through move-assignment.
    ///
    /// Otherwise if `*this` contains a value, but \p other contains an error,
    /// then the contained value is destroyed by calling its destructor. `*this`
    /// will no longer contain a value after the call, and will now contain `E`
    /// constructed as if direct-initializing (but not direct-list-initializing)
    /// an object with an argument of `E2&&`.
    ///
    /// If \p other contains a value and `*this` contains an error, then the
    /// contained error is destroyed by calling its destructor. `*this` now
    /// contains a value constructed as if direct-initializing (but not
    /// direct-list-initializing) an object with an argument of `T2&&`.
    ///
    /// \note The function does not participate in overload resolution unless
    ///       - `std::is_nothrow_constructible_v<T, T2&&>`,
    ///         `std::is_assignable_v<T&, T2&&>`,
    ///         `std::is_nothrow_constructible_v<E, E2&&>`,
    ///         `std::is_assignable_v<E&, E2&&>` are all true.
    ///       - T is not constructible, convertible, or assignable from any
    ///         expression of type (possibly const) `result<T2,E2>`
    ///
    /// \param other the other result object to convert
    /// \return reference to `(*this)`
    template <typename T2, typename E2,
              typename = typename std::enable_if<detail::result_is_move_convert_assignable<T,E,T2,E2>::value>::type>
    auto operator=(result<T2,E2>&& other)
      noexcept(std::is_nothrow_assignable<T,T2&&>::value &&
               std::is_nothrow_assignable<E,E2&&>::value) -> result&;

    /// \brief Perfect-forwarded assignment
    ///
    /// Depending on whether `*this` contains a value before the call, the
    /// contained value is either direct-initialized from std::forward<U>(value)
    /// or assigned from std::forward<U>(value).
    ///
    /// \note The function does not participate in overload resolution unless
    ///       - `std::decay_t<U>` is not a result type,
    ///       - `std::decay_t<U>` is not a failure type
    ///       - `std::is_nothrow_constructible_v<T, U>` is `true`
    ///       - `std::is_assignable_v<T&, U>` is `true`
    ///       - and at least one of the following is `true`:
    ///           - `T` is not a scalar type;
    ///           - `std::decay_t<U>` is not `T`.
    ///
    /// \param value to assign to the contained value
    /// \return reference to `(*this)`
    template <typename U,
              typename = typename std::enable_if<detail::result_is_value_assignable<T,U>::value>::type>
    auto operator=(U&& value)
      noexcept(std::is_nothrow_assignable<T,U>::value) -> result&;

    /// \{
    /// \brief Perfect-forwarded assignment
    ///
    /// Depending on whether `*this` contains an error before the call, the
    /// contained error is either direct-initialized via forwarding the error,
    /// or assigned from forwarding the error
    ///
    /// \note The function does not participate in overload resolution unless
    ///       - `std::is_nothrow_constructible_v<E, E2>` is `true`, and
    ///       - `std::is_assignable_v<E&, E2>` is `true`
    ///
    /// \param other the failure value to assign to this
    /// \return reference to `(*this)`
    template <typename E2,
              typename = typename std::enable_if<detail::result_is_failure_assignable<E,const E2&>::value>::type>
    auto operator=(const failure<E2>& other)
      noexcept(std::is_nothrow_assignable<E, const E2&>::value) -> result&;
    template <typename E2,
              typename = typename std::enable_if<detail::result_is_failure_assignable<E,E2&&>::value>::type>
    auto operator=(failure<E2>&& other)
      noexcept(std::is_nothrow_assignable<E, E2&&>::value) -> result&;
    /// \}

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Retrieves a pointer to the contained value
    ///
    /// This operator exists to give `result` an `optional`-like API for cases
    /// where it's known that the `result` already contains a value.
    ///
    /// Care must be taken to ensure that this is only used in safe contexts
    /// where a `T` value is active.
    ///
    /// \note The behavior is undefined if `*this` does not contain a value.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<Widget,int>{
    ///   make_widget()
    /// };
    ///
    /// r->do_something();
    /// ```
    ///
    /// \return a pointer to the contained value
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto operator->()
      noexcept -> typename std::remove_reference<T>::type*;
    RESULT_WARN_UNUSED
    constexpr auto operator->()
      const noexcept -> typename std::remove_reference<typename std::add_const<T>::type>::type*;
    /// \}

    /// \{
    /// \brief Retrieves a reference to the contained value
    ///
    /// This operator exists to give `result` an `optional`-like API for cases
    /// where it's known that the `result` already contains a value.
    ///
    /// Care must be taken to ensure that this is only used in safe contexts
    /// where a `T` value is active.
    ///
    /// \note The behaviour is undefined if `*this` does not contain a value
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<Widget,int>{
    ///   make_widget()
    /// };
    ///
    /// (*r).do_something();
    ///
    /// consume(*r);
    /// ```
    ///
    /// \return a reference to the contained value
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto operator*()
      & noexcept -> typename std::add_lvalue_reference<T>::type;
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto operator*()
      && noexcept -> typename std::add_rvalue_reference<T>::type;
    RESULT_WARN_UNUSED
    constexpr auto operator*()
      const& noexcept -> typename std::add_lvalue_reference<typename std::add_const<T>::type>::type;
    RESULT_WARN_UNUSED
    constexpr auto operator*()
      const&& noexcept -> typename std::add_rvalue_reference<typename std::add_const<T>::type>::type;
    /// \}

    //-------------------------------------------------------------------------

    /// \brief Contextually convertible to `true` if `*this` contains a value
    ///
    /// This function exists to allow for simple, terse checks for containing
    /// a value.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto get_result() -> cpp::result<int, int>;
    /// auto r = get_result();
    /// if (r) { ... }
    ///
    /// assert(static_cast<bool>(cpp::result<int,int>{42}));
    ///
    /// assert(!static_cast<bool>(cpp::result<int,int>{cpp::fail(42)}));
    /// ```
    ///
    /// \return `true` if `*this` contains a value, `false` if `*this`
    ///         does not contain a value
    RESULT_WARN_UNUSED
    constexpr explicit operator bool() const noexcept;

    /// \brief Returns `true` if `*this` contains a value
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto get_result() -> cpp::result<int, int>;
    /// auto r = get_result();
    /// if (r.has_value()) { ... }
    ///
    /// assert(cpp::result<int,int>{42}.has_value());
    ///
    /// assert(!cpp::result<int,int>{cpp::fail(42)}.has_value());
    /// ```
    ///
    /// \return `true` if `*this` contains a value, `false` if `*this`
    ///         contains an error
    RESULT_WARN_UNUSED
    constexpr auto has_value() const noexcept -> bool;

    /// \brief Returns `true` if `*this` contains an error
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto get_result() -> cpp::result<int, int>;
    ///
    /// auto r = get_result();
    /// if (r.has_error()) { ... }
    ///
    /// assert(!cpp::result<int,int>{42}.has_error());
    ///
    /// assert(cpp::result<int,int>{cpp::fail(42)}.has_error());
    /// ```
    ///
    /// \return `true` if `*this` contains an error, `false` if `*this`
    ///          contains a value
    RESULT_WARN_UNUSED
    constexpr auto has_error() const noexcept -> bool;

    //-------------------------------------------------------------------------

    /// \{
    /// \brief Returns a reference to the contained value
    ///
    /// This function provides checked (throwing) access to the underlying
    /// value. The constness and refness of this result is propagated to the
    /// underlying reference.
    ///
    /// If this contains an error, an exception is thrown containing the
    /// underlying error. The error is consumed propagating the same constness
    /// and refness of this result.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// assert(cpp::result<int,int>{42}.value() == 42);
    ///
    /// auto r = cpp::result<std::unique_ptr<int>,int>{
    ///   std::make_unique<int>(42)
    /// };
    /// auto s = std::move(r).value();
    ///
    /// try {
    ///   auto r = cpp::result<int,int>{ cpp::fail(42) };
    ///   auto v = r.value();
    /// } catch (const cpp::bad_result_access<int>& e) {
    ///   assert(e.error() == 42);
    /// }
    /// ```
    ///
    /// \throws bad_result_access<E> if `*this` does not contain a value.
    ///
    /// \return the value of `*this`
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto value()
      & -> typename std::add_lvalue_reference<T>::type;
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto value()
      && -> typename std::add_rvalue_reference<T>::type;
    RESULT_WARN_UNUSED
    constexpr auto value()
      const & -> typename std::add_lvalue_reference<typename std::add_const<T>::type>::type;
    RESULT_WARN_UNUSED
    constexpr auto value()
      const && -> typename std::add_rvalue_reference<typename std::add_const<T>::type>::type;
    /// \}

    /// \{
    /// \brief Returns the contained error, if one exists, or a
    ///        default-constructed error value
    ///
    /// The `error()` function will not throw any exceptions if `E` does not
    /// throw any exceptions for the copy or move construction.
    ///
    /// This is to limit the possible scope for exceptions, and to allow the
    /// error type to be treated as a "status"-like type, where the
    /// default-constructed case is considered the "good" state.
    ///
    /// If this function is invoked on an rvalue of a result, the error is
    /// returned via move-construction
    ///
    /// ### Requires
    ///
    /// * `std::is_default_constructible<E>::value` is `true`
    /// * `std::is_copy_constructible<E>::value` or
    ///   `std::is_move_constructible<E>::value` is `true`
    /// * `E{}` represents the "good" (non-error) state
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<int,std::error_code>{ 42 };
    /// assert(r.error() == std::error_code{});
    ///
    /// auto r = cpp::result<int,std::error_code>{
    ///   cpp::fail(std::io_errc::stream)
    /// };
    ///
    /// assert(r.error() == std::io_errc::stream);
    /// ```
    ///
    /// \return the error or a default-constructed error value
    RESULT_WARN_UNUSED
    constexpr auto error() const &
      noexcept(std::is_nothrow_constructible<E>::value &&
               std::is_nothrow_copy_constructible<E>::value) -> E;
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto error() &&
      noexcept(std::is_nothrow_constructible<E>::value &&
               std::is_nothrow_move_constructible<E>::value) -> E;
    /// }

    /// \{
    /// \brief Asserts an expectation that this result contains an error,
    ///        throwing a bad_result_access on failure
    ///
    /// If this function is invoked from an rvalue of `result`, then this will
    /// consume the underlying error first, if there is one.
    ///
    /// \note This function exists as a means to allow for results to be marked
    ///       `used` without requiring directly inspecting the underlying value.
    ///       This is, in effect, equivalent to `assert(res.has_value())`,
    ///       however it uses exceptions to ensure the stack can be unwound, and
    ///       exceptions invoked.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto start_service() -> cpp::result<void,int>;
    ///
    /// start_service().expect("Service failed to start!");
    /// ```
    ///
    /// \param message the message to provide to this expectation
    template <typename String,
              typename = typename std::enable_if<(
                std::is_convertible<String,const std::string&>::value &&
                std::is_copy_constructible<E>::value
              )>::type>
    RESULT_CPP14_CONSTEXPR auto expect(String&& message) const & -> void;
    template <typename String,
              typename = typename std::enable_if<(
                std::is_convertible<String,const std::string&>::value &&
                std::is_move_constructible<E>::value
              )>::type>
    RESULT_CPP14_CONSTEXPR auto expect(String&& message) && -> void;
    /// \}

    //-------------------------------------------------------------------------
    // Monadic Functionalities
    //-------------------------------------------------------------------------
  public:

    /// \{
    /// \brief Returns the contained value if `*this` has a value,
    ///        otherwise returns \p default_value.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<int,int>{42};
    /// assert(r.value_or(0) == 42);
    ///
    /// auto r = cpp::result<int,int>{cpp::fail(42)};
    /// assert(r.value_or(0) == 0);
    /// ```
    ///
    /// \param default_value the value to use in case `*this` contains an error
    /// \return the contained value or \p default_value
    template <typename U>
    RESULT_WARN_UNUSED
    constexpr auto value_or(U&& default_value)
      const & -> typename std::remove_reference<T>::type;
    template <typename U>
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto value_or(U&& default_value)
      && -> typename std::remove_reference<T>::type;
    /// \}

    /// \{
    /// \brief Returns the contained error if `*this` has an error,
    ///        otherwise returns \p default_error.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<int,int>{42};
    /// assert(r.error_or(0) == cpp::fail(0));
    ///
    /// auto r = cpp::result<int,int>{cpp::fail(42)};
    /// assert(r.error_or(0) == cpp::fail(42));
    /// ```
    ///
    /// \param default_error the error to use in case `*this` is empty
    /// \return the contained value or \p default_error
    template <typename U>
    RESULT_WARN_UNUSED
    constexpr auto error_or(U&& default_error) const & -> error_type;
    template <typename U>
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto error_or(U&& default_error) && -> error_type;
    /// \}

    //-------------------------------------------------------------------------

    /// \brief Returns a result containing \p value if this result contains
    ///        a value, otherwise returns a result containing the current
    ///        error.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<int,int>{42};
    /// assert(r.and_then(100) == 100);
    ///
    /// auto r = cpp::result<int,int>{cpp::fail(42)};
    /// assert(r.and_then(100) == cpp::fail(42));
    /// ```
    ///
    /// \param value the value to return as a result
    /// \return a result of \p value if this contains a value
    template <typename U>
    RESULT_WARN_UNUSED
    constexpr auto and_then(U&& value) const -> result<typename std::decay<U>::type,E>;

    /// \{
    /// \brief Invokes the function \p fn with the value of this result as
    ///        the argument
    ///
    /// If this result contains an error, a result of the error is returned
    ///
    /// The function being called must return a `result` type or the program
    /// is ill-formed
    ///
    /// If this is called on an rvalue of `result` which contains an error,
    /// the returned `result` is constructed from an rvalue of that error.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto to_string(int) -> cpp::result<std::string,int>;
    /// auto r = cpp::result<int,int>{42};
    /// assert(r.flat_map(to_string) == "42");
    ///
    /// auto r = cpp::result<int,int>{cpp::fail(42)};
    /// assert(r.flat_map(to_string) == cpp::fail(42));
    /// ```
    ///
    /// \param fn the function to invoke with this
    /// \return The result of the function being called
    template <typename Fn>
    RESULT_WARN_UNUSED
    constexpr auto flat_map(Fn&& fn) const & -> detail::invoke_result_t<Fn, const T&>;
    template <typename Fn>
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto flat_map(Fn&& fn) && -> detail::invoke_result_t<Fn, T&&>;
    /// \}

    /// \{
    /// \brief Invokes the function \p fn with the value of this result as
    ///        the argument
    ///
    /// If this result is an error, the result of this function is that
    /// error. Otherwise this function wraps the result and returns it as an
    /// result.
    ///
    /// If this is called on an rvalue of `result` which contains an error,
    /// the returned `result` is constructed from an rvalue of that error.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto to_string(int) -> std::string;
    /// auto r = cpp::result<int,int>{42};
    /// assert(r.map(to_string) == "42");
    ///
    /// auto r = cpp::result<int,int>{cpp::fail(42)};
    /// assert(r.map(to_string) == cpp::fail(42));
    /// ```
    ///
    /// \param fn the function to invoke with this
    /// \return The result result of the function invoked
    template <typename Fn>
    RESULT_WARN_UNUSED
    constexpr auto map(Fn&& fn) const & -> result<detail::invoke_result_t<Fn,const T&>,E>;
    template <typename Fn>
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto map(Fn&& fn) && -> result<detail::invoke_result_t<Fn,T&&>,E>;
    /// \}

    /// \{
    /// \brief Invokes the function \p fn with the error of this result as
    ///        the argument
    ///
    /// If this result contains a value, the result of this function is that
    /// value. Otherwise the function is called with that error and returns the
    /// result as a result.
    ///
    /// If this is called on an rvalue of `result` which contains a value,
    /// the returned `result` is constructed from an rvalue of that value.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto to_string(int) -> std::string;
    /// auto r = cpp::result<int,int>{42};
    /// assert(r.map_error(to_string) == 42);
    ///
    /// auto r = cpp::result<int,int>{cpp::fail(42)};
    /// assert(r.map_error(to_string) == cpp::fail("42"));
    ///
    /// auto r = cpp::result<std::string,int>{};
    /// auto s = r.map(std::string::size); // 's' contains 'result<size_t,int>'
    /// ```
    ///
    /// \param fn the function to invoke with this
    /// \return The result result of the function invoked
    template <typename Fn>
    RESULT_WARN_UNUSED
    constexpr auto map_error(Fn&& fn)
      const & -> result<T, detail::invoke_result_t<Fn,const E&>>;
    template <typename Fn>
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto map_error(Fn&& fn)
      && -> result<T, detail::invoke_result_t<Fn,E&&>>;
    /// \}

    /// \{
    /// \brief Invokes the function \p fn with the error of this result as
    ///        the argument
    ///
    /// If this result contains a value, a result of the value is returned
    ///
    /// The function being called must return a `result` type or the program
    /// is ill-formed
    ///
    /// If this is called on an rvalue of `result` which contains an error,
    /// the returned `result` is constructed from an rvalue of that error.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto to_string(int) -> cpp::result<int,std::string>;
    /// auto r = cpp::result<int,int>{42};
    /// assert(r.flat_map(to_string) == 42);
    ///
    /// auto r = cpp::result<int,int>{cpp::fail(42)};
    /// assert(r.flat_map(to_string) == cpp::fail("42"));
    /// ```
    ///
    /// \param fn the function to invoke with this
    /// \return The result of the function being called
    template <typename Fn>
    RESULT_WARN_UNUSED
    constexpr auto flat_map_error(Fn&& fn)
      const & -> detail::invoke_result_t<Fn, const E&>;
    template <typename Fn>
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto flat_map_error(Fn&& fn)
      && -> detail::invoke_result_t<Fn, E&&>;
    /// \}

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    detail::result_storage<T,E> m_storage;

    //-------------------------------------------------------------------------
    // Private Monadic Functions
    //-------------------------------------------------------------------------
  private:

    /// \{
    /// \brief Map implementations for void and non-void functions
    ///
    /// \param fn the function
    template <typename Fn>
    constexpr auto map_impl(std::true_type, Fn&& fn) const & -> result<void,E>;
    template <typename Fn>
    constexpr auto map_impl(std::false_type, Fn&& fn) const & -> result<detail::invoke_result_t<Fn,const T&>,E>;
    template <typename Fn>
    RESULT_CPP14_CONSTEXPR auto map_impl(std::true_type, Fn&& fn) && -> result<void,E>;
    template <typename Fn>
    RESULT_CPP14_CONSTEXPR auto map_impl(std::false_type, Fn&& fn) && -> result<detail::invoke_result_t<Fn,T&&>,E>;
    /// \}
  };

  //===========================================================================
  // class : result<void,E>
  //===========================================================================

  /////////////////////////////////////////////////////////////////////////////
  /// \brief Partial specialization of `result<void, E>`
  ///
  /// \tparam E the underlying error type
  /////////////////////////////////////////////////////////////////////////////
  template <typename E>
  class RESULT_NODISCARD result<void,E>
  {
    // Type requirements

    static_assert(
      !std::is_void<typename std::decay<E>::type>::value,
      "It is ill-formed for E to be (possibly CV-qualified) void type"
    );
    static_assert(
      !std::is_abstract<E>::value,
      "It is ill-formed for E to be abstract type"
    );
    static_assert(
      !is_failure<typename std::decay<E>::type>::value,
      "It is ill-formed for E to be a (possibly CV-qualified) 'failure' type"
    );
    static_assert(
      !std::is_reference<E>::value,
      "It is ill-formed for E to be a reference type. "
      "Only T types may be lvalue references"
    );

    // Friendship

    friend detail::result_error_extractor;

    template <typename T2, typename E2>
    friend class result;

    //-------------------------------------------------------------------------
    // Public Member Types
    //-------------------------------------------------------------------------
  public:

    using value_type = void; ///< The value type of this result
    using error_type = E;    ///< The error type of this result
    using failure_type = failure<E>; ///< The failure type

    template <typename U>
    using rebind = result<U,E>; ///< Rebinds the result type

    //-------------------------------------------------------------------------
    // Constructor / Assignment
    //-------------------------------------------------------------------------
  public:

    /// \brief Constructs a `result` object in a value state
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<void,int>{};
    /// ```
    constexpr result() noexcept;

    /// \brief Copy constructs this result
    ///
    /// If other contains an error, constructs an object that contains a copy
    /// of that error.
    ///
    /// \note This constructor is defined as deleted if
    ///       `std::is_copy_constructible<E>::value` is `false`
    ///
    /// \note This constructor is defined as trivial if both
    ///       `std::is_trivially_copy_constructible<E>::value` are `true`
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// const auto r = cpp::result<void,int>{};
    /// const auto s = r;
    /// ```
    ///
    /// \param other the result to copy
    constexpr result(const result& other) = default;

    /// \brief Move constructs a result
    ///
    /// If other contains an error, move-constructs this result from that
    /// error.
    ///
    /// \note This constructor is defined as deleted if
    ///       `std::is_move_constructible<E>::value` is `false`
    ///
    /// \note This constructor is defined as trivial if both
    ///       `std::is_trivially_move_constructible<E>::value` are `true`
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<void,std::string>{};
    /// auto s = std::move(r);
    /// ```
    ///
    /// \param other the result to move
    constexpr result(result&& other) = default;

    /// \brief Converting copy constructor
    ///
    /// If \p other contains a value, constructs a result object that is not
    /// in an error state -- ignoring the value.
    ///
    /// If \p other contains an error, constructs a result object that
    /// contains an error, initialized as if direct-initializing
    /// (but not direct-list-initializing) an object of type `E`.
    ///
    /// \note This constructor does not participate in overload resolution
    ///       unless the following conditions are met:
    ///       - `std::is_constructible_v<E, const E2&>` is `true`
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// const auto r = cpp::result<int,int>{42};
    /// const auto s = cpp::result<void,int>{r};
    /// ```
    ///
    /// \param other the other type to convert
    template <typename U, typename E2,
              typename = typename std::enable_if<std::is_constructible<E,E2>::value>::type>
    explicit result(const result<U,E2>& other)
      noexcept(std::is_nothrow_constructible<E,const E2&>::value);

    /// \brief Converting move constructor
    ///
    /// If \p other contains an error, constructs a result object that
    /// contains an error, initialized as if direct-initializing
    /// (but not direct-list-initializing) an object of type E&&.
    ///
    /// \note This constructor does not participate in overload resolution
    ///       unless the following conditions are met:
    ///       - `std::is_constructible_v<E, const E2&>` is `true`
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<int,std::string>{42};
    /// auto s = cpp::result<void,std::string>{
    ///   std::move(r)
    /// };
    /// ```
    ///
    /// \param other the other type to convert
    template <typename U, typename E2,
              typename = typename std::enable_if<std::is_constructible<E,E2>::value>::type>
    explicit result(result<U,E2>&& other)
      noexcept(std::is_nothrow_constructible<E,E2&&>::value);

    //-------------------------------------------------------------------------

    /// \brief Constructs a result object in a value state
    ///
    /// This constructor exists primarily for symmetry with the `result<T,E>`
    /// constructor. Unlike the `T` overload, no variadic arguments may be
    /// supplied.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<void,std::string>{cpp::in_place};
    /// ```
    constexpr explicit result(in_place_t) noexcept;

    /// \brief Constructs a result object that contains an error
    ///
    /// the value is initialized as if direct-initializing (but not
    /// direct-list-initializing) an object of type `E` from the arguments
    /// `std::forward<Args>(args)...`
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<void,std::string>{
    ///   cpp::in_place_error, "Hello world"
    /// };
    /// ```
    ///
    /// \param args the arguments to pass to `E`'s constructor
    template <typename...Args,
              typename = typename std::enable_if<std::is_constructible<E,Args...>::value>::type>
    constexpr explicit result(in_place_error_t, Args&&... args)
      noexcept(std::is_nothrow_constructible<E, Args...>::value);

    /// \brief Constructs a result object that contains an error
    ///
    /// The value is initialized as if direct-initializing (but not
    /// direct-list-initializing) an object of type `E` from the arguments
    /// `std::forward<std::initializer_list<U>>(ilist)`,
    /// `std::forward<Args>(args)...`
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto r = cpp::result<void,std::string>{
    ///   cpp::in_place_error, {'H','e','l','l','o'}
    /// };
    /// ```
    ///
    /// \param ilist An initializer list of entries to forward
    /// \param args  the arguments to pass to Es constructor
    template <typename U, typename...Args,
              typename = typename std::enable_if<std::is_constructible<E, std::initializer_list<U>&, Args...>::value>::type>
    constexpr explicit result(in_place_error_t,
                              std::initializer_list<U> ilist,
                              Args&&...args)
      noexcept(std::is_nothrow_constructible<E, std::initializer_list<U>, Args...>::value);

    //-------------------------------------------------------------------------

    /// \{
    /// \brief Constructs the underlying error of this result
    ///
    /// \note This constructor only participates in overload resolution if
    ///       `E` is constructible from \p e
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// cpp::result<void,int> r = cpp::fail(42);
    ///
    /// auto get_error_result() -> cpp::result<void,std::string> {
    ///   return cpp::fail("hello world!");
    /// }
    /// ```
    ///
    /// \param e the failure error
    template <typename E2,
              typename = typename std::enable_if<std::is_constructible<E,const E2&>::value>::type>
    constexpr /* implicit */ result(const failure<E2>& e)
      noexcept(std::is_nothrow_constructible<E,const E2&>::value);
    template <typename E2,
              typename = typename std::enable_if<std::is_constructible<E,E2&&>::value>::type>
    constexpr /* implicit */ result(failure<E2>&& e)
      noexcept(std::is_nothrow_constructible<E,E2&&>::value);
    /// \}

    //-------------------------------------------------------------------------

    /// \brief Copy assigns the result stored in \p other
    ///
    /// \note The function does not participate in overload resolution unless
    ///       - `std::is_nothrow_copy_constructible_v<E>` is `true`
    ///       this restriction guarantees that no 'valueless_by_exception` state
    ///       may occur.
    ///
    /// \note This assignment operator is defined as trivial if the following
    ///       conditions are all `true`:
    ///       - `std::is_trivially_copy_constructible<E>::value`
    ///       - `std::is_trivially_copy_assignable<E>::value`
    ///       - `std::is_trivially_destructible<E>::value`
    ///
    /// \param other the other result to copy
    auto operator=(const result& other) -> result& = default;

    /// \brief Move assigns the result stored in \p other
    ///
    /// \note The function does not participate in overload resolution unless
    ///       - `std::is_nothrow_copy_constructible_v<E>` is `true`
    ///       this restriction guarantees that no 'valueless_by_exception` state
    ///       may occur.
    ///
    /// \note This assignment operator is defined as trivial if the following
    ///       conditions are all `true`:
    ///       - `std::is_trivially_move_constructible<E>::value`
    ///       - `std::is_trivially_move_assignable<E>::value`
    ///       - `std::is_trivially_destructible<E>::value`
    ///
    /// \param other the other result to copy
    auto operator=(result&& other) -> result& = default;

    /// \brief Copy-converts the state of \p other
    ///
    /// If both this and \p other contain an error, the underlying error is
    /// assigned through copy-assignment.
    ///
    /// If \p other contains a value state, this result is constructed in a
    /// value state.
    ///
    /// If \p other contans an error state, and this contains a value state,
    /// the underlying error is constructed through copy-construction.
    ///
    /// \note The function does not participate in overload resolution unless
    ///       - `std::is_nothrow_constructible_v<E, const E2&>`,
    ///         `std::is_assignable_v<E&, const E2&>` are all `true`.
    ///
    /// \param other the other result object to convert
    /// \return reference to `(*this)`
    template <typename E2,
              typename = typename std::enable_if<std::is_nothrow_constructible<E,const E2&>::value &&
                                                 std::is_assignable<E&,const E2&>::value>::type>
    auto operator=(const result<void,E2>& other)
      noexcept(std::is_nothrow_assignable<E, const E2&>::value) -> result&;

    /// \brief Move-converts the state of \p other
    ///
    /// If both this and \p other contain an error, the underlying error is
    /// assigned through move-assignment.
    ///
    /// If \p other contains a value state, this result is constructed in a
    /// value state.
    ///
    /// If \p other contans an error state, and this contains a value state,
    /// the underlying error is constructed through move-construction.
    ///
    /// \note The function does not participate in overload resolution unless
    ///       - `std::is_nothrow_constructible_v<E, E2&&>`,
    ///         `std::is_assignable_v<E&, E2&&>` are all `true`.
    ///
    /// \param other the other result object to convert
    /// \return reference to `(*this)`
    template <typename E2,
              typename = typename std::enable_if<std::is_nothrow_constructible<E,E2&&>::value &&
                                                 std::is_assignable<E&,E2&&>::value>::type>
    auto operator=(result<void,E2>&& other)
      noexcept(std::is_nothrow_assignable<E, E2&&>::value) -> result&;

    /// \{
    /// \brief Perfect-forwarded assignment
    ///
    /// Depending on whether `*this` contains an error before the call, the
    /// contained error is either direct-initialized via forwarding the error,
    /// or assigned from forwarding the error
    ///
    /// \note The function does not participate in overload resolution unless
    ///       - `std::is_nothrow_constructible_v<E, E2>` is `true`, and
    ///       - `std::is_assignable_v<E&, E2>` is `true`
    ///
    /// \param other the failure value to assign to this
    /// \return reference to `(*this)`
    template <typename E2,
              typename = typename std::enable_if<detail::result_is_failure_assignable<E,const E2&>::value>::type>
    auto operator=(const failure<E2>& other)
      noexcept(std::is_nothrow_assignable<E, const E2&>::value) -> result&;
    template <typename E2,
              typename = typename std::enable_if<detail::result_is_failure_assignable<E,E2&&>::value>::type>
    auto operator=(failure<E2>&& other)
      noexcept(std::is_nothrow_assignable<E, E2&&>::value) -> result&;
    /// \}

    //-------------------------------------------------------------------------
    // Observers
    //-------------------------------------------------------------------------
  public:


    /// \brief Contextually convertible to `true` if `*this` does not contain
    ///        an error
    ///
    /// This function exists to allow for simple, terse checks for containing
    /// a value.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto get_result() -> cpp::result<void, int>;
    /// auto r = get_result();
    /// if (r) { ... }
    ///
    /// assert(static_cast<bool>(cpp::result<void,int>{}));
    ///
    /// assert(!static_cast<bool>(cpp::result<void,int>{cpp::fail(42)}));
    /// ```
    ///
    /// \return `true` if `*this` contains a value, `false` if `*this`
    ///         does not contain a value
    RESULT_WARN_UNUSED
    constexpr explicit operator bool() const noexcept;

    /// \copydoc result<T,E>::has_value
    RESULT_WARN_UNUSED
    constexpr auto has_value() const noexcept -> bool;

    /// \copydoc result<T,E>::has_error
    RESULT_WARN_UNUSED
    constexpr auto has_error() const noexcept -> bool;

    //-------------------------------------------------------------------------

    /// \{
    /// \brief Throws an exception if `(*this)` is in an error state
    ///
    /// This function exists for symmetry with `cpp::result<T,E>` objects where
    /// `T` contains a value.
    ///
    /// If this contains an error, an exception is thrown containing the
    /// underlying error. The error is consumed propagating the same constness
    /// and refness of this result.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// cpp::result<void,int>{}.value(); // no exception
    ///
    /// auto r = cpp::result<void,std::unique_ptr<int>>{
    ///   cpp::fail(std::make_unique<int>(42))
    /// };
    /// std::move(r).value(); // throws bad_result_access<std::unique_ptr<int>>
    ///
    /// try {
    ///   auto r = cpp::result<void,int>{ cpp::fail(42) }.value();
    /// } catch (const cpp::bad_result_access<int>& e) {
    ///   assert(e.error() == 42);
    /// }
    /// ```
    ///
    /// \throws bad_result_access<E> if `*this` does not contain a value.
    RESULT_CPP14_CONSTEXPR auto value() && -> void;
    RESULT_CPP14_CONSTEXPR auto value() const & -> void;
    /// \}

    /// \{
    /// \copydoc result<T,E>::error
    RESULT_WARN_UNUSED
    constexpr auto error() const &
      noexcept(std::is_nothrow_constructible<E>::value &&
               std::is_nothrow_copy_constructible<E>::value) -> E;
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto error() &&
      noexcept(std::is_nothrow_constructible<E>::value &&
               std::is_nothrow_copy_constructible<E>::value) -> E;
    /// \}

    /// \{
    /// \copydoc result<T,E>::expect
    template <typename String,
              typename = typename std::enable_if<(
                std::is_convertible<String,const std::string&>::value &&
                std::is_copy_constructible<E>::value
              )>::type>
    RESULT_CPP14_CONSTEXPR auto expect(String&& message) const & -> void;
    template <typename String,
              typename = typename std::enable_if<(
                std::is_convertible<String,const std::string&>::value &&
                std::is_move_constructible<E>::value
              )>::type>
    RESULT_CPP14_CONSTEXPR auto expect(String&& message) && -> void;
    /// \}

    //-------------------------------------------------------------------------
    // Monadic Functionalities
    //-------------------------------------------------------------------------
  public:

    /// \{
    /// \copydoc result<T,E>::error_or
    template <typename U>
    RESULT_WARN_UNUSED
    constexpr auto error_or(U&& default_error) const & -> error_type;
    template <typename U>
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto error_or(U&& default_error) && -> error_type;
    /// \}

    //-------------------------------------------------------------------------

    /// \copydoc result<T,E>::and_then
    template <typename U>
    RESULT_WARN_UNUSED
    constexpr auto and_then(U&& value) const -> result<typename std::decay<U>::type,E>;

    /// \{
    /// \brief Invokes the function \p fn if `(*this)` contains no value
    ///
    /// If this result contains an error, a result of the error is returned
    ///
    /// The function being called must return a `result` type or the program
    /// is ill-formed
    ///
    /// If this is called on an rvalue of `result` which contains an error,
    /// the returned `result` is constructed from an rvalue of that error.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto generate_int() -> cpp::result<int,int> { return 42; }
    /// auto r = cpp::result<void,int>{};
    /// assert(r.flat_map(generate_int) == 42);
    ///
    /// auto r = cpp::result<void,int>{cpp::fail(42)};
    /// assert(r.flat_map(generate_int) == cpp::fail(42));
    /// ```
    ///
    /// \param fn the function to invoke with this
    /// \return The result of the function being called
    template <typename Fn>
    RESULT_WARN_UNUSED
    constexpr auto flat_map(Fn&& fn) const & -> detail::invoke_result_t<Fn>;
    template <typename Fn>
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto flat_map(Fn&& fn) && -> detail::invoke_result_t<Fn>;
    /// \}

    /// \{
    /// \brief Invokes the function \p fn if `(*this)` contains no value
    ///
    /// If this result is an error, the result of this function is that
    /// error. Otherwise this function wraps the result and returns it as an
    /// result.
    ///
    /// If this is called on an rvalue of `result` which contains an error,
    /// the returned `result` is constructed from an rvalue of that error.
    ///
    /// ### Examples
    ///
    /// Basic Usage:
    ///
    /// ```cpp
    /// auto generate_int() -> int { return 42; }
    /// auto r = cpp::result<void,int>{};
    /// assert(r.map(generate_int) == 42);
    ///
    /// auto r = cpp::result<void,int>{cpp::fail(42)};
    /// assert(r.map(generate_int) == cpp::fail(42));
    /// ```
    ///
    /// \param fn the function to invoke with this
    /// \return The result result of the function invoked
    template <typename Fn>
    RESULT_WARN_UNUSED
    constexpr auto map(Fn&& fn) const & -> result<detail::invoke_result_t<Fn>,E>;
    template <typename Fn>
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto map(Fn&& fn) && -> result<detail::invoke_result_t<Fn>,E>;
    /// \}

    /// \{
    /// \copydoc result<T,E>::map_error
    template <typename Fn>
    constexpr auto map_error(Fn&& fn) const & -> result<void, detail::invoke_result_t<Fn,const E&>>;
    template <typename Fn>
    RESULT_CPP14_CONSTEXPR
    auto map_error(Fn&& fn) && -> result<void, detail::invoke_result_t<Fn,E&&>>;
    /// \}


    /// \{
    /// \copydoc result<T,E>::flat_map_error
    template <typename Fn>
    RESULT_WARN_UNUSED
    constexpr auto flat_map_error(Fn&& fn) const & -> detail::invoke_result_t<Fn, const E&>;
    template <typename Fn>
    RESULT_WARN_UNUSED
    RESULT_CPP14_CONSTEXPR auto flat_map_error(Fn&& fn) && -> detail::invoke_result_t<Fn, E&&>;
    /// \}

    //-------------------------------------------------------------------------
    // Private Members
    //-------------------------------------------------------------------------
  private:

    detail::result_storage<detail::unit,E> m_storage;

    //-------------------------------------------------------------------------
    // Private Monadic Functions
    //-------------------------------------------------------------------------
  private:

    /// \{
    /// \brief Map implementations for void and non-void functions
    ///
    /// \param fn the function
    template <typename Fn>
    constexpr auto map_impl(std::true_type, Fn&& fn) const & -> result<void,E>;
    template <typename Fn>
    constexpr auto map_impl(std::false_type, Fn&& fn) const & -> result<detail::invoke_result_t<Fn>,E>;
    template <typename Fn>
    RESULT_CPP14_CONSTEXPR auto map_impl(std::true_type, Fn&& fn) && -> result<void,E>;
    template <typename Fn>
    RESULT_CPP14_CONSTEXPR auto map_impl(std::false_type, Fn&& fn) && -> result<detail::invoke_result_t<Fn>,E>;
    /// \}
  };

  //===========================================================================
  // non-member functions : class : result
  //===========================================================================

  //---------------------------------------------------------------------------
  // Comparison
  //---------------------------------------------------------------------------

  template <typename T1, typename E1, typename T2, typename E2>
  constexpr auto operator==(const result<T1,E1>& lhs, const result<T2,E2>& rhs)
    noexcept -> bool;
  template <typename T1, typename E1, typename T2, typename E2>
  constexpr auto operator!=(const result<T1,E1>& lhs, const result<T2,E2>& rhs)
    noexcept -> bool;
  template <typename T1, typename E1, typename T2, typename E2>
  constexpr auto operator>=(const result<T1,E1>& lhs, const result<T2,E2>& rhs)
    noexcept -> bool;
  template <typename T1, typename E1, typename T2, typename E2>
  constexpr auto operator<=(const result<T1,E1>& lhs, const result<T2,E2>& rhs)
    noexcept -> bool;
  template <typename T1, typename E1, typename T2, typename E2>
  constexpr auto operator>(const result<T1,E1>& lhs, const result<T2,E2>& rhs)
    noexcept -> bool;
  template <typename T1, typename E1, typename T2, typename E2>
  constexpr auto operator<(const result<T1,E1>& lhs, const result<T2,E2>& rhs)
    noexcept -> bool;

  //---------------------------------------------------------------------------

  template <typename E1, typename E2>
  constexpr auto operator==(const result<void,E1>& lhs, const result<void,E2>& rhs)
    noexcept -> bool;
  template <typename E1, typename E2>
  constexpr auto operator!=(const result<void,E1>& lhs, const result<void,E2>& rhs)
    noexcept -> bool;
  template <typename E1, typename E2>
  constexpr auto operator>=(const result<void,E1>& lhs, const result<void,E2>& rhs)
    noexcept -> bool;
  template <typename E1, typename E2>
  constexpr auto operator<=(const result<void,E1>& lhs, const result<void,E2>& rhs)
    noexcept -> bool;
  template <typename E1, typename E2>
  constexpr auto operator>(const result<void,E1>& lhs, const result<void,E2>& rhs)
    noexcept -> bool;
  template <typename E1, typename E2>
  constexpr auto operator<(const result<void,E1>& lhs, const result<void,E2>& rhs)
    noexcept -> bool;

  //---------------------------------------------------------------------------

  template <typename T, typename E, typename U,
            typename = typename std::enable_if<!std::is_same<T,void>::value>::type>
  constexpr auto operator==(const result<T,E>& exp, const U& value)
    noexcept -> bool;
  template <typename T, typename U, typename E,
            typename = typename std::enable_if<!std::is_same<U,void>::value>::type>
  constexpr auto operator==(const T& value, const result<U,E>& exp)
    noexcept -> bool;
  template <typename T, typename E, typename U,
            typename = typename std::enable_if<!std::is_same<T,void>::value>::type>
  constexpr auto operator!=(const result<T,E>& exp, const U& value)
    noexcept -> bool;
  template <typename T, typename U, typename E,
            typename = typename std::enable_if<!std::is_same<U,void>::value>::type>
  constexpr auto operator!=(const T& value, const result<U,E>& exp)
    noexcept -> bool;
  template <typename T, typename E, typename U,
            typename = typename std::enable_if<!std::is_same<T,void>::value>::type>
  constexpr auto operator<=(const result<T,E>& exp, const U& value)
    noexcept -> bool;
  template <typename T, typename U, typename E,
            typename = typename std::enable_if<!std::is_same<U,void>::value>::type>
  constexpr auto operator<=(const T& value, const result<U,E>& exp)
    noexcept -> bool;
  template <typename T, typename E, typename U,
            typename = typename std::enable_if<!std::is_same<T,void>::value>::type>
  constexpr auto operator>=(const result<T,E>& exp, const U& value)
    noexcept -> bool;
  template <typename T, typename U, typename E,
            typename = typename std::enable_if<!std::is_same<U,void>::value>::type>
  constexpr auto operator>=(const T& value, const result<U,E>& exp)
    noexcept -> bool;
  template <typename T, typename E, typename U,
            typename = typename std::enable_if<!std::is_same<T,void>::value>::type>
  constexpr auto operator<(const result<T,E>& exp, const U& value)
    noexcept -> bool;
  template <typename T, typename U, typename E,
            typename = typename std::enable_if<!std::is_same<U,void>::value>::type>
  constexpr auto operator<(const T& value, const result<U,E>& exp)
    noexcept -> bool;
  template <typename T, typename E, typename U,
            typename = typename std::enable_if<!std::is_same<T,void>::value>::type>
  constexpr auto operator>(const result<T,E>& exp, const U& value)
    noexcept -> bool;
  template <typename T, typename U, typename E,
            typename = typename std::enable_if<!std::is_same<U,void>::value>::type>
  constexpr auto operator>(const T& value, const result<U,E>& exp)
    noexcept -> bool;

  //---------------------------------------------------------------------------

  template <typename T, typename E, typename U>
  constexpr auto operator==(const result<T,E>& exp, const failure<U>& value)
    noexcept -> bool;
  template <typename T, typename U, typename E>
  constexpr auto operator==(const failure<T>& value, const result<E,U>& exp)
    noexcept -> bool;
  template <typename T, typename E, typename U>
  constexpr auto operator!=(const result<T,E>& exp, const failure<U>& value)
    noexcept -> bool;
  template <typename T, typename U, typename E>
  constexpr auto operator!=(const failure<T>& value, const result<E,U>& exp)
    noexcept -> bool;
  template <typename T, typename E, typename U>
  constexpr auto operator<=(const result<T,E>& exp, const failure<U>& value)
    noexcept -> bool;
  template <typename T, typename U, typename E>
  constexpr auto operator<=(const failure<T>& value, const result<E,U>& exp)
    noexcept -> bool;
  template <typename T, typename E, typename U>
  constexpr auto operator>=(const result<T,E>& exp, const failure<U>& value)
    noexcept -> bool;
  template <typename T, typename U, typename E>
  constexpr auto operator>=(const failure<T>& value, const result<E,U>& exp)
    noexcept -> bool;
  template <typename T, typename E, typename U>
  constexpr auto operator<(const result<T,E>& exp, const failure<U>& value)
    noexcept -> bool;
  template <typename T, typename U, typename E>
  constexpr auto operator<(const failure<T>& value, const result<E,U>& exp)
    noexcept -> bool;
  template <typename T, typename E, typename U>
  constexpr auto operator>(const result<T,E>& exp, const failure<U>& value)
    noexcept -> bool;
  template <typename T, typename U, typename E>
  constexpr auto operator>(const failure<T>& value, const result<E,U>& exp)
    noexcept -> bool;

  //---------------------------------------------------------------------------
  // Utilities
  //---------------------------------------------------------------------------

  /// \{
  /// \brief Swaps the contents of \p lhs with \p rhs
  ///
  /// \param lhs the left result
  /// \param rhs the right result
  template <typename T, typename E>
  auto swap(result<T,E>& lhs, result<T,E>& rhs)
#if __cplusplus >= 201703L
    noexcept(std::is_nothrow_move_constructible<result<T,E>>::value &&
             std::is_nothrow_move_assignable<result<T,E>>::value &&
             std::is_nothrow_swappable<T>::value &&
             std::is_nothrow_swappable<E>::value)
#else
    noexcept(std::is_nothrow_move_constructible<result<T,E>>::value &&
             std::is_nothrow_move_assignable<result<T,E>>::value)
#endif
    -> void;
  template <typename E>
  auto swap(result<void,E>& lhs, result<void,E>& rhs)
#if __cplusplus >= 201703L
    noexcept(std::is_nothrow_move_constructible<result<void,E>>::value &&
             std::is_nothrow_move_assignable<result<void,E>>::value &&
             std::is_nothrow_swappable<E>::value)
#else
    noexcept(std::is_nothrow_move_constructible<result<void,E>>::value &&
             std::is_nothrow_move_assignable<result<void,E>>::value)
#endif
     -> void;
  /// \}

} // inline namespace bitwizeshift
} // namespace EXPECTED_NAMESPACE

namespace std {

  template <typename T, typename E>
  struct hash<::RESULT_NS_IMPL::result<T,E>>
  {
    auto operator()(const RESULT_NS_IMPL::result<T,E>& x) const -> size_t
    {
      if (x.has_value()) {
        return std::hash<T>{}(*x) + 1; // add '1' to differentiate from error case
      }
      return std::hash<E>{}(::RESULT_NS_IMPL::detail::extract_error(x));
    }
  };

  template <typename E>
  struct hash<::RESULT_NS_IMPL::result<void,E>>
  {
    auto operator()(const RESULT_NS_IMPL::result<void,E>& x) const -> size_t
    {
      if (x.has_value()) {
        return 0;
      }
      return std::hash<E>{}(::RESULT_NS_IMPL::detail::extract_error(x));
    }
  };

} // namespace std

#if !defined(RESULT_DISABLE_EXCEPTIONS)

//=============================================================================
// class : bad_result_access
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::bad_result_access<E>::bad_result_access(E2&& error)
  : logic_error{"error attempting to access value from result containing error"},
    m_error(detail::forward<E2>(error))
{

}

template <typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::bad_result_access<E>::bad_result_access(
  const char* what_arg,
  E2&& error
) : logic_error{what_arg},
    m_error(detail::forward<E2>(error))
{

}

template <typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::bad_result_access<E>::bad_result_access(
  const std::string& what_arg,
  E2&& error
) : logic_error{what_arg},
    m_error(detail::forward<E2>(error))
{

}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename E>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::bad_result_access<E>::error()
  & noexcept -> E&
{
  return m_error;
}

template <typename E>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::bad_result_access<E>::error()
  && noexcept -> E&&
{
  return static_cast<E&&>(m_error);
}

template <typename E>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::bad_result_access<E>::error()
  const & noexcept -> const E&
{
  return m_error;
}

template <typename E>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::bad_result_access<E>::error()
  const && noexcept -> const E&&
{
  return static_cast<const E&&>(m_error);
}

#endif

//=============================================================================
// class : failure
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

template <typename E>
template <typename...Args, typename>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::failure<E>::failure(in_place_t, Args&&...args)
  noexcept(std::is_nothrow_constructible<E, Args...>::value)
  : m_failure(detail::forward<Args>(args)...)
{

}

template <typename E>
template <typename U, typename...Args, typename>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::failure<E>::failure(
  in_place_t,
  std::initializer_list<U> ilist,
  Args&&...args
) noexcept(std::is_nothrow_constructible<E, std::initializer_list<U>, Args...>::value)
  : m_failure(ilist, detail::forward<Args>(args)...)
{

}

template <typename E>
template <typename E2,
          typename std::enable_if<RESULT_NS_IMPL::detail::failure_is_explicit_value_convertible<E,E2>::value,int>::type>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::failure<E>::failure(E2&& error)
  noexcept(std::is_nothrow_constructible<E,E2>::value)
  : m_failure(detail::forward<E2>(error))
{

}

template <typename E>
template <typename E2,
          typename std::enable_if<RESULT_NS_IMPL::detail::failure_is_implicit_value_convertible<E,E2>::value,int>::type>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::failure<E>::failure(E2&& error)
  noexcept(std::is_nothrow_constructible<E,E2>::value)
  : m_failure(detail::forward<E2>(error))
{

}

template <typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::failure<E>::failure(const failure<E2>& other)
  noexcept(std::is_nothrow_constructible<E,const E2&>::value)
  : m_failure(other.error())
{

}

template <typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::failure<E>::failure(failure<E2>&& other)
  noexcept(std::is_nothrow_constructible<E,E2&&>::value)
  : m_failure(static_cast<failure<E2>&&>(other).error())
{

}

//-----------------------------------------------------------------------------

template <typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::failure<E>::operator=(E2&& error)
  noexcept(
    std::is_nothrow_assignable<E,E2>::value ||
    std::is_lvalue_reference<E>::value
  ) -> failure&
{
  m_failure = detail::forward<E2>(error);

  return (*this);
}

template <typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::failure<E>::operator=(const failure<E2>& other)
  noexcept(std::is_nothrow_assignable<E,const E2&>::value)
  -> failure&
{
  m_failure = other.error();

  return (*this);
}

template <typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::failure<E>::operator=(failure<E2>&& other)
  noexcept(std::is_nothrow_assignable<E,E2&&>::value)
  -> failure&
{
  m_failure = static_cast<failure<E2>&&>(other).error();

  return (*this);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename E>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::failure<E>::error()
  & noexcept -> typename std::add_lvalue_reference<E>::type
{
  return m_failure;
}

template <typename E>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::failure<E>::error()
  && noexcept -> typename std::add_rvalue_reference<E>::type
{
  using reference = typename std::add_rvalue_reference<E>::type;

  return static_cast<reference>(m_failure);
}

template <typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::failure<E>::error()
  const & noexcept
  -> typename std::add_lvalue_reference<typename std::add_const<E>::type>::type
{
  return m_failure;
}

template <typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::failure<E>::error()
  const && noexcept
  -> typename std::add_rvalue_reference<typename std::add_const<E>::type>::type
{
  using reference = typename std::add_rvalue_reference<typename std::add_const<E>::type>::type;

  return static_cast<reference>(m_failure);
}

//=============================================================================
// non-member functions : class : failure
//=============================================================================

//-----------------------------------------------------------------------------
// Comparison
//-----------------------------------------------------------------------------

template <typename E1, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator==(const failure<E1>& lhs, const failure<E2>& rhs)
  noexcept -> bool
{
  return lhs.error() == rhs.error();
}

template <typename E1, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator!=(const failure<E1>& lhs, const failure<E2>& rhs)
  noexcept -> bool
{
  return lhs.error() != rhs.error();
}

template <typename E1, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator<(const failure<E1>& lhs, const failure<E2>& rhs)
  noexcept -> bool
{
  return lhs.error() < rhs.error();
}

template <typename E1, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator>(const failure<E1>& lhs, const failure<E2>& rhs)
  noexcept -> bool
{
  return lhs.error() > rhs.error();
}

template <typename E1, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator<=(const failure<E1>& lhs, const failure<E2>& rhs)
  noexcept -> bool
{
  return lhs.error() <= rhs.error();
}

template <typename E1, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator>=(const failure<E1>& lhs, const failure<E2>& rhs)
  noexcept -> bool
{
  return lhs.error() >= rhs.error();
}

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

template <typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::fail(E&& e)
  noexcept(std::is_nothrow_constructible<typename std::decay<E>::type,E>::value)
  -> failure<typename std::decay<E>::type>
{
  using result_type = failure<typename std::decay<E>::type>;

  return result_type(
    detail::forward<E>(e)
  );
}

template <typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::fail(std::reference_wrapper<E> e)
  noexcept -> failure<E&>
{
  using result_type = failure<E&>;

  return result_type{e.get()};
}

template <typename E, typename...Args, typename>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::fail(Args&&...args)
  noexcept(std::is_nothrow_constructible<E, Args...>::value)
  -> failure<E>
{
  return failure<E>(in_place, detail::forward<Args>(args)...);
}

template <typename E, typename U, typename...Args, typename>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::fail(std::initializer_list<U> ilist, Args&&...args)
  noexcept(std::is_nothrow_constructible<E, std::initializer_list<U>, Args...>::value)
  -> failure<E>
{
  return failure<E>(in_place, ilist, detail::forward<Args>(args)...);
}

template <typename E>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::swap(failure<E>& lhs, failure<E>& rhs)
#if __cplusplus >= 201703L
  noexcept(std::is_nothrow_swappable<E>::value) -> void
#else
  noexcept(std::is_nothrow_move_constructible<E>::value)
   -> void
#endif
{
  using std::swap;

  swap(lhs.error(), rhs.error());
}

//=============================================================================
// class : detail::result_union<T, E, IsTrivial>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

template <typename T, typename E, bool IsTrivial>
inline RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::detail::result_union<T, E, IsTrivial>
  ::result_union(unit)
  noexcept
  : m_empty{}
{
  // m_has_value intentionally not set
}

template <typename T, typename E, bool IsTrivial>
template <typename...Args>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::detail::result_union<T,E,IsTrivial>
  ::result_union(in_place_t, Args&&...args)
  noexcept(std::is_nothrow_constructible<T, Args...>::value)
  : m_value(detail::forward<Args>(args)...),
    m_has_value{true}
{
}

template <typename T, typename E, bool IsTrivial>
template <typename...Args>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::detail::result_union<T,E,IsTrivial>
  ::result_union(in_place_error_t, Args&&...args)
  noexcept(std::is_nothrow_constructible<E, Args...>::value)
  : m_error(detail::forward<Args>(args)...),
    m_has_value{false}
{
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

template <typename T, typename E, bool IsTrivial>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::result_union<T, E, IsTrivial>::destroy()
  const noexcept -> void
{
  // do nothing
}

//=============================================================================
// class : detail::result_union<T, E, false>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Destructor / Assignment
//-----------------------------------------------------------------------------

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::detail::result_union<T, E, false>
  ::result_union(unit)
  noexcept
  : m_empty{}
{
  // m_has_value intentionally not set
}

template <typename T, typename E>
template <typename...Args>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::detail::result_union<T,E,false>
  ::result_union(in_place_t, Args&&...args)
  noexcept(std::is_nothrow_constructible<T, Args...>::value)
  : m_value(detail::forward<Args>(args)...),
    m_has_value{true}
{
}

template <typename T, typename E>
template <typename...Args>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::detail::result_union<T,E,false>
  ::result_union(in_place_error_t, Args&&...args)
  noexcept(std::is_nothrow_constructible<E, Args...>::value)
  : m_error(detail::forward<Args>(args)...),
    m_has_value{false}
{
}

//-----------------------------------------------------------------------------

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::detail::result_union<T,E,false>
  ::~result_union()
  noexcept(std::is_nothrow_destructible<T>::value && std::is_nothrow_destructible<E>::value)
{
  destroy();
}

//-----------------------------------------------------------------------------
// Modifiers
//-----------------------------------------------------------------------------

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::result_union<T, E, false>::destroy()
  -> void
{
  if (m_has_value) {
    m_value.~underlying_value_type();
  } else {
    m_error.~underlying_error_type();
  }
}

//=============================================================================
// class : result_construct_base<T, E>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructors / Assignment
//-----------------------------------------------------------------------------

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::detail::result_construct_base<T,E>::result_construct_base(unit)
  noexcept
  : storage{unit{}}
{
}

template <typename T, typename E>
template <typename...Args>
inline constexpr RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::detail::result_construct_base<T,E>::result_construct_base(
  in_place_t,
  Args&&...args
) noexcept(std::is_nothrow_constructible<T, Args...>::value)
  : storage{in_place, detail::forward<Args>(args)...}
{
}

template <typename T, typename E>
template <typename...Args>
inline constexpr RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::detail::result_construct_base<T,E>::result_construct_base(
  in_place_error_t,
  Args&&...args
) noexcept(std::is_nothrow_constructible<E, Args...>::value)
  : storage(in_place_error, detail::forward<Args>(args)...)
{
}

//-----------------------------------------------------------------------------
// Construction / Assignment
//-----------------------------------------------------------------------------

template <typename T, typename E>
template <typename...Args>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::result_construct_base<T,E>::construct_value(Args&&...args)
  noexcept(std::is_nothrow_constructible<T,Args...>::value)
  -> void
{
  using value_type = typename storage_type::underlying_value_type;

  auto* p = static_cast<void*>(std::addressof(storage.m_value));
  new (p) value_type(detail::forward<Args>(args)...);
  storage.m_has_value = true;
}

template <typename T, typename E>
template <typename...Args>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::result_construct_base<T,E>::construct_error(Args&&...args)
  noexcept(std::is_nothrow_constructible<E,Args...>::value)
  -> void
{
  using error_type = typename storage_type::underlying_error_type;

  auto* p = static_cast<void*>(std::addressof(storage.m_error));
  new (p) error_type(detail::forward<Args>(args)...);
  storage.m_has_value = false;
}

template <typename T, typename E>
template <typename Result>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::result_construct_base<T,E>::construct_error_from_result(
  Result&& other
) -> void
{
  if (other.storage.m_has_value) {
    construct_value();
  } else {
    construct_error(detail::forward<Result>(other).storage.m_error);
  }
}


template <typename T, typename E>
template <typename Result>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::result_construct_base<T,E>::construct_from_result(
  Result&& other
) -> void
{
  if (other.storage.m_has_value) {
    construct_value_from_result_impl(
      std::is_lvalue_reference<T>{},
      detail::forward<Result>(other).storage.m_value
    );
  } else {
    construct_error(detail::forward<Result>(other).storage.m_error);
  }
}

template <typename T, typename E>
template <typename Value>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::result_construct_base<T,E>::assign_value(Value&& value)
  noexcept(std::is_nothrow_assignable<T,Value>::value)
  -> void
{
  if (!storage.m_has_value) {
    storage.destroy();
    construct_value(detail::forward<Value>(value));
  } else {
    storage.m_value = detail::forward<Value>(value);
  }
}

template <typename T, typename E>
template <typename Error>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::result_construct_base<T,E>::assign_error(Error&& error)
  noexcept(std::is_nothrow_assignable<E,Error>::value)
  -> void
{
  if (storage.m_has_value) {
    storage.destroy();
    construct_error(detail::forward<Error>(error));
  } else {
    storage.m_error = detail::forward<Error>(error);
  }
}

template <typename T, typename E>
template <typename Result>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::result_construct_base<T,E>::assign_from_result(Result&& other)
  -> void
{
  if (other.storage.m_has_value != storage.m_has_value) {
    storage.destroy();
    construct_from_result(detail::forward<Result>(other));
  } else if (storage.m_has_value) {
    assign_value_from_result_impl(
      std::is_lvalue_reference<T>{},
      detail::forward<Result>(other)
    );
  } else {
    storage.m_error = detail::forward<Result>(other).storage.m_error;
  }
}

template <typename T, typename E>
template <typename ReferenceWrapper>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::result_construct_base<T,E>::construct_value_from_result_impl(
  std::true_type,
  ReferenceWrapper&& reference
) noexcept -> void
{
  using value_type = typename storage_type::underlying_value_type;

  auto* p = static_cast<void*>(std::addressof(storage.m_value));
  new (p) value_type(reference.get());
  storage.m_has_value = true;
}

template <typename T, typename E>
template <typename Value>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::result_construct_base<T,E>::construct_value_from_result_impl(
  std::false_type,
  Value&& value
) noexcept(std::is_nothrow_constructible<T,Value>::value) -> void
{
  using value_type = typename storage_type::underlying_value_type;

  auto* p = static_cast<void*>(std::addressof(storage.m_value));
  new (p) value_type(detail::forward<Value>(value));
  storage.m_has_value = true;
}

template <typename T, typename E>
template <typename Result>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::result_construct_base<T,E>::assign_value_from_result_impl(
  std::true_type,
  Result&& other
) -> void
{
  // T is a reference; unwrap it
  storage.m_value = other.storage.m_value.get();
}

template <typename T, typename E>
template <typename Result>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::result_construct_base<T,E>::assign_value_from_result_impl(
  std::false_type,
  Result&& other
) -> void
{
  storage.m_value = detail::forward<Result>(other).storage.m_value;
}


//=============================================================================
// class : result_trivial_copy_ctor_base_impl
//=============================================================================

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::detail::result_trivial_copy_ctor_base_impl<T,E>
  ::result_trivial_copy_ctor_base_impl(const result_trivial_copy_ctor_base_impl& other)
  noexcept(std::is_nothrow_copy_constructible<T>::value &&
           std::is_nothrow_copy_constructible<E>::value)
  : base_type(unit{})
{
  using ctor_base = result_construct_base<T,E>;

  ctor_base::construct_from_result(static_cast<const ctor_base&>(other));
}

//=============================================================================
// class : result_trivial_move_ctor_base
//=============================================================================

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::detail::result_trivial_move_ctor_base_impl<T, E>
  ::result_trivial_move_ctor_base_impl(result_trivial_move_ctor_base_impl&& other)
  noexcept(std::is_nothrow_move_constructible<T>::value &&
           std::is_nothrow_move_constructible<E>::value)
  : base_type(unit{})
{
  using ctor_base = result_construct_base<T,E>;

  ctor_base::construct_from_result(static_cast<ctor_base&&>(other));
}

//=============================================================================
// class : result_copy_assign_base
//=============================================================================

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::result_trivial_copy_assign_base_impl<T, E>
  ::operator=(const result_trivial_copy_assign_base_impl& other)
  noexcept(std::is_nothrow_copy_constructible<T>::value &&
           std::is_nothrow_copy_constructible<E>::value &&
           std::is_nothrow_copy_assignable<T>::value &&
           std::is_nothrow_copy_assignable<E>::value)
  -> result_trivial_copy_assign_base_impl&
{
  using ctor_base = result_construct_base<T,E>;

  ctor_base::assign_from_result(static_cast<const ctor_base&>(other));
  return (*this);
}

//=========================================================================
// class : result_move_assign_base
//=========================================================================

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::result_trivial_move_assign_base_impl<T, E>
  ::operator=(result_trivial_move_assign_base_impl&& other)
  noexcept(std::is_nothrow_move_constructible<T>::value &&
           std::is_nothrow_move_constructible<E>::value &&
           std::is_nothrow_move_assignable<T>::value &&
           std::is_nothrow_move_assignable<E>::value)
  -> result_trivial_move_assign_base_impl&
{
  using ctor_base = result_construct_base<T,E>;

  ctor_base::assign_from_result(static_cast<ctor_base&&>(other));
  return (*this);
}

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::detail::result_error_extractor::get(const result<T,E>& exp)
  noexcept -> const E&
{
  return exp.m_storage.storage.m_error;
}

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::detail::result_error_extractor::get(result<T,E>& exp)
  noexcept -> E&
{
  return exp.m_storage.storage.m_error;
}

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::detail::extract_error(const result<T,E>& exp) noexcept -> const E&
{
  return result_error_extractor::get(exp);
}

template <typename E>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::throw_bad_result_access(E&& error) -> void
{
#if defined(RESULT_DISABLE_EXCEPTIONS)
  std::fprintf(
    stderr,
    "error attempting to access value from result containing error\n"
  );
  std::abort();
#else
  std::abort();
/* // ESP32-S3向けビルドがコンパイルエラーとなるためコメントアウト by lovyan03; 
  using exception_type = bad_result_access<
    typename std::remove_const<
      typename std::remove_reference<E>::type
    >::type
  >;

  throw exception_type{
    detail::forward<E>(error)
  };
//*/
#endif
}

template <typename String, typename E>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::detail::throw_bad_result_access_message(
  String&& message,
  E&& error
) -> void
{
#if defined(RESULT_DISABLE_EXCEPTIONS)
  const auto message_string = std::string{
    detail::forward<String>(message)
  };
  std::fprintf(stderr, "%s\n", message_string.c_str());
  std::abort();
#else
  using exception_type = bad_result_access<
    typename std::remove_const<
      typename std::remove_reference<E>::type
    >::type
  >;

  throw exception_type{
    detail::forward<String>(message),
    detail::forward<E>(error)
  };
#endif
}

//=============================================================================
// class : result<T,E>
//=============================================================================

template <typename T, typename E>
template <typename U, typename>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<T, E>::result()
  noexcept(std::is_nothrow_constructible<U>::value)
  : m_storage(in_place)
{

}

template <typename T, typename E>
template <typename T2, typename E2,
          typename std::enable_if<RESULT_NS_IMPL::detail::result_is_implicit_copy_convertible<T,E,T2,E2>::value,int>::type>
inline RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::result<T, E>::result(const result<T2,E2>& other)
  noexcept(std::is_nothrow_constructible<T,const T2&>::value &&
           std::is_nothrow_constructible<E,const E2&>::value)
  : m_storage(detail::unit{})
{
  m_storage.construct_from_result(
    static_cast<const result<T2,E2>&>(other).m_storage
  );
}

template <typename T, typename E>
template <typename T2, typename E2,
          typename std::enable_if<RESULT_NS_IMPL::detail::result_is_explicit_copy_convertible<T,E,T2,E2>::value,int>::type>
inline RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::result<T, E>::result(const result<T2,E2>& other)
  noexcept(std::is_nothrow_constructible<T,const T2&>::value &&
           std::is_nothrow_constructible<E,const E2&>::value)
  : m_storage(detail::unit{})
{
  m_storage.construct_from_result(
    static_cast<const result<T2,E2>&>(other).m_storage
  );
}

template <typename T, typename E>
template <typename T2, typename E2,
          typename std::enable_if<RESULT_NS_IMPL::detail::result_is_implicit_move_convertible<T,E,T2,E2>::value,int>::type>
inline RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::result<T, E>::result(result<T2,E2>&& other)
  noexcept(std::is_nothrow_constructible<T,T2&&>::value &&
           std::is_nothrow_constructible<E,E2&&>::value)
  : m_storage(detail::unit{})
{
  m_storage.construct_from_result(
    static_cast<result<T2,E2>&&>(other).m_storage
  );
}

template <typename T, typename E>
template <typename T2, typename E2,
          typename std::enable_if<RESULT_NS_IMPL::detail::result_is_explicit_move_convertible<T,E,T2,E2>::value,int>::type>
inline RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::result<T, E>::result(result<T2,E2>&& other)
  noexcept(std::is_nothrow_constructible<T,T2&&>::value &&
           std::is_nothrow_constructible<E,E2&&>::value)
  : m_storage(detail::unit{})
{
  m_storage.construct_from_result(
    static_cast<result<T2,E2>&&>(other).m_storage
  );
}

//-----------------------------------------------------------------------------

template <typename T, typename E>
template <typename...Args, typename>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<T, E>::result(in_place_t, Args&&...args)
  noexcept(std::is_nothrow_constructible<T, Args...>::value)
  : m_storage(in_place, detail::forward<Args>(args)...)
{

}

template <typename T, typename E>
template <typename U, typename...Args, typename>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<T, E>::result(
  in_place_t,
  std::initializer_list<U> ilist,
  Args&&...args
) noexcept(std::is_nothrow_constructible<T, std::initializer_list<U>, Args...>::value)
  : m_storage(in_place, ilist, detail::forward<Args>(args)...)
{

}

//-----------------------------------------------------------------------------

template <typename T, typename E>
template <typename...Args, typename>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<T, E>::result(in_place_error_t, Args&&...args)
  noexcept(std::is_nothrow_constructible<E, Args...>::value)
  : m_storage(in_place_error, detail::forward<Args>(args)...)
{

}

template <typename T, typename E>
template <typename U, typename...Args, typename>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<T, E>::result(
  in_place_error_t,
  std::initializer_list<U> ilist,
  Args&&...args
) noexcept(std::is_nothrow_constructible<E, std::initializer_list<U>, Args...>::value)
  : m_storage(in_place_error, ilist, detail::forward<Args>(args)...)
{

}

//-------------------------------------------------------------------------

template <typename T, typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<T, E>::result(const failure<E2>& e)
  noexcept(std::is_nothrow_constructible<E,const E2&>::value)
  : m_storage(in_place_error, e.error())
{

}

template <typename T, typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<T, E>::result(failure<E2>&& e)
  noexcept(std::is_nothrow_constructible<E,E2&&>::value)
  : m_storage(in_place_error, static_cast<E2&&>(e.error()))
{

}

template <typename T, typename E>
template <typename U,
          typename std::enable_if<RESULT_NS_IMPL::detail::result_is_explicit_value_convertible<T,U>::value,int>::type>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<T, E>::result(U&& value)
  noexcept(std::is_nothrow_constructible<T,U>::value)
  : m_storage(in_place, detail::forward<U>(value))
{

}

template <typename T, typename E>
template <typename U,
          typename std::enable_if<RESULT_NS_IMPL::detail::result_is_implicit_value_convertible<T,U>::value,int>::type>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<T, E>::result(U&& value)
  noexcept(std::is_nothrow_constructible<T,U>::value)
  : m_storage(in_place, detail::forward<U>(value))
{

}

//-----------------------------------------------------------------------------

template <typename T, typename E>
template <typename T2, typename E2, typename>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::result<T, E>::operator=(const result<T2,E2>& other)
  noexcept(std::is_nothrow_assignable<T, const T2&>::value &&
           std::is_nothrow_assignable<E, const E2&>::value)
  -> result&
{
  m_storage.assign_from_result(
    static_cast<const result<T2,E2>&>(other).m_storage
  );
  return (*this);
}

template <typename T, typename E>
template <typename T2, typename E2, typename>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::result<T, E>::operator=(result<T2,E2>&& other)
  noexcept(std::is_nothrow_assignable<T, T2&&>::value &&
           std::is_nothrow_assignable<E, E2&&>::value)
  -> result&
{
  m_storage.assign_from_result(
    static_cast<result<T2,E2>&&>(other).m_storage
  );
  return (*this);
}

template <typename T, typename E>
template <typename U, typename>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::result<T, E>::operator=(U&& value)
  noexcept(std::is_nothrow_assignable<T, U>::value)
  -> result&
{
  m_storage.assign_value(detail::forward<U>(value));
  return (*this);
}

template <typename T, typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::result<T, E>::operator=(const failure<E2>& other)
  noexcept(std::is_nothrow_assignable<E, const E2&>::value)
  -> result&
{
  m_storage.assign_error(other.error());
  return (*this);
}

template <typename T, typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::result<T, E>::operator=(failure<E2>&& other)
  noexcept(std::is_nothrow_assignable<E, E2&&>::value)
  -> result&
{
  m_storage.assign_error(static_cast<E2&&>(other.error()));
  return (*this);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T, E>::operator->()
  noexcept -> typename std::remove_reference<T>::type*
{
  // Prior to C++17, std::addressof was not `constexpr`.
  // Since `addressof` fixes a relatively obscure issue where users define a
  // custom `operator&`, the pre-C++17 implementation has been defined to be
  // `&(**this)` so that it may exist in constexpr contexts.
#if __cplusplus >= 201703L
  return std::addressof(**this);
#else
  return &(**this);
#endif
}

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T, E>::operator->()
  const noexcept -> typename std::remove_reference<typename std::add_const<T>::type>::type*
{
#if __cplusplus >= 201703L
  return std::addressof(**this);
#else
  return &(**this);
#endif
}

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T, E>::operator*()
  & noexcept -> typename std::add_lvalue_reference<T>::type
{
  return m_storage.storage.m_value;
}

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T, E>::operator*()
  && noexcept -> typename std::add_rvalue_reference<T>::type
{
  using reference = typename std::add_rvalue_reference<T>::type;

  return static_cast<reference>(m_storage.storage.m_value);
}

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T, E>::operator*()
  const& noexcept -> typename std::add_lvalue_reference<typename std::add_const<T>::type>::type
{
  return m_storage.storage.m_value;
}

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T, E>::operator*()
  const&& noexcept -> typename std::add_rvalue_reference<typename std::add_const<T>::type>::type
{
  using reference = typename std::add_rvalue_reference<typename std::add_const<T>::type>::type;

  return static_cast<reference>(m_storage.storage.m_value);
}

//-----------------------------------------------------------------------------

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<T,E>::operator bool()
  const noexcept
{
  return m_storage.storage.m_has_value;
}

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T,E>::has_value()
  const noexcept -> bool
{
  return m_storage.storage.m_has_value;
}

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T,E>::has_error()
  const noexcept -> bool
{
  return !m_storage.storage.m_has_value;
}

//-----------------------------------------------------------------------------

// The `has_value()` expression below is incorrectly identified as an unused
// value, which results in the `-Wunused-value` warning. This is suppressed
// to prevent false-positives
#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wunused-value"
#elif defined(__GNUC__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-value"
#elif defined(_MSC_VER)
// Older MSVC versions incorrectly warn on returning a reference to a temporary.
// This has been suppressed
# pragma warning(push)
# pragma warning(disable:4172)
#endif

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T,E>::value()
  & -> typename std::add_lvalue_reference<T>::type
{
  return (has_value() ||
    (detail::throw_bad_result_access(m_storage.storage.m_error), false),
    m_storage.storage.m_value
  );
}

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T,E>::value()
  && -> typename std::add_rvalue_reference<T>::type
{
  using reference = typename std::add_rvalue_reference<T>::type;

  return (has_value() ||
    (detail::throw_bad_result_access(static_cast<E&&>(m_storage.storage.m_error)), true),
    static_cast<reference>(m_storage.storage.m_value)
  );
}

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T,E>::value()
  const & -> typename std::add_lvalue_reference<typename std::add_const<T>::type>::type
{
  return (has_value() ||
    (detail::throw_bad_result_access(m_storage.storage.m_error), true),
    m_storage.storage.m_value
  );
}

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T,E>::value()
  const && -> typename std::add_rvalue_reference<typename std::add_const<T>::type>::type
{
  using reference = typename std::add_rvalue_reference<typename std::add_const<T>::type>::type;

  return (has_value() ||
    (detail::throw_bad_result_access(static_cast<const E&&>(m_storage.storage.m_error)), true),
    (static_cast<reference>(m_storage.storage.m_value))
  );
}

#if defined(__clang__)
# pragma clang diagnostic pop
#elif defined(__GNUC__)
# pragma GCC diagnostic pop
#elif defined(_MSC_VER)
# pragma warning(pop)
#endif

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T,E>::error() const &
  noexcept(std::is_nothrow_constructible<E>::value &&
           std::is_nothrow_copy_constructible<E>::value) -> E
{
  static_assert(
    std::is_default_constructible<E>::value,
    "E must be default-constructible if 'error()' checks are used. "
    "This is to allow for default-constructed error states to represent the "
    "'good' state"
  );

  return m_storage.storage.m_has_value
    ? E{}
    : m_storage.storage.m_error;
}

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T,E>::error() &&
  noexcept(std::is_nothrow_constructible<E>::value &&
           std::is_nothrow_move_constructible<E>::value) -> E
{
  static_assert(
    std::is_default_constructible<E>::value,
    "E must be default-constructible if 'error()' checks are used. "
    "This is to allow for default-constructed error states to represent the "
    "'good' state"
  );

  return m_storage.storage.m_has_value
    ? E{}
    : static_cast<E&&>(m_storage.storage.m_error);
}

//-----------------------------------------------------------------------------

template <typename T, typename E>
template <typename String, typename>
inline RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T,E>::expect(String&& message)
  const & -> void
{
  if (has_error()) {
    detail::throw_bad_result_access_message(
      detail::forward<String>(message),
      m_storage.storage.m_error
    );
  }
}

template <typename T, typename E>
template <typename String, typename>
inline RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T,E>::expect(String&& message)
  && -> void
{
  if (has_error()) {
    detail::throw_bad_result_access_message(
      detail::forward<String>(message),
      static_cast<E&&>(m_storage.storage.m_error)
    );
  }
}

//-----------------------------------------------------------------------------
// Monadic Functionalities
//-----------------------------------------------------------------------------

template <typename T, typename E>
template <typename U>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T, E>::value_or(U&& default_value)
  const& -> typename std::remove_reference<T>::type
{
  return m_storage.storage.m_has_value
    ? m_storage.storage.m_value
    : detail::forward<U>(default_value);
}

template <typename T, typename E>
template <typename U>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T, E>::value_or(U&& default_value)
  && -> typename std::remove_reference<T>::type
{
  return m_storage.storage.m_has_value
    ? static_cast<T&&>(**this)
    : detail::forward<U>(default_value);
}

template <typename T, typename E>
template <typename U>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T, E>::error_or(U&& default_error)
  const& -> error_type
{
  return m_storage.storage.m_has_value
    ? detail::forward<U>(default_error)
    : m_storage.storage.m_error;
}

template <typename T, typename E>
template <typename U>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T, E>::error_or(U&& default_error)
  && -> error_type
{
  return m_storage.storage.m_has_value
    ? detail::forward<U>(default_error)
    : static_cast<E&&>(m_storage.storage.m_error);
}

template <typename T, typename E>
template <typename U>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T, E>::and_then(U&& value)
  const -> result<typename std::decay<U>::type,E>
{
  return map([&value](const T&){
    return detail::forward<U>(value);
  });
}

//-----------------------------------------------------------------------------

template <typename T, typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T, E>::flat_map(Fn&& fn)
  const & -> detail::invoke_result_t<Fn, const T&>
{
  using result_type = detail::invoke_result_t<Fn, const T&>;

  static_assert(
    is_result<result_type>::value,
    "flat_map must return a result type or the program is ill-formed"
  );

  return has_value()
    ? detail::invoke(detail::forward<Fn>(fn), m_storage.storage.m_value)
    : result_type(in_place_error, m_storage.storage.m_error);
}

template <typename T, typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T, E>::flat_map(Fn&& fn)
  && -> detail::invoke_result_t<Fn, T&&>
{
  using result_type = detail::invoke_result_t<Fn, T&&>;

  static_assert(
    is_result<result_type>::value,
    "flat_map must return a result type or the program is ill-formed"
  );

  return has_value()
    ? detail::invoke(detail::forward<Fn>(fn), static_cast<T&&>(m_storage.storage.m_value))
    : result_type(in_place_error, static_cast<E&&>(m_storage.storage.m_error));
}

template <typename T, typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T, E>::map(Fn&& fn)
  const & -> result<detail::invoke_result_t<Fn,const T&>,E>
{
  using result_type = detail::invoke_result_t<Fn,const T&>;

  return map_impl(std::is_void<result_type>{}, detail::forward<Fn>(fn));
}

template <typename T, typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T, E>::map(Fn&& fn)
  && -> result<detail::invoke_result_t<Fn,T&&>,E>
{
  using result_type = detail::invoke_result_t<Fn,T&&>;

  return static_cast<result<T,E>&&>(*this).map_impl(
    std::is_void<result_type>{},
    detail::forward<Fn>(fn)
  );
}

//-----------------------------------------------------------------------------

template <typename T, typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T, E>::map_error(Fn&& fn)
  const & -> result<T, detail::invoke_result_t<Fn,const E&>>
{
  using result_type = result<T, detail::invoke_result_t<Fn, const E&>>;

  return has_error()
    ? result_type(in_place_error, detail::invoke(
      detail::forward<Fn>(fn), m_storage.storage.m_error
    ))
    : result_type(in_place, m_storage.storage.m_value);
}

template <typename T, typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T, E>::map_error(Fn&& fn)
  && -> result<T, detail::invoke_result_t<Fn,E&&>>
{
  using result_type = result<T, detail::invoke_result_t<Fn, E&&>>;

  return has_error()
    ? result_type(in_place_error, detail::invoke(
      detail::forward<Fn>(fn), static_cast<E&&>(m_storage.storage.m_error)
    ))
    : result_type(static_cast<T&&>(m_storage.storage.m_value));
}

template <typename T, typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T, E>::flat_map_error(Fn&& fn)
  const & -> detail::invoke_result_t<Fn, const E&>
{
  using result_type = detail::invoke_result_t<Fn, const E&>;

  static_assert(
    is_result<result_type>::value,
    "flat_map_error must return a result type or the program is ill-formed"
  );

  return has_value()
    ? result_type(in_place, m_storage.storage.m_value)
    : detail::invoke(detail::forward<Fn>(fn), m_storage.storage.m_error);
}

template <typename T, typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T, E>::flat_map_error(Fn&& fn)
  && -> detail::invoke_result_t<Fn, E&&>
{
  using result_type = detail::invoke_result_t<Fn, E&&>;

  static_assert(
    is_result<result_type>::value,
    "flat_map_error must return a result type or the program is ill-formed"
  );

  return has_value()
    ? result_type(in_place, static_cast<T&&>(m_storage.storage.m_value))
    : detail::invoke(detail::forward<Fn>(fn), static_cast<E&&>(m_storage.storage.m_error));
}

//-----------------------------------------------------------------------------
// Private Monadic Functions
//-----------------------------------------------------------------------------

template <typename T, typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T, E>::map_impl(std::true_type, Fn&& fn)
  const & -> result<void,E>
{
  using result_type = result<void, E>;

  return has_value()
    ? (detail::invoke(detail::forward<Fn>(fn), m_storage.storage.m_value), result_type{})
    : result_type(in_place_error, m_storage.storage.m_error);
}

template <typename T, typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<T, E>::map_impl(std::false_type, Fn&& fn)
  const & -> result<detail::invoke_result_t<Fn,const T&>,E>
{
  using invoke_result_type = detail::invoke_result_t<Fn,const T&>;
  using result_type = result<invoke_result_type, E>;

  return has_value()
    ? result_type(in_place, detail::invoke(
      detail::forward<Fn>(fn), m_storage.storage.m_value
    ))
    : result_type(in_place_error, m_storage.storage.m_error);
}

template <typename T, typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T, E>::map_impl(std::true_type, Fn&& fn)
  && -> result<void,E>
{
  using result_type = result<void, E>;

  return has_value()
    ? (detail::invoke(
        detail::forward<Fn>(fn), static_cast<T&&>(m_storage.storage.m_value)
      ), result_type{})
    : result_type(in_place_error, static_cast<E&&>(m_storage.storage.m_error));
}

template <typename T, typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<T, E>::map_impl(std::false_type, Fn&& fn)
  && -> result<detail::invoke_result_t<Fn,T&&>,E>
{
  using invoke_result_type = detail::invoke_result_t<Fn,T&&>;
  using result_type = result<invoke_result_type, E>;

  return has_value()
    ? result_type(in_place, detail::invoke(
      detail::forward<Fn>(fn), static_cast<T&&>(m_storage.storage.m_value)
    ))
    : result_type(in_place_error, static_cast<E&&>(m_storage.storage.m_error));
}

//=============================================================================
// class : result<void,E>
//=============================================================================

//-----------------------------------------------------------------------------
// Constructor / Assignment
//-----------------------------------------------------------------------------

template <typename E>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<void, E>::result()
  noexcept
  : m_storage(in_place)
{

}

template <typename E>
template <typename U, typename E2, typename>
inline RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::result<void, E>::result(const result<U,E2>& other)
  noexcept(std::is_nothrow_constructible<E,const E2&>::value)
  : m_storage(detail::unit{})
{
  m_storage.construct_error_from_result(
    static_cast<const result<U,E2>&>(other).m_storage
  );
}

template <typename E>
template <typename U, typename E2, typename>
inline RESULT_INLINE_VISIBILITY
RESULT_NS_IMPL::result<void, E>::result(result<U,E2>&& other)
  noexcept(std::is_nothrow_constructible<E,E2&&>::value)
  : m_storage(detail::unit{})
{
  m_storage.construct_error_from_result(
    static_cast<result<U,E2>&&>(other).m_storage
  );
}


//-----------------------------------------------------------------------------

template <typename E>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<void, E>::result(in_place_t)
  noexcept
  : m_storage(in_place)
{

}

template <typename E>
template <typename...Args, typename>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<void, E>::result(in_place_error_t, Args&&...args)
  noexcept(std::is_nothrow_constructible<E, Args...>::value)
  : m_storage(in_place_error, detail::forward<Args>(args)...)
{

}

template <typename E>
template <typename U, typename...Args, typename>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<void, E>::result(in_place_error_t,
                                        std::initializer_list<U> ilist,
                                        Args&&...args)
  noexcept(std::is_nothrow_constructible<E, std::initializer_list<U>, Args...>::value)
  : m_storage(in_place_error, ilist, detail::forward<Args>(args)...)
{

}

//-----------------------------------------------------------------------------

template <typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<void, E>::result(const failure<E2>& e)
  noexcept(std::is_nothrow_constructible<E,const E2&>::value)
  : m_storage(in_place_error, e.error())
{

}

template <typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<void, E>::result(failure<E2>&& e)
  noexcept(std::is_nothrow_constructible<E,E2&&>::value)
  : m_storage(in_place_error, static_cast<E2&&>(e.error()))
{

}

//-----------------------------------------------------------------------------

template <typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::result<void, E>::operator=(const result<void,E2>& other)
  noexcept(std::is_nothrow_assignable<E, const E2&>::value)
  -> result&
{
  m_storage.assign_from_result(other.m_storage);
  return (*this);
}

template <typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::result<void, E>::operator=(result<void,E2>&& other)
  noexcept(std::is_nothrow_assignable<E, E2&&>::value)
  -> result&
{
  m_storage.assign_from_result(static_cast<result<void,E2>&&>(other).m_storage);
  return (*this);
}

template <typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::result<void, E>::operator=(const failure<E2>& other)
  noexcept(std::is_nothrow_assignable<E, const E2&>::value)
  -> result&
{
  m_storage.assign_error(other.error());
  return (*this);
}

template <typename E>
template <typename E2, typename>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::result<void, E>::operator=(failure<E2>&& other)
  noexcept(std::is_nothrow_assignable<E, E2&&>::value)
  -> result&
{
  m_storage.assign_error(static_cast<E2&&>(other.error()));
  return (*this);
}

//-----------------------------------------------------------------------------
// Observers
//-----------------------------------------------------------------------------

template <typename E>
inline RESULT_INLINE_VISIBILITY constexpr
RESULT_NS_IMPL::result<void, E>::operator bool()
  const noexcept
{
  return has_value();
}

template <typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<void, E>::has_value()
  const noexcept -> bool
{
  return m_storage.storage.m_has_value;
}

template <typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<void, E>::has_error()
  const noexcept -> bool
{
  return !has_value();
}

//-----------------------------------------------------------------------------

template <typename E>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<void, E>::value()
  const & -> void
{
  static_cast<void>(
    has_value() ||
    (detail::throw_bad_result_access(m_storage.storage.m_error), true)
  );
}

template <typename E>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<void, E>::value()
  && -> void
{
  static_cast<void>(
    has_value() ||
    (detail::throw_bad_result_access(static_cast<E&&>(m_storage.storage.m_error)), true)
  );
}

template <typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<void, E>::error()
  const &
  noexcept(std::is_nothrow_constructible<E>::value &&
           std::is_nothrow_copy_constructible<E>::value) -> E
{
  return has_value() ? E{} : m_storage.storage.m_error;
}

template <typename E>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<void, E>::error()
  && noexcept(std::is_nothrow_constructible<E>::value &&
              std::is_nothrow_copy_constructible<E>::value) -> E
{
  return has_value() ? E{} : static_cast<E&&>(m_storage.storage.m_error);
}

//-----------------------------------------------------------------------------

template <typename E>
template <typename String, typename>
inline RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<void,E>::expect(String&& message)
  const & -> void
{
  if (has_error()) {
    detail::throw_bad_result_access_message(
      detail::forward<String>(message),
      m_storage.storage.m_error
    );
  }
}

template <typename E>
template <typename String, typename>
inline RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<void,E>::expect(String&& message)
  && -> void
{
  if (has_error()) {
    detail::throw_bad_result_access_message(
      detail::forward<String>(message),
      static_cast<E&&>(m_storage.storage.m_error)
    );
  }
}

//-----------------------------------------------------------------------------
// Monadic Functionalities
//-----------------------------------------------------------------------------

template <typename E>
template <typename U>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<void, E>::error_or(U&& default_error)
  const & -> error_type
{
  return has_value()
    ? detail::forward<U>(default_error)
    : m_storage.storage.m_error;
}

template <typename E>
template <typename U>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<void, E>::error_or(U&& default_error)
  && -> error_type
{
  return has_value()
    ? detail::forward<U>(default_error)
    : static_cast<E&&>(m_storage.storage.m_error);
}

template <typename E>
template <typename U>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<void, E>::and_then(U&& value)
  const -> result<typename std::decay<U>::type,E>
{
  return map([&value]{
    return detail::forward<U>(value);
  });
}

//-----------------------------------------------------------------------------

template <typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<void, E>::flat_map(Fn&& fn)
  const & -> detail::invoke_result_t<Fn>
{
  using result_type = detail::invoke_result_t<Fn>;

  static_assert(
    is_result<result_type>::value,
    "flat_map must return a result type or the program is ill-formed"
  );

  return has_value()
    ? detail::invoke(detail::forward<Fn>(fn))
    : result_type(in_place_error, m_storage.storage.m_error);
}

template <typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<void, E>::flat_map(Fn&& fn)
  && -> detail::invoke_result_t<Fn>
{
  using result_type = detail::invoke_result_t<Fn>;

  static_assert(
    is_result<result_type>::value,
    "flat_map must return a result type or the program is ill-formed"
  );

  return has_value()
    ? detail::invoke(detail::forward<Fn>(fn))
    : result_type(in_place_error, static_cast<E&&>(m_storage.storage.m_error));
}

template <typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<void, E>::map(Fn&& fn)
  const & -> result<detail::invoke_result_t<Fn>,E>
{
  using result_type = detail::invoke_result_t<Fn>;

  return map_impl(std::is_void<result_type>{}, detail::forward<Fn>(fn));
}

template <typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<void, E>::map(Fn&& fn)
  && -> result<detail::invoke_result_t<Fn>,E>
{
  using result_type = detail::invoke_result_t<Fn>;

  return static_cast<result<void,E>&&>(*this).map_impl(
    std::is_void<result_type>{},
    detail::forward<Fn>(fn)
  );
}

//-----------------------------------------------------------------------------

template <typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<void, E>::map_error(Fn&& fn)
  const & -> result<void, detail::invoke_result_t<Fn,const E&>>
{
  using result_type = result<void, detail::invoke_result_t<Fn, const E&>>;

  return has_value()
    ? result_type{}
    : result_type(in_place_error, detail::invoke(
      detail::forward<Fn>(fn), m_storage.storage.m_error
    ));
}

template <typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<void, E>::map_error(Fn&& fn)
  && -> result<void, detail::invoke_result_t<Fn,E&&>>
{
  using result_type = result<void, detail::invoke_result_t<Fn, E&&>>;

  return has_value()
    ? result_type{}
    : result_type(in_place_error,
      detail::invoke(detail::forward<Fn>(fn), static_cast<E&&>(m_storage.storage.m_error)
    ));
}

template <typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<void, E>::flat_map_error(Fn&& fn)
  const & -> detail::invoke_result_t<Fn,const E&>
{
  using result_type = detail::invoke_result_t<Fn,const E&>;

  static_assert(
    is_result<result_type>::value,
    "flat_map_error must return a result type or the program is ill-formed"
  );
  static_assert(
    std::is_default_constructible<typename result_type::value_type>::value,
    "flat_map_error for result<void,E> requires the new T type to be default-"
    "constructible"
  );

  return has_value()
    ? result_type{}
    : detail::invoke(detail::forward<Fn>(fn), m_storage.storage.m_error);
}

template <typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<void, E>::flat_map_error(Fn&& fn)
  && -> detail::invoke_result_t<Fn,E&&>
{
  using result_type = detail::invoke_result_t<Fn,E&&>;

  static_assert(
    is_result<result_type>::value,
    "flat_map_error must return a result type or the program is ill-formed"
  );
  static_assert(
    std::is_default_constructible<typename result_type::value_type>::value,
    "flat_map_error for result<void,E> requires the new T type to be default-"
    "constructible"
  );

  return has_value()
    ? result_type{}
    : detail::invoke(detail::forward<Fn>(fn), static_cast<E&&>(m_storage.storage.m_error));
}

//-----------------------------------------------------------------------------
// Private Monadic Functions
//-----------------------------------------------------------------------------

template <typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<void, E>::map_impl(std::true_type, Fn&& fn)
  const & -> result<void,E>
{
  using result_type = result<void, E>;

  return has_value()
    ? (detail::invoke(detail::forward<Fn>(fn)), result_type{})
    : result_type(in_place_error, m_storage.storage.m_error);
}

template <typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::result<void, E>::map_impl(std::false_type, Fn&& fn)
  const & -> result<detail::invoke_result_t<Fn>,E>
{
  using invoke_result_type = detail::invoke_result_t<Fn>;
  using result_type = result<invoke_result_type, E>;

  return has_value()
    ? result_type(in_place, detail::invoke(detail::forward<Fn>(fn)))
    : result_type(in_place_error, m_storage.storage.m_error);
}

template <typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<void, E>::map_impl(std::true_type, Fn&& fn)
  && -> result<void,E>
{
  using result_type = result<void, E>;

  return has_value()
    ? (detail::invoke(detail::forward<Fn>(fn)), result_type{})
    : result_type(in_place_error, static_cast<E&&>(m_storage.storage.m_error));
}

template <typename E>
template <typename Fn>
inline RESULT_INLINE_VISIBILITY RESULT_CPP14_CONSTEXPR
auto RESULT_NS_IMPL::result<void, E>::map_impl(std::false_type, Fn&& fn)
  && -> result<detail::invoke_result_t<Fn>,E>
{
  using invoke_result_type = detail::invoke_result_t<Fn>;
  using result_type = result<invoke_result_type, E>;

  return has_value()
    ? result_type(in_place, detail::invoke(detail::forward<Fn>(fn)))
    : result_type(in_place_error, static_cast<E&&>(m_storage.storage.m_error));
}

//=============================================================================
// non-member functions : class : result
//=============================================================================

//-----------------------------------------------------------------------------
// Comparison
//-----------------------------------------------------------------------------

template <typename T1, typename E1, typename T2, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator==(const result<T1,E1>& lhs,
                                const result<T2,E2>& rhs)
  noexcept -> bool
{
  return (lhs.has_value() == rhs.has_value())
    ? (
        lhs.has_value()
        ? *lhs == *rhs
        : detail::extract_error(lhs) == detail::extract_error(rhs)
      )
    : false;
}

template <typename T1, typename E1, typename T2, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator!=(const result<T1,E1>& lhs,
                                const result<T2,E2>& rhs)
  noexcept -> bool
{
  return (lhs.has_value() == rhs.has_value())
    ? (
        lhs.has_value()
        ? *lhs != *rhs
        : detail::extract_error(lhs) != detail::extract_error(rhs)
      )
    : true;
}

template <typename T1, typename E1, typename T2, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator>=(const result<T1,E1>& lhs,
                                const result<T2,E2>& rhs)
  noexcept -> bool
{
  return (lhs.has_value() == rhs.has_value())
    ? (
        lhs.has_value()
        ? *lhs >= *rhs
        : detail::extract_error(lhs) >= detail::extract_error(rhs)
      )
    : static_cast<int>(static_cast<bool>(lhs)) >= static_cast<int>(static_cast<bool>(rhs));
}

template <typename T1, typename E1, typename T2, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator<=(const result<T1,E1>& lhs,
                                const result<T2,E2>& rhs)
  noexcept -> bool
{
  return (lhs.has_value() == rhs.has_value())
    ? (
        lhs.has_value()
        ? *lhs <= *rhs
        : detail::extract_error(lhs) <= detail::extract_error(rhs)
      )
    : static_cast<int>(static_cast<bool>(lhs)) <= static_cast<int>(static_cast<bool>(rhs));
}

template <typename T1, typename E1, typename T2, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator>(const result<T1,E1>& lhs,
                                 const result<T2,E2>& rhs)
  noexcept -> bool
{
  return (lhs.has_value() == rhs.has_value())
    ? (
        lhs.has_value()
        ? *lhs > *rhs
        : detail::extract_error(lhs) > detail::extract_error(rhs)
      )
    : static_cast<int>(static_cast<bool>(lhs)) > static_cast<int>(static_cast<bool>(rhs));
}

template <typename T1, typename E1, typename T2, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator<(const result<T1,E1>& lhs,
                                 const result<T2,E2>& rhs)
  noexcept -> bool
{
  return (lhs.has_value() == rhs.has_value())
    ? (
        lhs.has_value()
        ? *lhs < *rhs
        : detail::extract_error(lhs) < detail::extract_error(rhs)
      )
    : static_cast<int>(static_cast<bool>(lhs)) < static_cast<int>(static_cast<bool>(rhs));
}


//-----------------------------------------------------------------------------

template <typename E1, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator==(const result<void,E1>& lhs,
                                const result<void,E2>& rhs)
  noexcept -> bool
{
  return lhs.has_value() == rhs.has_value()
    ? (
        lhs.has_value()
        ? true
        : detail::extract_error(lhs) == detail::extract_error(rhs)
      )
    : false;
}

template <typename E1, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator!=(const result<void,E1>& lhs,
                                const result<void,E2>& rhs)
  noexcept -> bool
{
  return lhs.has_value() == rhs.has_value()
    ? (
        lhs.has_value()
        ? false
        : detail::extract_error(lhs) != detail::extract_error(rhs)
      )
    : true;
}

template <typename E1, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator>=(const result<void,E1>& lhs,
                                const result<void,E2>& rhs)
  noexcept -> bool
{
  return lhs.has_value() == rhs.has_value()
    ? (
        lhs.has_value()
        ? true
        : detail::extract_error(lhs) >= detail::extract_error(rhs)
      )
    : static_cast<int>(static_cast<bool>(lhs)) >= static_cast<int>(static_cast<bool>(rhs));
}

template <typename E1, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator<=(const result<void,E1>& lhs,
                                const result<void,E2>& rhs)
  noexcept -> bool
{
  return lhs.has_value() == rhs.has_value()
    ? (
        lhs.has_value()
        ? true
        : detail::extract_error(lhs) <= detail::extract_error(rhs)
      )
    : static_cast<int>(static_cast<bool>(lhs)) <= static_cast<int>(static_cast<bool>(rhs));
}

template <typename E1, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator>(const result<void,E1>& lhs,
                               const result<void,E2>& rhs)
  noexcept -> bool
{
  return lhs.has_value() == rhs.has_value()
    ? (
        lhs.has_value()
        ? false
        : detail::extract_error(lhs) > detail::extract_error(rhs)
      )
    : static_cast<int>(static_cast<bool>(lhs)) > static_cast<int>(static_cast<bool>(rhs));
}

template <typename E1, typename E2>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator<(const result<void,E1>& lhs,
                               const result<void,E2>& rhs)
  noexcept -> bool
{
  return lhs.has_value() == rhs.has_value()
    ? (
        lhs.has_value()
        ? false
        : detail::extract_error(lhs) < detail::extract_error(rhs)
      )
    : static_cast<int>(static_cast<bool>(lhs)) < static_cast<int>(static_cast<bool>(rhs));
}


//-----------------------------------------------------------------------------

template <typename T, typename E, typename U, typename>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator==(const result<T,E>& exp, const U& value)
  noexcept -> bool
{
  return (exp.has_value() && *exp == value);
}

template <typename T, typename U, typename E, typename>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator==(const T& value, const result<U,E>& exp)
  noexcept -> bool
{
  return (exp.has_value() && *exp == value);
}

template <typename T, typename E, typename U, typename>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator!=(const result<T,E>& exp, const U& value)
  noexcept -> bool
{
  return exp.has_value() ? *exp != value : true;
}

template <typename T, typename U, typename E, typename>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator!=(const T& value, const result<U,E>& exp)
  noexcept -> bool
{
  return exp.has_value() ? value != *exp : true;
}

template <typename T, typename E, typename U, typename>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator<=(const result<T,E>& exp, const U& value)
  noexcept -> bool
{
  return exp.has_value() ? *exp <= value : false;
}

template <typename T, typename U, typename E, typename>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator<=(const T& value, const result<U,E>& exp)
  noexcept -> bool
{
  return exp.has_value() ? value <= *exp : true;
}

template <typename T, typename E, typename U, typename>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator>=(const result<T,E>& exp, const U& value)
  noexcept -> bool
{
  return exp.has_value() ? *exp >= value : true;
}

template <typename T, typename U, typename E, typename>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator>=(const T& value, const result<U,E>& exp)
  noexcept -> bool
{
  return exp.has_value() ? value >= *exp : false;
}

template <typename T, typename E, typename U, typename>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator<(const result<T,E>& exp, const U& value)
  noexcept -> bool
{
  return exp.has_value() ? *exp < value : false;
}

template <typename T, typename U, typename E, typename>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator<(const T& value, const result<U,E>& exp)
  noexcept -> bool
{
  return exp.has_value() ? value < *exp : true;
}

template <typename T, typename E, typename U, typename>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator>(const result<T,E>& exp, const U& value)
  noexcept -> bool
{
  return exp.has_value() ? *exp > value : false;
}

template <typename T, typename U, typename E, typename>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator>(const T& value, const result<U,E>& exp)
  noexcept -> bool
{
  return exp.has_value() ? value > *exp : true;
}

//-----------------------------------------------------------------------------

template <typename T, typename E, typename U>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator==(const result<T,E>& exp, const failure<U>& error)
  noexcept -> bool
{
  return exp.has_error() ? detail::extract_error(exp) == error.error() : false;
}

template <typename T, typename U, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator==(const failure<T>& error, const result<E,U>& exp)
  noexcept -> bool
{
  return exp.has_error() ? error.error() == detail::extract_error(exp) : false;
}

template <typename T, typename E, typename U>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator!=(const result<T,E>& exp, const failure<U>& error)
  noexcept -> bool
{
  return exp.has_error() ? detail::extract_error(exp) != error.error() : true;
}

template <typename T, typename U, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator!=(const failure<T>& error, const result<E,U>& exp)
  noexcept -> bool
{
  return exp.has_error() ? error.error() != detail::extract_error(exp) : true;
}

template <typename T, typename E, typename U>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator<=(const result<T,E>& exp, const failure<U>& error)
  noexcept -> bool
{
  return exp.has_error() ? detail::extract_error(exp) <= error.error() : true;
}

template <typename T, typename U, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator<=(const failure<T>& error, const result<E,U>& exp)
  noexcept -> bool
{
  return exp.has_error() ? error.error() <= detail::extract_error(exp) : false;
}

template <typename T, typename E, typename U>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator>=(const result<T,E>& exp, const failure<U>& error)
  noexcept -> bool
{
  return exp.has_error() ? detail::extract_error(exp) >= error.error() : false;
}

template <typename T, typename U, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator>=(const failure<T>& error, const result<E,U>& exp)
  noexcept -> bool
{
  return exp.has_error() ? error.error() >= detail::extract_error(exp) : true;
}

template <typename T, typename E, typename U>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator<(const result<T,E>& exp, const failure<U>& error)
  noexcept -> bool
{
  return exp.has_error() ? detail::extract_error(exp) < error.error() : true;
}

template <typename T, typename U, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator<(const failure<T>& error, const result<E,U>& exp)
  noexcept -> bool
{
  return exp.has_error() ? error.error() < detail::extract_error(exp) : false;
}

template <typename T, typename E, typename U>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator>(const result<T,E>& exp, const failure<U>& error)
  noexcept -> bool
{
  return exp.has_error() ? detail::extract_error(exp) > error.error() : false;
}

template <typename T, typename U, typename E>
inline RESULT_INLINE_VISIBILITY constexpr
auto RESULT_NS_IMPL::operator>(const failure<T>& error, const result<E,U>& exp)
  noexcept -> bool
{
  return exp.has_error() ? error.error() > detail::extract_error(exp) : true;
}

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

template <typename T, typename E>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::swap(result<T,E>& lhs, result<T,E>& rhs)
#if __cplusplus >= 201703L
  noexcept(std::is_nothrow_move_constructible<result<T,E>>::value &&
           std::is_nothrow_move_assignable<result<T,E>>::value &&
           std::is_nothrow_swappable<T>::value &&
           std::is_nothrow_swappable<E>::value)
#else
  noexcept(std::is_nothrow_move_constructible<result<T,E>>::value &&
           std::is_nothrow_move_assignable<result<T,E>>::value)
#endif
  -> void
{
  using std::swap;

  if (lhs.has_value() == rhs.has_value()) {
    if (lhs.has_value()) {
      swap(*lhs, *rhs);
    } else {
      auto& lhs_error = detail::result_error_extractor::get(lhs);
      auto& rhs_error = detail::result_error_extractor::get(rhs);

      swap(lhs_error, rhs_error);
    }
    // If both `result`s contain values, do nothing
  } else {
    auto temp = static_cast<result<T,E>&&>(lhs);
    lhs = static_cast<result<T,E>&&>(rhs);
    rhs = static_cast<result<T,E>&&>(temp);
  }
}

template <typename E>
inline RESULT_INLINE_VISIBILITY
auto RESULT_NS_IMPL::swap(result<void,E>& lhs, result<void,E>& rhs)
#if __cplusplus >= 201703L
  noexcept(std::is_nothrow_move_constructible<result<void,E>>::value &&
           std::is_nothrow_move_assignable<result<void,E>>::value &&
           std::is_nothrow_swappable<E>::value)
#else
  noexcept(std::is_nothrow_move_constructible<result<void,E>>::value &&
           std::is_nothrow_move_assignable<result<void,E>>::value)
#endif
   -> void
{
  using std::swap;

  if (lhs.has_value() == rhs.has_value()) {
    if (lhs.has_error()) {
      auto& lhs_error = detail::result_error_extractor::get(lhs);
      auto& rhs_error = detail::result_error_extractor::get(rhs);

      swap(lhs_error, rhs_error);
    }
    // If both `result`s contain values, do nothing
  } else {
    auto temp = static_cast<result<void,E>&&>(lhs);
    lhs = static_cast<result<void,E>&&>(rhs);
    rhs = static_cast<result<void,E>&&>(temp);
  }
}

#if defined(__clang__)
# pragma clang diagnostic pop
#endif

#undef RESULT_NAMESPACE_INTERNAL
#undef RESULT_NS_IMPL
#undef RESULT_CPP14_CONSTEXPR
#undef RESULT_CPP17_INLINE
#undef RESULT_INLINE_VISIBILITY
#undef RESULT_NODISCARD
#undef RESULT_WARN_UNUSED

#endif /* RESULT_RESULT_HPP */
