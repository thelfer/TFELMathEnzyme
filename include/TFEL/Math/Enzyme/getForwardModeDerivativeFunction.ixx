/*!
 * \file   TFEL/Math/Enzyme/getForwardModeDerivativeFunction.ixx
 * \brief  This file implements the computeForwardModeDerivative function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_GETFORWARDMODEDERIVATIVEFUNCTION_IXX
#define LIB_TFEL_MATH_ENZYME_GETFORWARDMODEDERIVATIVEFUNCTION_IXX

namespace tfel::math::enzyme::internals {

  template <std::size_t N,
            std::size_t... Ns,
            internals::EnzymeCallableConcept CallableType,
            typename CallableArgumentType0>
  auto getForwardModeDerivativeFunctionImplementation(
      const CallableType& c,
      const TypeList<CallableArgumentType0> args_list) requires(N == 0) {
    auto dc = [c](CallableArgumentType0 warg) {
      return ::tfel::math::enzyme::computeForwardModeDerivative(c, warg);
    };
    if constexpr (sizeof...(Ns) == 0) {
      return dc;
    } else {
      return getForwardModeDerivativeFunctionImplementation<Ns...>(dc, args_list);
    }
  }  // end of getForwardModeDerivativeFunctionImplementation

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <std::size_t... Ns, internals::EnzymeCallableConcept CallableType>
  auto getForwardModeDerivativeFunction(const CallableType& c) requires(
      sizeof...(Ns) > 0) {
    return internals::getForwardModeDerivativeFunctionImplementation<Ns...>(
        c, internals::getArgumentsList<CallableType>());
  }  // end of getForwardModeDerivativeFunction

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_GETFORWARDMODEDERIVATIVEFUNCTION_IXX */
