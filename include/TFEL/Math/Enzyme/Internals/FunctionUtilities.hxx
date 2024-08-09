/*!
 * \file   TFEL/Math/Enzyme/Internals/FunctionUtilities.hxx
 * \brief    
 * \author Thomas Helfer
 * \date   09/08/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_INTERNALS_FUNCTIONUTILITIES_HXX
#define LIB_TFEL_MATH_ENZYME_INTERNALS_FUNCTIONUTILITIES_HXX

#include "TFEL/Math/Enzyme/Internals/TypeList.hxx"

namespace tfel::math::enzyme::internals {

  template <typename CallableType>
  constexpr bool hasCallOperator() noexcept {
    return requires { &CallableType::operator(); };
  }

  template <typename T>
  struct IsFunction : std::false_type {};

  template <typename ResultType, typename... ArgumentsTypes>
  struct IsFunction<ResultType(ArgumentsTypes...)> : std::true_type {};

  template <typename T>
  concept IsFunctionConcept = IsFunction<T>::value;

  template <typename T>
  constexpr bool isFunction() noexcept {
    return IsFunction<T>::value;
  }

  template <typename T>
  struct IsFunctionPointer : std::false_type {};

  template <typename ResultType, typename... ArgumentsTypes>
  struct IsFunctionPointer<ResultType (*)(ArgumentsTypes...)> : std::true_type {
  };

  template <typename T>
  constexpr bool isFunctionPointer() noexcept {
    return IsFunctionPointer<T>::value;
  }

  template <typename T>
  concept IsFunctionPointerConcept = IsFunctionPointer<T>::value;

  template <IsFunctionPointerConcept auto Func>
  struct FunctionWrapper {};

  template <typename CallableType>
  struct FunctionTraits;

  template <typename ResultType, typename... ArgumentsTypes>
  struct FunctionTraits<ResultType (*)(ArgumentsTypes...)> {
    using type = TypeList<ArgumentsTypes...>;
  };

  template <typename ResultType, typename... ArgumentsTypes>
  struct FunctionTraits<ResultType(ArgumentsTypes...)> {
    using type = TypeList<ArgumentsTypes...>;
  };

  template <typename ResultType, typename Obj, typename... ArgumentsTypes>
  struct FunctionTraits<ResultType (Obj::*)(ArgumentsTypes...)> {
    using type = TypeList<ArgumentsTypes...>;
  };

  template <typename ResultType, typename Obj, typename... ArgumentsTypes>
  struct FunctionTraits<ResultType (Obj::*)(ArgumentsTypes...) const> {
    using type = TypeList<ArgumentsTypes...>;
  };

  template <typename CallableType>
  struct FunctionTraits : FunctionTraits<decltype(&CallableType::operator())> {};

  template <typename CallableType>
  auto getArgumentsList() {
    using List = typename FunctionTraits<CallableType>::type;
    return List{};
  }

  template <typename CallableType>
  constexpr std::size_t getArgumentsSize() noexcept {
    using List = typename FunctionTraits<CallableType>::type;
    return getTypeListSize(List{});
  }

}  // end of namespace tfel::math::enzyme::internals

#endif /* LIB_TFEL_MATH_ENZYME_INTERNALS_FUNCTIONUTILITIES_HXX */
