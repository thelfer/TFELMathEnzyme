/*!
 * \file   TFEL/Math/Enzyme/computeForwardModeDerivative.hxx
 * \brief  This file declares the computeForwardModeDerivative function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_COMPUTEFORWARDMODEDERIVATIVE_HXX
#define LIB_TFEL_MATH_ENZYME_COMPUTEFORWARDMODEDERIVATIVE_HXX

#include "TFEL/Math/Enzyme/fwddiff.hxx"

namespace tfel::math::enzyme {

  /*!
   * \brief Compute the forward mode derivative of a callable
   * \tparam CallableType: type of the callable
   * \tparam ArgumentsTypes: types of the arguments passed to the callable
   * \param[in] c: callable
   * \param[in] args: arguments passed to the callable
   * \return the derivative of the callable with respect to its arguments
   * \note This function only supports callables with a single argument
   */
  template <internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeForwardModeDerivative(
      const CallableType&,
      ArgumentsTypes&&...) requires(std::is_invocable_v<CallableType,
                                                        ArgumentsTypes...>);

}  // end of namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/computeForwardModeDerivative.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_COMPUTEFORWARDMODEDERIVATIVE_IXX */
