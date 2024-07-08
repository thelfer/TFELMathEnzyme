/*!
 * \file   TFEL/Math/Enzyme/GetPartialDerivative.hxx
 * \brief  This hearder declares the getPartialDerivative functions
 * \author Thomas Helfer
 * \date   08/07/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_GETPARTIALDERIVATIVE_HXX
#define LIB_TFEL_MATH_ENZYME_GETPARTIALDERIVATIVE_HXX

#include "TFEL/Math/Enzyme/Variable.hxx"
#include "TFEL/Math/Enzyme/Internals/CallEnzymeForwardDifferentiate.hxx"

namespace tfel::math::enzyme {

  /*!
   * \returns a callable object computing the derivative of the callable object
   * in argument
   * \tparam N: index of the variable with respect to which the function is
   * differentiated
   * \tparam CallableType: type of the callable
   * \tparam VariablesTypes: type of the variables of the callable
   * \param[in] c: callable object
   */
  template <std::size_t N,
            VariableConcept... VariablesTypes,
            typename CallableType>
  [[nodiscard]] auto
  getPartialDerivative(const CallableType&) noexcept requires(
      (std::is_invocable_v<CallableType, VariablesTypes...>)&&  //
      (N < sizeof...(VariablesTypes)));

  /*!
   * \returns a callable object computing the derivative of the callable object
   * in argument
   * \tparam CallableType: type of the callable
   * \tparam VariableType: type of the variable of the callable
   * \param[in] c: callable object
   */
  template <VariableConcept VariableType, typename CallableType>
  [[nodiscard]] auto getDerivative(const CallableType&) noexcept;

} // end of tfel::math::enzyme

#include "TFEL/Math/Enzyme/GetPartialDerivative.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_GETPARTIALDERIVATIVE_HXX */
