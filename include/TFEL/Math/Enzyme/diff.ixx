/*!
 * \file   TFEL/Math/Enzyme/diff.ixx
 * \brief  This file implements the diff function
 * \author Thomas Helfer
 * \date   02/07/2025
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_DIFF_IXX
#define LIB_TFEL_MATH_ENZYME_DIFF_IXX 1

#include "TFEL/Math/Enzyme/Internals/Enzyme.hxx"

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
  auto convertToEnzymeArgument(InputType&& in)
    requires(std::is_convertible_v<InputType, ExpectedCallableArgumentType>)
  {
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
      const TypeList<CurrentArgumentType, ArgumentsTypes...>&) {
    static_assert(
        isConvertible<CurrentArgumentType, CurrentCallableArgumentType>());
    checkCallEnzymeArgumentsConsistency(TypeList<CallableArgumentsTypes...>(),
                                        TypeList<ArgumentsTypes...>());
  }  // end of checkCallEnzymeArgumentsConsistency

  template <typename... CallableArgumentsTypes, typename... ArgumentsTypes>
  constexpr void checkCallEnzymeArguments(
      const TypeList<CallableArgumentsTypes...>&,
      const TypeList<ArgumentsTypes...>&) noexcept
    requires(sizeof...(CallableArgumentsTypes) == sizeof...(ArgumentsTypes))
  {
    static_assert(
        countNumberOfVariableValueAndIncrement<ArgumentsTypes...>() != 0u,
        "one argument of type VariableValueAndIncrement is expected");
    static_assert(
        countNumberOfVariableValueAndIncrement<ArgumentsTypes...>() == 1u,
        "only one argument of type VariableValueAndIncrement is expected");
  }  // end of checkCallEnzymeArguments

  template <typename CallableType,
            typename CallableArgumentType,
            typename ArgumentType>
  auto diffImplementation(const TypeList<CallableArgumentType>&,
                          const CallableType& c,
                          ArgumentType&& arg)
    requires(isVariableValueAndIncrement<std::decay_t<ArgumentType>>())
  {
    auto wrapper = [](const CallableType* const ptr,
                      const CallableArgumentType warg) { return (*ptr)(warg); };
    using ResultType = std::invoke_result_t<CallableType, CallableArgumentType>;
    void* const wrapper_ptr = reinterpret_cast<void*>(+wrapper);
    const void* const c_ptr = reinterpret_cast<const void*>(&c);
    return __enzyme_fwddiff<ResultType>(
        wrapper_ptr, enzyme_const, c_ptr, enzyme_dup,
        convertToEnzymeArgument<CallableArgumentType>(arg.value),
        convertToEnzymeArgument<CallableArgumentType>(arg.increment));
  }  // end of diffImplementation

  template <typename CallableType,
            typename CallableArgumentType0,
            typename CallableArgumentType1,
            typename ArgumentType0,
            typename ArgumentType1>
  auto diffImplementation(
      const TypeList<CallableArgumentType0, CallableArgumentType1>&,
      const CallableType& c,
      ArgumentType0&& arg0,
      ArgumentType1&& arg1) {
    static_assert(isConvertible<ArgumentType0, CallableArgumentType0>(),
                  "first argument is not compatible with the first argument of "
                  "the callable");
    static_assert(
        isConvertible<ArgumentType1, CallableArgumentType1>(),
        "second argument is not compatible with the second argument of "
        "the callable");
    checkCallEnzymeArguments(
        TypeList<CallableArgumentType0, CallableArgumentType1>{},
        TypeList<ArgumentType0, ArgumentType1>{});
    auto wrapper =
        [](const CallableType* const ptr, const CallableArgumentType0 warg0,
           const CallableArgumentType1 warg1) { return (*ptr)(warg0, warg1); };
    using ResultType = std::invoke_result_t<CallableType, CallableArgumentType0,
                                            CallableArgumentType1>;
    void* const wrapper_ptr = reinterpret_cast<void*>(+wrapper);
    const void* const c_ptr = reinterpret_cast<const void*>(&c);
    constexpr bool b0 = isVariableValueAndIncrement<ArgumentType0>();
    if constexpr (b0) {
      return __enzyme_fwddiff<ResultType>(
          wrapper_ptr, enzyme_const, c_ptr, enzyme_dup,
          convertToEnzymeArgument<CallableArgumentType0>(arg0.value),
          convertToEnzymeArgument<CallableArgumentType0>(arg0.increment),
          enzyme_const, convertToEnzymeArgument<CallableArgumentType1>(arg1));
    } else {
      return __enzyme_fwddiff<ResultType>(
          wrapper_ptr, enzyme_const, c_ptr, enzyme_const,
          convertToEnzymeArgument<CallableArgumentType0>(arg0), enzyme_dup,
          convertToEnzymeArgument<CallableArgumentType1>(arg1.value),
          convertToEnzymeArgument<CallableArgumentType1>(arg1.increment));
    }
  }

  template <typename CallableType,
            typename CallableArgumentType0,
            typename CallableArgumentType1,
            typename CallableArgumentType2,
            typename ArgumentType0,
            typename ArgumentType1,
            typename ArgumentType2>
  auto diffImplementation(const TypeList<CallableArgumentType0,
                                         CallableArgumentType1,
                                         CallableArgumentType2>&,
                          const CallableType& c,
                          ArgumentType0&& arg0,
                          ArgumentType1&& arg1,
                          ArgumentType2&& arg2) {
    checkCallEnzymeArguments(
        TypeList<CallableArgumentType0, CallableArgumentType1,
                 CallableArgumentType2>{},
        TypeList<ArgumentType0, ArgumentType1, ArgumentType2>{});
    auto wrapper = [](const CallableType* const ptr,
                      const CallableArgumentType0 warg0,
                      const CallableArgumentType1 warg1,
                      const CallableArgumentType2 warg2) {
      return (*ptr)(warg0, warg1, warg2);
    };
    using ResultType =
        std::invoke_result_t<CallableType, CallableArgumentType0,
                             CallableArgumentType1, CallableArgumentType2>;
    void* const wrapper_ptr = reinterpret_cast<void*>(+wrapper);
    const void* const c_ptr = reinterpret_cast<const void*>(&c);
    constexpr bool b0 = isVariableValueAndIncrement<ArgumentType0>();
    constexpr bool b1 = isVariableValueAndIncrement<ArgumentType1>();
    if constexpr (b0) {
      return __enzyme_fwddiff<ResultType>(
          wrapper_ptr, enzyme_const, c_ptr, enzyme_dup,
          convertToEnzymeArgument<CallableArgumentType0>(arg0.value),
          convertToEnzymeArgument<CallableArgumentType0>(arg0.increment),
          enzyme_const, convertToEnzymeArgument<CallableArgumentType1>(arg1),
          enzyme_const, convertToEnzymeArgument<CallableArgumentType2>(arg2));
    } else if constexpr (b1) {
      return __enzyme_fwddiff<ResultType>(
          wrapper_ptr, enzyme_const, c_ptr, enzyme_const,
          convertToEnzymeArgument<CallableArgumentType0>(arg0), enzyme_dup,
          convertToEnzymeArgument<CallableArgumentType1>(arg1.value),
          convertToEnzymeArgument<CallableArgumentType1>(arg1.increment),
          enzyme_const, convertToEnzymeArgument<CallableArgumentType2>(arg2));
    } else {
      return __enzyme_fwddiff<ResultType>(
          wrapper_ptr, enzyme_const, c_ptr, enzyme_const,
          convertToEnzymeArgument<CallableArgumentType0>(arg0), enzyme_const,
          convertToEnzymeArgument<CallableArgumentType1>(arg1), enzyme_dup,
          convertToEnzymeArgument<CallableArgumentType2>(arg2.value),
          convertToEnzymeArgument<CallableArgumentType2>(arg2.increment));
    }
  }

  template <typename CallableType,
            typename CallableArgumentType0,
            typename CallableArgumentType1,
            typename CallableArgumentType2,
            typename CallableArgumentType3,
            typename ArgumentType0,
            typename ArgumentType1,
            typename ArgumentType2,
            typename ArgumentType3>
  auto diffImplementation(const TypeList<CallableArgumentType0,
                                         CallableArgumentType1,
                                         CallableArgumentType2,
                                         CallableArgumentType3>&,
                          const CallableType& c,
                          ArgumentType0&& arg0,
                          ArgumentType1&& arg1,
                          ArgumentType2&& arg2,
                          ArgumentType3&& arg3) {
    checkCallEnzymeArguments(
        TypeList<CallableArgumentType0, CallableArgumentType1,
                 CallableArgumentType2, CallableArgumentType3>{},
        TypeList<ArgumentType0, ArgumentType1, ArgumentType2, ArgumentType3>{});
    auto wrapper =
        [](const CallableType* const ptr, const CallableArgumentType0 warg0,
           const CallableArgumentType1 warg1, const CallableArgumentType2 warg2,
           const CallableArgumentType3 warg3) {
          return (*ptr)(warg0, warg1, warg2, warg3);
        };
    using ResultType =
        std::invoke_result_t<CallableType, CallableArgumentType0,
                             CallableArgumentType1, CallableArgumentType2,
                             CallableArgumentType3>;
    void* const wrapper_ptr = reinterpret_cast<void*>(+wrapper);
    const void* const c_ptr = reinterpret_cast<const void*>(&c);
    constexpr bool b0 = isVariableValueAndIncrement<ArgumentType0>();
    constexpr bool b1 = isVariableValueAndIncrement<ArgumentType1>();
    constexpr bool b2 = isVariableValueAndIncrement<ArgumentType2>();
    if constexpr (b0) {
      return __enzyme_fwddiff<ResultType>(
          wrapper_ptr, enzyme_const, c_ptr, enzyme_dup,
          convertToEnzymeArgument<CallableArgumentType0>(arg0.value),
          convertToEnzymeArgument<CallableArgumentType0>(arg0.increment),
          enzyme_const, convertToEnzymeArgument<CallableArgumentType1>(arg1),
          enzyme_const, convertToEnzymeArgument<CallableArgumentType2>(arg2),
          enzyme_const, convertToEnzymeArgument<CallableArgumentType3>(arg3));
    } else if constexpr (b1) {
      return __enzyme_fwddiff<ResultType>(
          wrapper_ptr, enzyme_const, c_ptr, enzyme_const,
          convertToEnzymeArgument<CallableArgumentType0>(arg0), enzyme_dup,
          convertToEnzymeArgument<CallableArgumentType1>(arg1.value),
          convertToEnzymeArgument<CallableArgumentType1>(arg1.increment),
          enzyme_const, convertToEnzymeArgument<CallableArgumentType2>(arg2),
          enzyme_const, convertToEnzymeArgument<CallableArgumentType3>(arg3));
    } else if constexpr (b2) {
      return __enzyme_fwddiff<ResultType>(
          wrapper_ptr, enzyme_const, c_ptr, enzyme_const,
          convertToEnzymeArgument<CallableArgumentType0>(arg0), enzyme_const,
          convertToEnzymeArgument<CallableArgumentType1>(arg1), enzyme_dup,
          convertToEnzymeArgument<CallableArgumentType2>(arg2.value),
          convertToEnzymeArgument<CallableArgumentType2>(arg2.increment),
          enzyme_const, convertToEnzymeArgument<CallableArgumentType3>(arg3));
    } else {
      return __enzyme_fwddiff<ResultType>(
          wrapper_ptr, enzyme_const, c_ptr, enzyme_const,
          convertToEnzymeArgument<CallableArgumentType0>(arg0), enzyme_const,
          convertToEnzymeArgument<CallableArgumentType1>(arg1), enzyme_const,
          convertToEnzymeArgument<CallableArgumentType2>(arg2), enzyme_dup,
          convertToEnzymeArgument<CallableArgumentType3>(arg3.value),
          convertToEnzymeArgument<CallableArgumentType3>(arg3.increment));
    }
  }

}  // namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <internals::EnzymeCallableConcept CallableType,
            typename ArgumentType0>
  auto diff(const CallableType& c, ArgumentType0&& arg0)
    requires((internals::isVariableValueAndIncrement<ArgumentType0>()) &&
             (internals::getArgumentsSize<CallableType>() == 1u))
  {
    return internals::diffImplementation(
        internals::getArgumentsList<CallableType>(), c,
        std::forward<ArgumentType0>(arg0));
  }  // end of diff

  template <internals::EnzymeCallableConcept CallableType,
            typename ArgumentType0,
            typename ArgumentType1>
  auto diff(const CallableType& c, ArgumentType0&& arg0, ArgumentType1&& arg1)
    requires((internals::getArgumentsSize<CallableType>() == 2u))
  {
    return internals::diffImplementation(
        internals::getArgumentsList<CallableType>(), c,
        std::forward<ArgumentType0>(arg0), std::forward<ArgumentType1>(arg1));
  }  // end of diff

  template <internals::EnzymeCallableConcept CallableType,
            typename ArgumentType0,
            typename ArgumentType1,
            typename ArgumentType2>
  auto diff(const CallableType& c,
            ArgumentType0&& arg0,
            ArgumentType1&& arg1,
            ArgumentType2&& arg2)
    requires((internals::getArgumentsSize<CallableType>() == 3u))
  {
    return internals::diffImplementation(
        internals::getArgumentsList<CallableType>(), c,
        std::forward<ArgumentType0>(arg0), std::forward<ArgumentType1>(arg1),
        std::forward<ArgumentType2>(arg2));
  }  // end of diff

  template <internals::EnzymeCallableConcept CallableType,
            typename ArgumentType0,
            typename ArgumentType1,
            typename ArgumentType2,
            typename ArgumentType3>
  auto diff(const CallableType& c,
            ArgumentType0&& arg0,
            ArgumentType1&& arg1,
            ArgumentType2&& arg2,
            ArgumentType3&& arg3)
    requires((internals::getArgumentsSize<CallableType>() == 4u))
  {
    return internals::diffImplementation(
        internals::getArgumentsList<CallableType>(), c,
        std::forward<ArgumentType0>(arg0), std::forward<ArgumentType1>(arg1),
        std::forward<ArgumentType2>(arg2), std::forward<ArgumentType3>(arg3));
  }  // end of diff

}  // namespace tfel::math::enzyme

namespace tfel::math::enzyme::internals {

  template <internals::IsFunctionPointerConcept auto F,
            typename... CallableArgumentsTypes,
            typename... ArgumentsTypes>
  auto diffImplementation(const TypeList<CallableArgumentsTypes...>&,
                          ArgumentsTypes&&... args)
    requires((sizeof...(CallableArgumentsTypes) == sizeof...(ArgumentsTypes)) &&
             (std::is_invocable_v<decltype(F), CallableArgumentsTypes...>))
  {
    auto c = [](const CallableArgumentsTypes... wargs) { return F(wargs...); };
    return ::tfel::math::enzyme::diff(c, std::forward<ArgumentsTypes>(args)...);
  }

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto diff(internals::FunctionWrapper<F>, ArgumentsTypes&&... args) {
    return internals::diffImplementation<F>(
        internals::getArgumentsList<decltype(F)>(),
        std::forward<ArgumentsTypes>(args)...);
  }

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_DIFF_IXX */
