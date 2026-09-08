/*!
 * \file   TFEL/Math/Enzyme/computeDerivative.hxx
 * \brief  This file declares the computeDerivative function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_COMPUTEDERIVATIVE_HXX
#define LIB_TFEL_MATH_ENZYME_COMPUTEDERIVATIVE_HXX

#include "TFEL/Math/Enzyme/fwddiff.hxx"

namespace tfel::math::enzyme {

  /*!
   * \brief Compute the derivative of a callable with respect to the variables
   * designated by the indices `idx` using the specified mode
   * \tparam m: differentiation mode (Mode::FORWARD or Mode::REVERSE)
   * \tparam idx: indices of the variables with respect to which the derivatives
   * are computed
   * \tparam CallableType: type of the callable
   * \tparam ArgumentsTypes: types of the arguments passed to the callable
   * \param[in] c: callable
   * \param[in] args: arguments passed to the callable
   * \return the derivative(s) of the callable result
   * \note In forward mode, only single-argument callables are supported
   */
  template <Mode m,
            std::size_t... idx,
            internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeDerivative(const CallableType&, ArgumentsTypes&&...) requires(
      std::is_invocable_v<CallableType, ArgumentsTypes...>);

  /*!
   * \brief Compute the derivative of a free function with respect to the
   * variables designated by the indices `idx` using the specified mode
   * \tparam m: differentiation mode (Mode::FORWARD or Mode::REVERSE)
   * \tparam idx: indices of the variables with respect to which the derivatives
   * are computed
   * \tparam F: pointer to the free function
   * \tparam ArgumentsTypes: types of the arguments passed to the free function
   * \param[in] f: free function wrapper (created with
   * tfel::math::enzyme::function)
   * \param[in] args: arguments passed to the free
   * function \return the derivative(s) of the function result
   * \note Regular functions must be passed as template parameters via
   * FunctionWrapper
   */
  template <Mode m,
            std::size_t... idx,
            internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeDerivative(internals::FunctionWrapper<F>,
                         ArgumentsTypes&&...)  //
      requires(std::is_invocable_v<decltype(F), ArgumentsTypes...>);

}  // end of namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/computeDerivative.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_COMPUTEDERIVATIVE_IXX */
