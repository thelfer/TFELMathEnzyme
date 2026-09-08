/*!
 * \file   TFEL/Math/Enzyme/Internals/FunctionUtilities.hxx
 * \brief
 * \author Thomas Helfer
 * \date   09/08/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_INTERNALS_FUNCTIONUTILITIES_HXX
#define LIB_TFEL_MATH_ENZYME_INTERNALS_FUNCTIONUTILITIES_HXX

#include "TFEL/Math/Enzyme/Variable.hxx"
#include "TFEL/Math/Enzyme/Internals/TypeList.hxx"
#include "TFEL/Math/Enzyme/Internals/Enzyme.hxx"

namespace tfel::math::enzyme::internals {

  /*!
   * \brief Convert a value to an Enzyme-compatible argument type
   * \tparam ExpectedType: the expected type for Enzyme
   * \tparam T: the input type
   * \param[in] v: the value to convert
   * \return an Enzyme-compatible argument (Const, Duplicated, or the original
   * value)
   */
  template <typename ExpectedType, typename T>
  auto convertToEnzymeArgument(T&& v) {
    if constexpr (isEnzymeType<T>) {
      static_assert(
          std::same_as<ExpectedType, typename EnzymeValueType<T>::type>,
          "invalid Enzyme argument");
      return v;
    } else if constexpr (isVariableValueAndIncrement<T>()) {
      return ::enzyme::Duplicated<ExpectedType>{v.value, v.increment};
    } else {
      return ::enzyme::Const<ExpectedType>(v);
    }
  }  // end of convertToEnzymeArgument

  /*!
   * \brief Check if a type has a call operator
   * \tparam CallableType: the type to check
   * \return true if the type has a call operator
   */
  template <typename CallableType>
  constexpr bool hasCallOperator() noexcept {
    return requires { &CallableType::operator(); };
  }

  /*!
   * \brief Type trait to check if a type is a function type
   * \tparam T: the type to check
   */
  template <typename T>
  struct IsFunction : std::false_type {};

  /*!
   * \brief Specialization for function types
   * \tparam ResultType: the return type
   * \tparam ArgumentsTypes: the argument types
   */
  template <typename ResultType, typename... ArgumentsTypes>
  struct IsFunction<ResultType(ArgumentsTypes...)> : std::true_type {};

  /*!
   * \brief Concept to check if a type is a function type
   * \tparam T: the type to check
   */
  template <typename T>
  concept IsFunctionConcept = IsFunction<T>::value;

  /*!
   * \brief Check if a type is a function type
   * \tparam T: the type to check
   * \return true if T is a function type
   */
  template <typename T>
  constexpr bool isFunction() noexcept {
    return IsFunction<T>::value;
  }

  /*!
   * \brief Type trait to check if a type is a function pointer
   * \tparam T: the type to check
   */
  template <typename T>
  struct IsFunctionPointer : std::false_type {};

  /*!
   * \brief Specialization for function pointer types
   * \tparam ResultType: the return type
   * \tparam ArgumentsTypes: the argument types
   */
  template <typename ResultType, typename... ArgumentsTypes>
  struct IsFunctionPointer<ResultType (*)(ArgumentsTypes...)> : std::true_type {
  };

  /*!
   * \brief Check if a type is a function pointer
   * \tparam T: the type to check
   * \return true if T is a function pointer
   */
  template <typename T>
  constexpr bool isFunctionPointer() noexcept {
    return IsFunctionPointer<T>::value;
  }

  /*!
   * \brief Concept to check if a type is a function pointer
   * \tparam T: the type to check
   */
  template <typename T>
  concept IsFunctionPointerConcept = IsFunctionPointer<T>::value;

  /*!
   * \brief Wrapper for function pointers to be used as callable objects
   * \tparam Func: the function pointer
   */
  template <IsFunctionPointerConcept auto Func>
  struct FunctionWrapper {};

  /*!
   * \brief Primary template for function traits
   * \tparam CallableType: the callable type
   */
  template <typename CallableType>
  struct FunctionTraits;

  /*!
   * \brief Specialization for function pointers
   * \tparam ResultType: the return type
   * \tparam ArgumentsTypes: the argument types
   */
  template <typename ResultType, typename... ArgumentsTypes>
  struct FunctionTraits<ResultType (*)(ArgumentsTypes...)> {
    /*!
     * \brief The type list of argument types
     */
    using type = TypeList<ArgumentsTypes...>;
  };

  /*!
   * \brief Specialization for function types
   * \tparam ResultType: the return type
   * \tparam ArgumentsTypes: the argument types
   */
  template <typename ResultType, typename... ArgumentsTypes>
  struct FunctionTraits<ResultType(ArgumentsTypes...)> {
    /*!
     * \brief The type list of argument types
     */
    using type = TypeList<ArgumentsTypes...>;
  };

  /*!
   * \brief Specialization for member function pointers
   * \tparam ResultType: the return type
   * \tparam Obj: the object type
   * \tparam ArgumentsTypes: the argument types
   */
  template <typename ResultType, typename Obj, typename... ArgumentsTypes>
  struct FunctionTraits<ResultType (Obj::*)(ArgumentsTypes...)> {
    /*!
     * \brief The type list of argument types
     */
    using type = TypeList<ArgumentsTypes...>;
  };

  /*!
   * \brief Specialization for const member function pointers
   * \tparam ResultType: the return type
   * \tparam Obj: the object type
   * \tparam ArgumentsTypes: the argument types
   */
  template <typename ResultType, typename Obj, typename... ArgumentsTypes>
  struct FunctionTraits<ResultType (Obj::*)(ArgumentsTypes...) const> {
    /*!
     * \brief The type list of argument types
     */
    using type = TypeList<ArgumentsTypes...>;
  };

  /*!
   * \brief Specialization for callable objects (functor-like)
   * \tparam CallableType: the callable type
   */
  template <typename CallableType>
  struct FunctionTraits : FunctionTraits<decltype(&CallableType::operator())> {
  };

  /*!
   * \brief Get the argument types of a callable as a TypeList
   * \tparam CallableType: the callable type
   * \return a TypeList containing the argument types
   */
  template <typename CallableType>
  auto getArgumentsList() {
    using List =
        typename FunctionTraits<std::remove_reference_t<CallableType>>::type;
    return List{};
  }

  /*!
   * \brief Get the number of arguments of a callable
   * \tparam CallableType: the callable type
   * \return the number of arguments
   */
  template <typename CallableType>
  constexpr std::size_t getArgumentsSize() noexcept {
    using List = typename FunctionTraits<CallableType>::type;
    return getTypeListSize(List{});
  }

  /*!
   * \brief A simple concept to select a suitable callable, i.e. a type which
   * has a call operator but is not a function, nor a function pointer.
   *
   * \note Regular functions are treated separately due to the way Enzyme works:
   * they must be passed as template parameters and not by pointer, otherwise,
   * the symbol associated with the function is not seen by Enzyme.
   * \tparam CallableType: the type to check
   */
  template <typename CallableType>
  concept EnzymeCallableConcept =
      (!isFunction<CallableType>()) && (!isFunctionPointer<CallableType>()) &&
      (hasCallOperator<std::remove_reference_t<CallableType>>());

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  /*!
   * \brief an inline variable used to generate function wrappers
   * \tparam F: the function pointer to wrap
   */
  template <internals::IsFunctionPointerConcept auto F>
  constexpr inline auto function = internals::FunctionWrapper<F>{};

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_INTERNALS_FUNCTIONUTILITIES_HXX */
