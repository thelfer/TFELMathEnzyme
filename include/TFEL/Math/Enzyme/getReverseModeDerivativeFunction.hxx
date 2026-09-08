/*!
 * \file   TFEL/Math/Enzyme/getReverseModeDerivativeFunction.hxx
 * \brief  This file declares the getReverseModeDerivativeFunction function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_GETREVERSEMODEDERIVATIVEFUNCTION_HXX
#define LIB_TFEL_MATH_ENZYME_GETREVERSEMODEDERIVATIVEFUNCTION_HXX

#include "TFEL/Math/Enzyme/computeReverseModeDerivative.hxx"

namespace tfel::math::enzyme {

  /*!
   * \brief Get a function that computes the reverse mode derivative of a
   * callable \tparam Ns: indices specifying which arguments to differentiate
   * with respect to \tparam CallableType: type of the callable \param[in] c:
   * callable \return a callable that computes the derivative using reverse mode
   * \note The returned function can be called with the same arguments as the
   * original callable
   */
  template <std::size_t... Ns, internals::EnzymeCallableConcept CallableType>
  auto getReverseModeDerivativeFunction(const CallableType&) requires(
      sizeof...(Ns) > 0);

}  // end of namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/getReverseModeDerivativeFunction.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_GETREVERSEMODEDERIVATIVEFUNCTION_HXX */
