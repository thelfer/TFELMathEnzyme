/*!
 * \file   TFEL/Math/Enzyme/getDerivativeFunction.ixx
 * \brief  This file implements the getDerivativeFunction function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_GETDERIVATIVEFUNCTION_IXX
#define LIB_TFEL_MATH_ENZYME_GETDERIVATIVEFUNCTION_IXX

#include "TFEL/Math/Enzyme/getForwardModeDerivativeFunction.hxx"
#include "TFEL/Math/Enzyme/getReverseModeDerivativeFunction.hxx"

namespace tfel::math::enzyme {

  template <Mode m,
            std::size_t... Ns,
            internals::EnzymeCallableConcept CallableType>
  auto getDerivativeFunction(const CallableType& c) {
    if constexpr (m == Mode::FORWARD) {
      return getForwardModeDerivativeFunction<Ns...>(c);
    } else {
      return getReverseModeDerivativeFunction<Ns...>(c);
    }
  }

  template <std::size_t... Ns, internals::EnzymeCallableConcept CallableType>
  auto getDerivativeFunction(const CallableType& c) {
    return getDerivativeFunction<Mode::REVERSE, Ns...>(c);
  }  // end of getDerivativeFunction

}  // end of namespace tfel::math::enzyme

namespace tfel::math::enzyme::internals {

  template <Mode m,
            std::size_t... idx,
            internals::IsFunctionPointerConcept auto F,
            typename... FunctionArgumentsTypes>
  auto getDerivativeFunctionImplementation(
      internals::FunctionWrapper<F>,
      const TypeList<FunctionArgumentsTypes...>) {
    auto c = [](const FunctionArgumentsTypes... wargs) { return F(wargs...); };
    return ::tfel::math::enzyme::getDerivativeFunction<m, idx...>(c);
  }  // end of getDerivativeFunctionImplementation

} // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <Mode m,
            std::size_t... idx,
            internals::IsFunctionPointerConcept auto F>
  auto getDerivativeFunction(internals::FunctionWrapper<F> f) {
    return internals::getDerivativeFunctionImplementation<m, idx...>(
        f, internals::getArgumentsList<decltype(F)>());
  }  // end of getDerivativeFunction

  template <std::size_t... idx,
            internals::IsFunctionPointerConcept auto F>
  auto getDerivativeFunction(internals::FunctionWrapper<F> f) {
    return getDerivativeFunction<Mode::REVERSE, idx...>(f);
  }  // end of getDerivativeFunction

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_GETDERIVATIVEFUNCTION_IXX */
