/*!
 * \file   TFEL/Math/Enzyme/fwddiff.ixx
 * \brief  This file implements the fwddiff function
 * \author Thomas Helfer
 * \date   02/07/2025
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_FWDDIFF_IXX
#define LIB_TFEL_MATH_ENZYME_FWDDIFF_IXX 1

#include "TFEL/Math/Enzyme/Internals/Enzyme.hxx"

namespace tfel::math::enzyme::internals {

  template <EnzymeCallableConcept CallableType,
            typename... CallableArgumentsTypes,
            typename... ArgumentsTypes>
  auto fwddiffImplementation(TypeList<CallableArgumentsTypes...>,
                             CallableType&& c,
                             ArgumentsTypes&&... args)
  //  requires(
  //      checkCallEnzymeFwdDiffArguments(TypeList<CallableArgumentsTypes>{},
  //                                     TypeList<ArgumentsTypes>{}){
  {
    auto w = callable_wrapper<std::remove_reference_t<CallableType>,
                              TypeList<CallableArgumentsTypes...>>;
    auto* const c_ptr = [&c] {
      if constexpr (std::is_const_v<std::remove_reference_t<CallableType>>) {
        return reinterpret_cast<const void*>(&c);
      } else {
        return reinterpret_cast<void*>(&c);
      }
    }();
    using ResultType =
        std::invoke_result_t<CallableType, CallableArgumentsTypes...>;
    return ::enzyme::get<0>(
        ::enzyme::autodiff<::enzyme::Forward,
                           ::enzyme::DuplicatedNoNeed<ResultType>>(
            +w, ::enzyme::Const(c_ptr),
            convertToEnzymeArgument<CallableArgumentsTypes>(args)...));
  }  // end of  fwddiffImplementation

}  // namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto fwddiff(CallableType&& c, ArgumentsTypes&&... args) requires(
      internals::countNumberOfVariableValueAndIncrement<ArgumentsTypes...>() >
      0) {
    return internals::fwddiffImplementation(
        internals::getArgumentsList<CallableType>(),
        std::forward<CallableType>(c), std::forward<ArgumentsTypes>(args)...);
  }  // end of fwddiff

}  // end of namespace tfel::math::enzyme

// namespace tfel::math::enzyme::internals {
//
//   template <typename... CallableArgumentsTypes, typename... ArgumentsTypes>
//   constexpr void checkCallEnzymeFwdDiffArguments(
//       const TypeList<CallableArgumentsTypes...>&,
//       const TypeList<ArgumentsTypes...>&) noexcept
//       requires(sizeof...(CallableArgumentsTypes) ==
//       sizeof...(ArgumentsTypes)) {
//     static_assert(
//         countNumberOfVariableValueAndIncrement<ArgumentsTypes...>() != 0u,
//         "one argument of type VariableValueAndIncrement is expected");
//     static_assert(
//         countNumberOfVariableValueAndIncrement<ArgumentsTypes...>() == 1u,
//         "only one argument of type VariableValueAndIncrement is expected");
//   }  // end of checkCallEnzymeFwdDiffArguments
//

namespace tfel::math::enzyme::internals {

  template <internals::IsFunctionPointerConcept auto F,
            typename... CallableArgumentsTypes,
            typename... ArgumentsTypes>
  auto fwddiffImplementation(
      const TypeList<CallableArgumentsTypes...>&,
      ArgumentsTypes&&... args) requires((std::
                                              is_invocable_v<
                                                  decltype(F),
                                                  CallableArgumentsTypes...>)) {
    return ::tfel::math::enzyme::fwddiff(
        [](const CallableArgumentsTypes... wargs) { return F(wargs...); },
        std::forward<ArgumentsTypes>(args)...);
  }

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto fwddiff(internals::FunctionWrapper<F>,
               ArgumentsTypes&&... args)  //
      requires(internals::countNumberOfVariableValueAndIncrement<
                   ArgumentsTypes...>() > 0) {
    return internals::fwddiffImplementation<F>(
        internals::getArgumentsList<decltype(F)>(),
        std::forward<ArgumentsTypes>(args)...);
  }  // end of fwddiff

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_FWDDIFF_IXX */
