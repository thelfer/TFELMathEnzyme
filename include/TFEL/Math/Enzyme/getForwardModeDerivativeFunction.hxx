/*!
 * \file   TFEL/Math/Enzyme/getForwardModeDerivativeFunction.hxx
 * \brief  This file declares the getForwardModeDerivativeFunction function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_GETFORWARDMODEDERIVATIVEFUNCTION_HXX
#define LIB_TFEL_MATH_ENZYME_GETFORWARDMODEDERIVATIVEFUNCTION_HXX

#include "TFEL/Math/Enzyme/computeForwardModeDerivative.hxx"

namespace tfel::math::enzyme {

  /*!
   * \brief Get a function that computes the forward mode derivative of a
   * callable.
   * \tparam Ns: Indices specifying which arguments to differentiate with
   * respect to. Only the index 0 is supported: forward mode supports
   * differentiating callables of a single variable (a repeated index, like `0,
   * 0`, gives higher order derivatives).
   * \tparam CallableType: Type of the callable.
   * \param[in] c: The callable to differentiate.
   * \return A callable that computes the derivative.
   * \note The returned function can be called with the same arguments as the
   *   original callable.
   */
  template <std::size_t... Ns, internals::EnzymeCallableConcept CallableType>
  auto getForwardModeDerivativeFunction(const CallableType&)
    requires((sizeof...(Ns) > 0) && ((Ns == 0) && ...));

  /*!
   * \brief Get a function that computes the forward mode derivative of a free
   *   function.
   * \tparam Ns: Indices specifying which arguments to differentiate with
   *   respect to. Only the index 0 is supported (see above).
   * \tparam F: Pointer to the free function.
   * \param[in] f: Free function wrapper (created with
   *   tfel::math::enzyme::function).
   * \return A callable that computes the derivative.
   * \note Regular functions must be passed as template parameters via
   *   FunctionWrapper.
   */
  template <std::size_t... Ns, internals::IsFunctionPointerConcept auto F>
  auto getForwardModeDerivativeFunction(internals::FunctionWrapper<F>)
    requires((sizeof...(Ns) > 0) && ((Ns == 0) && ...));

}  // end of namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/getForwardModeDerivativeFunction.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_GETFORWARDMODEDERIVATIVEFUNCTION_HXX */
