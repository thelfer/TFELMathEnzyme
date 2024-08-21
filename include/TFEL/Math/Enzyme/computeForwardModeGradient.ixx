/*!
 * \file   TFEL/Math/Enzyme/computeForwardModeGradient.ixx
 * \brief  This file implements the getDerivative function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_COMPUTEFORWARDMODEGRADIENT_IXX
#define LIB_TFEL_MATH_ENZYME_COMPUTEFORWARDMODEGRADIENT_IXX

#include "TFEL/Math/General/DerivativeType.hxx"

namespace tfel::math::enzyme::internals {

  template <EnzymeCallableConcept CallableType,
            typename CallableArgumentType0,
            typename ArgumentType0>
  auto computeForwardModeGradientImplementation(
      const CallableType& c,
      const TypeList<CallableArgumentType0>&,
      ArgumentType0&&
          arg0) requires(std::is_invocable_v<CallableType, ArgumentType0>) {
    using ResultType =
        std::invoke_result_t<CallableType, CallableArgumentType0>;
    using DerivativeResultType =
        derivative_type<ResultType, std::decay_t<CallableArgumentType0>>;
    if constexpr (ScalarConcept<CallableArgumentType0>) {
      // derivative with respect to a scalar
      auto vdv = VariableValueAndIncrement<std::decay_t<CallableArgumentType0>>{
          .value = arg0, .increment = 1};
      return fwddiff(c, vdv);
    } else if constexpr (ScalarConcept<ResultType>) {
      auto vdv = VariableValueAndIncrement<std::decay_t<CallableArgumentType0>>{
          .value = arg0, .increment = {}};
      auto r = DerivativeResultType{};
      for (typename std::decay_t<CallableArgumentType0>::size_type i = 0;
           i != vdv.value.size(); ++i) {
        vdv.increment[i] = 1;
        const auto dc = fwddiff(c, vdv);
        r(i) = dc;
        vdv.increment[i] = 0;
      }
      return r;
    } else {
      // derivative with respect to a MathObject
      auto vdv = VariableValueAndIncrement<std::decay_t<CallableArgumentType0>>{
          .value = arg0, .increment = {}};
      auto r = DerivativeResultType{};
      for (typename std::decay_t<CallableArgumentType0>::size_type i = 0;
           i != r.size(0); ++i) {
        vdv.increment[i] = 1;
        const auto dc = fwddiff(c, vdv);
        for (typename ResultType::size_type j = 0; j != vdv.value.size(); ++j) {
          r(i, j) = dc(j);
        }
        vdv.increment[i] = 0;
      }
      return r;
    }
  }  // end of computeForwardModeGradientImplementation

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeForwardModeGradient(
      const CallableType& c,
      ArgumentsTypes&&... args) requires(std::
                                             is_invocable_v<
                                                 CallableType,
                                                 ArgumentsTypes...>) {
    static_assert(sizeof...(ArgumentsTypes) == 1,
                  "only callable of one variable are supported");
    return internals::computeForwardModeGradientImplementation(
        c, internals::getArgumentsList<CallableType>(),
        std::forward<ArgumentsTypes>(args)...);
  }  // end of computeForwardModeGradient

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_COMPUTEFORWARDMODEGRADIENT_IXX */
