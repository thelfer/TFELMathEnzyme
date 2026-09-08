/*!
 * \file   TFEL/Math/Enzyme/computeReverseModeDerivative.hxx
 * \brief
 * \author Thomas Helfer
 * \date   19/08/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_COMPUTEREVERSEMODEDERIVATIVE_HXX
#define LIB_TFEL_MATH_ENZYME_COMPUTEREVERSEMODEDERIVATIVE_HXX

#include <cstddef>
#include "TFEL/Math/Enzyme/Internals/Enzyme.hxx"
#include "TFEL/Math/Enzyme/Variable.hxx"
#include "TFEL/Math/Enzyme/Internals/FunctionUtilities.hxx"

namespace tfel::math::enzyme {

  /*!
   * \brief Compute the derivative of a callable with respect to the variables
   * designated by the indices `idx` using reverse mode automatic
   * differentiation \tparam idx: indices of the variables with respect to which
   * the derivatives are computed \tparam CallableType: type of the callable
   * \tparam ArgumentsTypes: types of the arguments passed to the callable
   * \param[in] c: callable
   * \param[in] args: arguments passed to the callable
   * \return the derivative(s) of the callable result with respect to the
   * specified variables \note Supports 1 or 2 arguments, and can compute
   * derivatives with respect to multiple variables at once
   */
  template <std::size_t... idx,
            internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeReverseModeDerivative(const CallableType&,
                                    ArgumentsTypes&&...)  //
      requires((sizeof...(ArgumentsTypes) > 0) &&
               (sizeof...(ArgumentsTypes) < 3) &&  //
               (sizeof...(idx) > 0) &&
               (sizeof...(idx) <= sizeof...(ArgumentsTypes)) &&
               ((idx < sizeof...(ArgumentsTypes)) && ...) &&
               (std::is_invocable_v<CallableType, ArgumentsTypes...>)&&  //
               (VariableConcept<
                   std::invoke_result_t<CallableType, ArgumentsTypes...>>));
  /*!
   * \brief Compute the derivative of a callable with respect to all the
   * variables using reverse mode automatic differentiation
   * \tparam CallableType: type of the callable
   * \tparam ArgumentsTypes: types of the arguments passed to the callable
   * \param[in] c: callable
   * \param[in] args: arguments passed to the callable
   * \return the derivatives of the callable result with respect to all
   * variables \note This version computes derivatives with respect to all input
   * variables
   */
  template <internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeReverseModeDerivative(const CallableType&,
                                    ArgumentsTypes&&...)                 //
      requires((std::is_invocable_v<CallableType, ArgumentsTypes...>)&&  //
               (VariableConcept<
                   std::invoke_result_t<CallableType, ArgumentsTypes...>>));
  /*!
   * \brief Compute the derivative of a free function with respect to the
   * variables designated by the indices `idx` using reverse mode
   * \tparam idx: indices of the variables with respect to which the derivatives
   * are computed
   * \tparam F: pointer to the free function
   * \tparam ArgumentsTypes: types of the arguments passed to the free function
   * \param[in] f: free function wrapper (created with
   * tfel::math::enzyme::function) \param[in] args: arguments passed to the free
   * function \return the derivative(s) of the function result with respect to
   * the specified variables \note Regular functions must be passed as template
   * parameters via FunctionWrapper
   */
  template <std::size_t... idx,
            internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeReverseModeDerivative(internals::FunctionWrapper<F>,
                                    ArgumentsTypes&&...)  //
      requires((sizeof...(ArgumentsTypes) > 0) &&
               (sizeof...(ArgumentsTypes) < 3) &&  //
               (sizeof...(idx) > 0) &&
               (sizeof...(idx) <= sizeof...(ArgumentsTypes)) &&
               ((idx < sizeof...(ArgumentsTypes)) && ...) &&
               (std::is_invocable_v<decltype(F), ArgumentsTypes...>)&&  //
               (VariableConcept<
                   std::invoke_result_t<decltype(F), ArgumentsTypes...>>));
  /*!
   * \brief Compute the derivative of a free function with respect to all
   * variables using reverse mode
   * \tparam F: pointer to the free function
   * \tparam ArgumentsTypes: types of the arguments passed to the free function
   * \param[in] f: free function wrapper (created with
   * tfel::math::enzyme::function) \param[in] args: arguments passed to the free
   * function \return the derivatives of the function result with respect to all
   * variables \note Regular functions must be passed as template parameters via
   * FunctionWrapper
   */
  template <internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeReverseModeDerivative(internals::FunctionWrapper<F>,
                                    ArgumentsTypes&&...)                //
      requires((std::is_invocable_v<decltype(F), ArgumentsTypes...>)&&  //
               (VariableConcept<
                   std::invoke_result_t<decltype(F), ArgumentsTypes...>>));

}  // namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/computeReverseModeDerivative.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_COMPUTEREVERSEMODEDERIVATIVE_HXX */
