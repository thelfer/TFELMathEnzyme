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

  /*!
   * \brief Get a function that computes the derivative of a callable using the
   * specified mode \tparam m: differentiation mode (Mode::FORWARD or
   * Mode::REVERSE) \tparam Ns: indices specifying which arguments to
   * differentiate with respect to \tparam CallableType: type of the callable
   * \param[in] c: callable
   * \return a callable that computes the derivative
   * \note The returned function can be called with the same arguments as the
   * original callable
   */
  template <Mode m,
            std::size_t... Ns,
            internals::EnzymeCallableConcept CallableType>
  auto getDerivativeFunction(const CallableType&);

  /*!
   * \brief Get a function that computes the derivative of a callable using
   * reverse mode \tparam Ns: indices specifying which arguments to
   * differentiate with respect to \tparam CallableType: type of the callable
   * \param[in] c: callable
   * \return a callable that computes the derivative using reverse mode
   * \note This is a convenience function that defaults to reverse mode
   */
  template <std::size_t... Ns, internals::EnzymeCallableConcept CallableType>
  auto getDerivativeFunction(const CallableType&);

  /*!
   * \brief Get a function that computes the derivative of a free function using
   * the specified mode \tparam m: differentiation mode (Mode::FORWARD or
   * Mode::REVERSE) \tparam idx: indices specifying which arguments to
   * differentiate with respect to \tparam F: pointer to the free function
   * \param[in] f: free function wrapper (created with
   * tfel::math::enzyme::function) \return a callable that computes the
   * derivative \note Regular functions must be passed as template parameters
   * via FunctionWrapper
   */
  template <Mode m,
            std::size_t... idx,
            internals::IsFunctionPointerConcept auto F>
  auto getDerivativeFunction(internals::FunctionWrapper<F>);

  /*!
   * \brief Get a function that computes the derivative of a free function using
   * reverse mode \tparam idx: indices specifying which arguments to
   * differentiate with respect to \tparam F: pointer to the free function
   * \param[in] f: free function wrapper (created with
   * tfel::math::enzyme::function) \return a callable that computes the
   * derivative using reverse mode \note This is a convenience function that
   * defaults to reverse mode
   */
  template <std::size_t... idx, internals::IsFunctionPointerConcept auto F>
  auto getDerivativeFunction(internals::FunctionWrapper<F>);

}  // end of namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/getDerivativeFunction.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_GETDERIVATIVEFUNCTION_HXX */
