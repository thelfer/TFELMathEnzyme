/*!
 * \file   TFEL/Math/Enzyme/computeDerivative.hxx
 * \brief  This file implements the computeDerivative function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_COMPUTEDERIVATIVE_IXX
#define LIB_TFEL_MATH_ENZYME_COMPUTEDERIVATIVE_IXX

#include "TFEL/Math/Enzyme/computeForwardModeDerivative.hxx"
#include "TFEL/Math/Enzyme/computeReverseModeDerivative.hxx"

namespace tfel::math::enzyme {

  template <Mode m,
            std::size_t... idx,
            internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto
  computeDerivative(const CallableType& c, ArgumentsTypes&&... args) requires(
      std::is_invocable_v<CallableType, ArgumentsTypes...>) {
    if constexpr (m == Mode::REVERSE) {
      return computeReverseModeDerivative<idx...>(
          c, std::forward<ArgumentsTypes>(args)...);
    } else {
      static_assert(
          sizeof...(ArgumentsTypes) == 1,
          "only callable of one variable are supported in forward mode");
      return computeForwardModeDerivative(
          c, std::forward<ArgumentsTypes>(args)...);
    }
  }  // end of computeDerivative

} // end of namespace tfel::math::enzyme

namespace tfel::math::enzyme::internals {

  template <Mode m,
            std::size_t... idx,
            internals::IsFunctionPointerConcept auto F,
            typename... FunctionArgumentsTypes,
            typename... ArgumentsTypes>
  auto computeDerivativeImplementation(
      internals::FunctionWrapper<F>,
      const TypeList<FunctionArgumentsTypes...>,
      ArgumentsTypes&&... args)  //
      requires(std::is_invocable_v<decltype(F), ArgumentsTypes...>) {
    auto c = [](const FunctionArgumentsTypes... wargs) { return F(wargs...); };
    return ::tfel::math::enzyme::computeDerivative<m, idx...>(
        c, std::forward<ArgumentsTypes>(args)...);
  }  // end of computeDerivativeImplementation

} // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <Mode m,
            std::size_t... idx,
            internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeDerivative(internals::FunctionWrapper<F> f,
                         ArgumentsTypes&&... args)  //
      requires(std::is_invocable_v<decltype(F), ArgumentsTypes...>) {
    return internals::computeDerivativeImplementation<m, idx...>(
        f, internals::getArgumentsList<decltype(F)>(),
        std::forward<ArgumentsTypes>(args)...);
 } // end of computeDerivative

}  // end of namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/computeDerivative.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_COMPUTEDERIVATIVE_IXX */
