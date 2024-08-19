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
#include "TFEL/Math/Enzyme/Internals/TypeList.hxx"

/*!
 * \brief specifier used to introduce duplicated variables
 * \note see the enzyme documentation for details
 */
extern int enzyme_dup;
/*!
 * \brief specifier used to introduce a variable with respect to which a
 * callable is differentiated
 */
extern int enzyme_out;
//! \brief specifier used to introduce a standard variables
extern int enzyme_const;

/*!
 * \brief Enzyme'entry point for differentiation
 * \tparam ResultType: type of the result of the callable pointed by ptr
 * \tparam ArgumentsTypes: types of the arguments of the callable pointed by ptr
 * \param[in] ptr:
 */
template <typename ResultType, typename... ArgumentsTypes>
ResultType __enzyme_autodiff(void*, ArgumentsTypes...);

/*!
 * \brief Enzyme'entry point for forward differentiation
 * \tparam ResultType: type of the result of the callable pointed by ptr
 * \tparam ArgumentsTypes: types of the arguments of the callable pointed by ptr
 * \param[in] ptr:
 */
template <typename ResultType, typename... ArgumentsTypes>
ResultType __enzyme_fwddiff(void*, ArgumentsTypes...);

namespace tfel::math::enzyme::internals {

  template <typename SourceType, typename DestinationType>
  struct IsConvertible : std::is_convertible<SourceType, DestinationType> {};

  template <typename SourceType, typename DestinationType>
  struct IsConvertible<VariableValueAndIncrement<SourceType>, DestinationType>
      : std::is_convertible<SourceType, DestinationType> {};

  template <typename SourceType, typename DestinationType>
  constexpr bool isConvertible() noexcept {
    return IsConvertible<SourceType, DestinationType>::value;
  }  // end of isConvertible

  template <typename ExpectedCallableArgumentType, typename InputType>
  auto convertToEnzymeArgument(InputType&& in) requires(
      std::is_convertible_v<InputType, ExpectedCallableArgumentType>) {
    if constexpr (std::same_as<
                      std::remove_cvref_t<ExpectedCallableArgumentType>,
                      std::remove_cvref_t<InputType>>) {
      if constexpr (std::is_reference_v<ExpectedCallableArgumentType>) {
        return &in;
      } else {
        return std::forward<InputType>(in);
      }
    } else {
      return static_cast<ExpectedCallableArgumentType>(in);
    }
  }

  template <typename CallableArgumentType, typename ArgumentType>
  constexpr void checkCallEnzymeArgumentsConsistency(
      const TypeList<CallableArgumentType>&, const TypeList<ArgumentType>&) {
    static_assert(isConvertible<ArgumentType, CallableArgumentType>(),
                  "first argument is not compatible with the first argument of "
                  "the callable");
  }  // end of checkCallEnzymeArgumentsConsistency

  template <typename CallableArgumentType0,
            typename CallableArgumentType1,
            typename ArgumentType0,
            typename ArgumentType1>
  constexpr void checkCallEnzymeArgumentsConsistency(
      const TypeList<CallableArgumentType0, CallableArgumentType1>&,
      const TypeList<ArgumentType0, ArgumentType1>&) {
    static_assert(
        isConvertible<ArgumentType1, CallableArgumentType1>(),
        "second argument is not compatible with the second argument of "
        "the callable");
    checkCallEnzymeArgumentsConsistency(TypeList<CallableArgumentType0>{},
                                        TypeList<ArgumentType0>{});
  }  // end of checkCallEnzymeArgumentsConsistency

  template <typename CallableArgumentType0,
            typename CallableArgumentType1,
            typename CallableArgumentType2,
            typename ArgumentType0,
            typename ArgumentType1,
            typename ArgumentType2>
  constexpr void checkCallEnzymeArgumentsConsistency(
      const TypeList<CallableArgumentType0,
                     CallableArgumentType1,
                     CallableArgumentType2>&,
      const TypeList<ArgumentType0, ArgumentType1, ArgumentType2>&) {
    static_assert(isConvertible<ArgumentType2, CallableArgumentType2>(),
                  "third argument is not compatible with the third argument of "
                  "the callable");
    checkCallEnzymeArgumentsConsistency(
        TypeList<CallableArgumentType0, CallableArgumentType1>{},
        TypeList<ArgumentType0, ArgumentType1>{});
  }  // end of checkCallEnzymeArgumentsConsistency

  template <typename CallableArgumentType0,
            typename CallableArgumentType1,
            typename CallableArgumentType2,
            typename CallableArgumentType3,
            typename ArgumentType0,
            typename ArgumentType1,
            typename ArgumentType2,
            typename ArgumentType3>
  constexpr void checkCallEnzymeArgumentsConsistency(
      const TypeList<CallableArgumentType0,
                     CallableArgumentType1,
                     CallableArgumentType2,
                     CallableArgumentType3>&,
      const TypeList<ArgumentType0,
                     ArgumentType1,
                     ArgumentType2,
                     ArgumentType3>&) {
    static_assert(
        isConvertible<ArgumentType3, CallableArgumentType3>(),
        "fourth argument is not compatible with the fourth argument of "
        "the callable");
    checkCallEnzymeArgumentsConsistency(
        TypeList<CallableArgumentType0, CallableArgumentType1,
                 CallableArgumentType2>{},
        TypeList<ArgumentType0, ArgumentType1, ArgumentType2>{});
  }  // end of checkCallEnzymeArgumentsConsistency

  template <typename CurrentCallableArgumentType,
            typename... CallableArgumentsTypes,
            typename CurrentArgumentType,
            typename... ArgumentsTypes>
  constexpr void checkCallEnzymeArgumentsConsistency(
      const TypeList<CurrentCallableArgumentType, CallableArgumentsTypes...>&,
      const TypeList<CurrentArgumentType,
                     ArgumentsTypes...>&)  //
      requires((sizeof...(CallableArgumentsTypes) > 0) &&
               (sizeof...(CallableArgumentsTypes) ==
                sizeof...(ArgumentsTypes))) {
    static_assert(
        isConvertible<CurrentArgumentType, CurrentCallableArgumentType>());
    checkCallEnzymeArgumentsConsistency(TypeList<CallableArgumentsTypes...>(),
                                        TypeList<ArgumentsTypes...>());
  }  // end of checkCallEnzymeArgumentsConsistency

  template <typename... CallableArgumentsTypes, typename... ArgumentsTypes>
  constexpr void checkCallEnzymeArguments(
      const TypeList<CallableArgumentsTypes...>&,
      const TypeList<ArgumentsTypes...>&) noexcept
      requires(sizeof...(CallableArgumentsTypes) == sizeof...(ArgumentsTypes)) {
    static_assert(
        countNumberOfVariableValueAndIncrement<ArgumentsTypes...>() != 0u,
        "one argument of type VariableValueAndIncrement is expected");
    static_assert(
        countNumberOfVariableValueAndIncrement<ArgumentsTypes...>() == 1u,
        "only one argument of type VariableValueAndIncrement is expected");
  }  // end of checkCallEnzymeArguments

}  // end of namespace tfel::math::enzyme::internals

#endif /* LIB_TFEL_MATH_ENZYME_INTERNALS_ENZYME_HXX */
