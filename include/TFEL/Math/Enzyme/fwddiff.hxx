/*!
 * \file   TFEL/Math/Enzyme/fwddiff.hxx
 * \brief  This file declares the fwddiff function
 * \author Thomas Helfer
 * \date   02/07/2025
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_FWDDIFF_HXX
#define LIB_TFEL_MATH_ENZYME_FWDDIFF_HXX 1

#include <cstddef>
#include "TFEL/Math/Enzyme/Variable.hxx"
#include "TFEL/Math/Enzyme/Internals/FunctionUtilities.hxx"

namespace tfel::math::enzyme {

  /*!
   * \brief Compute the increment of a callable given the values of some
   * variables and their increments and the values of the other.
   * \tparam CallableType: type of the callable
   * \tparam ArgumentsTypes: types of the arguments passed
   * \param[in] c: callable
   * \param[in] args: arguments (must include at least one
   * VariableValueAndIncrement) \return the callable increment \note This
   * function uses Enzyme's forward mode automatic differentiation
   */
  template <internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto fwddiff(CallableType&&, ArgumentsTypes&&...) requires(
      internals::countNumberOfVariableValueAndIncrement<ArgumentsTypes...>() >
      0);

  /*!
   * \brief Helper function to compute the differential of a regular function.
   *
   * Regular functions must be passed as template parameters and not by pointer,
   * otherwise, the symbol associated with the function is not seen by Enzyme.
   * \tparam F: the function pointer
   * \tparam ArgumentsTypes: types of the arguments passed
   * \param[in] f: function wrapper created with tfel::math::enzyme::function
   * \param[in] args: arguments (must include at least one
   * VariableValueAndIncrement) \return the function increment
   */
  template <internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto fwddiff(internals::FunctionWrapper<F>, ArgumentsTypes&&...) requires(
      internals::countNumberOfVariableValueAndIncrement<ArgumentsTypes...>() >
      0);

}  // end of namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/fwddiff.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_FWDDIFF_HXX */
