/*!
 * \file   TFEL/Math/Enzyme/getDerivative.hxx
 * \brief  This file declares the getDerivative function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_GETDERIVATIVE_HXX
#define LIB_TFEL_MATH_ENZYME_GETDERIVATIVE_HXX

#include "TFEL/Math/Enzyme/fwddiff.hxx"

namespace tfel::math::enzyme {

  template <std::size_t,
            std::size_t...,
            internals::EnzymeCallableConcept CallableType>
  auto getDerivative(const CallableType&);

}  // end of namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/getDerivative.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_GETDERIVATIVE_HXX */
