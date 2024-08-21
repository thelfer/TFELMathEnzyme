/*!
 * \file   TFEL/Math/Enzyme/getGradientFunction.ixx
 * \brief  This file implements the getGradientFunction function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_GETGRADIENTFUNCTION_IXX
#define LIB_TFEL_MATH_ENZYME_GETGRADIENTFUNCTION_IXX

#include "TFEL/Math/Enzyme/getForwardModeGradientFunction.hxx"
#include "TFEL/Math/Enzyme/getReverseModeGradientFunction.hxx"

namespace tfel::math::enzyme {

  template <Mode m,
            std::size_t... Ns,
            internals::EnzymeCallableConcept CallableType>
  auto getGradientFunction(const CallableType& c) {
    if constexpr (m == Mode::FORWARD) {
      return getForwardModeGradientFunction<Ns...>(c);
    } else {
      return getReverseModeGradientFunction<Ns...>(c);
    }
  }

  template <std::size_t... Ns, internals::EnzymeCallableConcept CallableType>
  auto getGradientFunction(const CallableType& c) {
    return getGradientFunction<Mode::REVERSE, Ns...>(c);
  }  // end of getGradientFunction

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_GETGRADIENTFUNCTION_IXX */
