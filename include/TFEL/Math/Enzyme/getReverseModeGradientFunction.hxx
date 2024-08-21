/*!
 * \file   TFEL/Math/Enzyme/getReverseModeGradientFunction.hxx
 * \brief  This file declares the getReverseModeGradientFunction function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_GETREVERSEMODEGRADIENTFUNCTION_HXX
#define LIB_TFEL_MATH_ENZYME_GETREVERSEMODEGRADIENTFUNCTION_HXX

#include "TFEL/Math/Enzyme/computeReverseModeGradient.hxx"

namespace tfel::math::enzyme {

  template <std::size_t... Ns, internals::EnzymeCallableConcept CallableType>
  auto getReverseModeGradientFunction(const CallableType&) requires(
      sizeof...(Ns) > 0);

}  // end of namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/getReverseModeGradientFunction.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_GETREVERSEMODEGRADIENTFUNCTION_HXX */
