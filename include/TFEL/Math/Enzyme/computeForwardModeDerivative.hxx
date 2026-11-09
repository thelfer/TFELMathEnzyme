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

#include "TFEL/Math/Enzyme/Internals/FunctionUtilities.hxx"

namespace tfel::math::enzyme {

  /*!
   * \brief Compute the forward mode derivative of a callable.
   * \tparam CallableType: Type of the callable.
   * \tparam ArgumentsTypes: Types of the arguments passed to the callable.
   * \param[in] c: The callable to differentiate.
   * \param[in] args: Arguments passed to the callable.
   * \return The derivative of the callable with respect to its arguments.
   * \note This function only supports callables with a single argument.
   */
  template <internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeForwardModeDerivative(const CallableType&, ArgumentsTypes&&...)
    requires(std::is_invocable_v<CallableType, ArgumentsTypes...>);

  /*!
   * \brief Compute the forward mode derivative of a free function.
   * \tparam F: Pointer to the free function.
   * \tparam ArgumentType0: Type of the argument passed to the free function.
   * \param[in] f: Free function wrapper (created with
   *   tfel::math::enzyme::function).
   * \param[in] arg0: Argument passed to the free function.
   * \return The derivative of the function with respect to its argument.
   * \note Regular functions must be passed as template parameters via
   *   FunctionWrapper, as the symbol associated with the function must be seen
   *   by Enzyme.
   * \note This function only supports functions with a single argument.
   */
  template <internals::IsFunctionPointerConcept auto F, typename ArgumentType0>
  auto computeForwardModeDerivative(internals::FunctionWrapper<F>,
                                    ArgumentType0&&)  //
    requires(std::is_invocable_v<decltype(F), ArgumentType0>);

}  // end of namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/computeForwardModeDerivative.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_COMPUTEFORWARDMODEDERIVATIVE_HXX */
