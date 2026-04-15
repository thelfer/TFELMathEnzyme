/*!
 * \file   TFEL/Math/Enzyme/Internals/Enzyme.hxx
 * \brief  This header declares some functions and variables
 *         required by enzyme.
 * \author Thomas Helfer
 * \date   08/07/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_INTERNALS_ENZYME_HXX
#define LIB_TFEL_MATH_ENZYME_INTERNALS_ENZYME_HXX

#include <utility>
#include <type_traits>
#include <enzyme/enzyme>
#include "TFEL/Math/Enzyme/Internals/TypeList.hxx"

namespace tfel::math::enzyme::internals {

  template <typename T>
  inline constexpr bool isEnzymeType = false;

  template <typename T>
  inline constexpr bool isEnzymeType<::enzyme::Const<T>> = true;

  template <typename T>
  inline constexpr bool isEnzymeType<::enzyme::Duplicated<T>> = true;

  template <typename T>
  struct EnzymeValueType;

  template <typename T>
  struct EnzymeValueType<::enzyme::Const<T>> {
    using type = T;
  };

  template <typename T>
  struct EnzymeValueType<::enzyme::Duplicated<T>> {
    using type = T;
  };

  template <typename F, typename L>
  inline constexpr auto callable_wrapper = nullptr;

  template <typename F, typename... Args>
  inline constexpr auto callable_wrapper<F, TypeList<Args...>> =
      [](std::conditional_t<std::is_const_v<F>,  //
                            const void* const,
                            void* const> ptr,
         const Args... args) { return (*reinterpret_cast<F*>(ptr))(args...); };

  template <typename SourceType, typename DestinationType>
  struct IsConvertible : std::is_convertible<SourceType, DestinationType> {};

  template <typename SourceType, typename DestinationType>
  constexpr bool isConvertible() noexcept {
    return IsConvertible<SourceType, DestinationType>::value;
  }  // end of isConvertible

  //   template <typename ExpectedCallableArgumentType, typename InputType>
  //   auto convertToEnzymeArgument(InputType&& in) requires(
  //       std::is_convertible_v<InputType, ExpectedCallableArgumentType>) {
  //     if constexpr (std::same_as<
  //                       std::remove_cvref_t<ExpectedCallableArgumentType>,
  //                       std::remove_cvref_t<InputType>>) {
  //       if constexpr (std::is_reference_v<ExpectedCallableArgumentType>) {
  //         return &in;
  //       } else {
  //         return std::forward<InputType>(in);
  //       }
  //     } else {
  //       return static_cast<ExpectedCallableArgumentType>(in);
  //     }
  //   }
  //
  //   template <typename CallableArgumentType, typename ArgumentType>
  //   constexpr void checkCallEnzymeArgumentsConsistency(
  //       const TypeList<CallableArgumentType>&, const TypeList<ArgumentType>&)
  //       {
  //     static_assert(isConvertible<ArgumentType, CallableArgumentType>(),
  //                   "first argument is not compatible with the first argument
  //                   of " "the callable");
  //   }  // end of checkCallEnzymeArgumentsConsistency
  //
  //   template <typename CallableArgumentType0,
  //             typename CallableArgumentType1,
  //             typename ArgumentType0,
  //             typename ArgumentType1>
  //   constexpr void checkCallEnzymeArgumentsConsistency(
  //       const TypeList<CallableArgumentType0, CallableArgumentType1>&,
  //       const TypeList<ArgumentType0, ArgumentType1>&) {
  //     static_assert(
  //         isConvertible<ArgumentType1, CallableArgumentType1>(),
  //         "second argument is not compatible with the second argument of "
  //         "the callable");
  //     checkCallEnzymeArgumentsConsistency(TypeList<CallableArgumentType0>{},
  //                                         TypeList<ArgumentType0>{});
  //   }  // end of checkCallEnzymeArgumentsConsistency
  //
  //   template <typename CallableArgumentType0,
  //             typename CallableArgumentType1,
  //             typename CallableArgumentType2,
  //             typename ArgumentType0,
  //             typename ArgumentType1,
  //             typename ArgumentType2>
  //   constexpr void checkCallEnzymeArgumentsConsistency(
  //       const TypeList<CallableArgumentType0,
  //                      CallableArgumentType1,
  //                      CallableArgumentType2>&,
  //       const TypeList<ArgumentType0, ArgumentType1, ArgumentType2>&) {
  //     static_assert(isConvertible<ArgumentType2, CallableArgumentType2>(),
  //                   "third argument is not compatible with the third argument
  //                   of " "the callable");
  //     checkCallEnzymeArgumentsConsistency(
  //         TypeList<CallableArgumentType0, CallableArgumentType1>{},
  //         TypeList<ArgumentType0, ArgumentType1>{});
  //   }  // end of checkCallEnzymeArgumentsConsistency
  //
  //   template <typename CallableArgumentType0,
  //             typename CallableArgumentType1,
  //             typename CallableArgumentType2,
  //             typename CallableArgumentType3,
  //             typename ArgumentType0,
  //             typename ArgumentType1,
  //             typename ArgumentType2,
  //             typename ArgumentType3>
  //   constexpr void checkCallEnzymeArgumentsConsistency(
  //       const TypeList<CallableArgumentType0,
  //                      CallableArgumentType1,
  //                      CallableArgumentType2,
  //                      CallableArgumentType3>&,
  //       const TypeList<ArgumentType0,
  //                      ArgumentType1,
  //                      ArgumentType2,
  //                      ArgumentType3>&) {
  //     static_assert(
  //         isConvertible<ArgumentType3, CallableArgumentType3>(),
  //         "fourth argument is not compatible with the fourth argument of "
  //         "the callable");
  //     checkCallEnzymeArgumentsConsistency(
  //         TypeList<CallableArgumentType0, CallableArgumentType1,
  //                  CallableArgumentType2>{},
  //         TypeList<ArgumentType0, ArgumentType1, ArgumentType2>{});
  //   }  // end of checkCallEnzymeArgumentsConsistency
  //
  //   template <typename CurrentCallableArgumentType,
  //             typename... CallableArgumentsTypes,
  //             typename CurrentArgumentType,
  //             typename... ArgumentsTypes>
  //   constexpr void checkCallEnzymeArgumentsConsistency(
  //       const TypeList<CurrentCallableArgumentType,
  //       CallableArgumentsTypes...>&, const TypeList<CurrentArgumentType,
  //                      ArgumentsTypes...>&)  //
  //       requires((sizeof...(CallableArgumentsTypes) > 0) &&
  //                (sizeof...(CallableArgumentsTypes) ==
  //                 sizeof...(ArgumentsTypes))) {
  //     static_assert(
  //         isConvertible<CurrentArgumentType, CurrentCallableArgumentType>());
  //     checkCallEnzymeArgumentsConsistency(TypeList<CallableArgumentsTypes...>(),
  //                                         TypeList<ArgumentsTypes...>());
  //   }  // end of checkCallEnzymeArgumentsConsistency

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  enum struct Mode { FORWARD, REVERSE };

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_INTERNALS_ENZYME_HXX */
