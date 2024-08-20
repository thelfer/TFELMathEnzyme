/*!
 * \file   TFEL/Math/Enzyme/getForwardModeGradientFunction.ixx
 * \brief  This file implements the computeForwardModeGradient function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_GETFORWARDMODEGRADIENTFUNCTION_IXX
#define LIB_TFEL_MATH_ENZYME_GETFORWARDMODEGRADIENTFUNCTION_IXX

namespace tfel::math::enzyme::internals {

  template <std::size_t N,
            std::size_t... Ns,
            internals::EnzymeCallableConcept CallableType,
            typename CallableArgumentType0>
  auto getForwardModeGradientFunctionImplementation(
      const CallableType& c, const TypeList<CallableArgumentType0> args_list)
    requires(N == 0)
  {
    auto dc = [c](CallableArgumentType0 warg) {
      return ::tfel::math::enzyme::computeForwardModeGradient(c, warg);
    };
    if constexpr (sizeof...(Ns) == 0) {
      return dc;
    } else {
      return getForwardModeGradientFunctionImplementation<Ns...>(dc, args_list);
    }
  }  // end of getForwardModeGradientFunctionImplementation

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <std::size_t... Ns, internals::EnzymeCallableConcept CallableType>
  auto getForwardModeGradientFunction(const CallableType& c)
    requires(sizeof...(Ns) > 0)
  {
    return internals::getForwardModeGradientFunctionImplementation<Ns...>(
        c, internals::getArgumentsList<CallableType>());
  }  // end of getForwardModeGradientFunction

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_GETFORWARDMODEGRADIENTFUNCTION_IXX */
