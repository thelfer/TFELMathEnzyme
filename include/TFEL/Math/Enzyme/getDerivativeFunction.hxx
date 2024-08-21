/*!
 * \file   TFEL/Math/Enzyme/getDerivativeFunction.hxx
 * \brief  This file declares the getDerivativeFunction function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_GETDERIVATIVEFUNCTION_HXX
#define LIB_TFEL_MATH_ENZYME_GETDERIVATIVEFUNCTION_HXX

#include "TFEL/Math/Enzyme/Internals/Enzyme.hxx"
#include "TFEL/Math/Enzyme/Internals/FunctionUtilities.hxx"

namespace tfel::math::enzyme {

  template <Mode m,
            std::size_t... Ns,
            internals::EnzymeCallableConcept CallableType>
  auto getDerivativeFunction(const CallableType&);

  template <std::size_t... Ns, internals::EnzymeCallableConcept CallableType>
  auto getDerivativeFunction(const CallableType&);

  template <Mode m,
            std::size_t... idx,
            internals::IsFunctionPointerConcept auto F>
  auto getDerivativeFunction(internals::FunctionWrapper<F>);

  template <std::size_t... idx, internals::IsFunctionPointerConcept auto F>
  auto getDerivativeFunction(internals::FunctionWrapper<F>);

}  // end of namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/getDerivativeFunction.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_GETDERIVATIVEFUNCTION_HXX */
