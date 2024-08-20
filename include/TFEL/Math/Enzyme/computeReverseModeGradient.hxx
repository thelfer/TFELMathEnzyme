/*!
 * \file   TFEL/Math/Enzyme/computeReverseModeGradient.hxx
 * \brief
 * \author Thomas Helfer
 * \date   19/08/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_COMPUTEREVERSEMODEGRADIENT_HXX
#define LIB_TFEL_MATH_ENZYME_COMPUTEREVERSEMODEGRADIENT_HXX

#include <cstddef>
#include "TFEL/Math/Enzyme/Internals/Enzyme.hxx"
#include "TFEL/Math/Enzyme/Variable.hxx"
#include "TFEL/Math/Enzyme/Internals/FunctionUtilities.hxx"

namespace tfel::math::enzyme {

  /*!
   * \brief compute the gradient of a callable with respect to the variables
   * designated by the indices `idx`
   * \tparam idx: indices of the variables with respect to which the gradients
   * are computed. \tparam CallableType: type of the callable \tparam
   * ArgumentsTypes: types of the arguments passed to the callable \param[in] c:
   * callable \param[in] args: arguments passed to the callable
   */
  template <std::size_t... idx,
            internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeReverseModeGradient(const CallableType&,
                                  ArgumentsTypes&&...)  //
    requires((sizeof...(ArgumentsTypes) > 0) &&
             (sizeof...(ArgumentsTypes) < 3) &&  //
             (sizeof...(idx) > 0) &&
             (sizeof...(idx) <= sizeof...(ArgumentsTypes)) &&
             ((idx < sizeof...(ArgumentsTypes)) && ...) &&
             (std::is_invocable_v<CallableType, ArgumentsTypes...>) &&  //
             (VariableConcept<
                 std::invoke_result_t<CallableType, ArgumentsTypes...>>));
  /*!
   * \brief compute the gradient of a callable with respect to all the
   * variables. \tparam CallableType: type of the callable \tparam
   * ArgumentsTypes: types of the arguments passed to the callable \param[in] c:
   * callable \param[in] args: arguments passed to the callable
   */
  template <internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeReverseModeGradient(const CallableType&,
                                  ArgumentsTypes&&...)                  //
    requires((std::is_invocable_v<CallableType, ArgumentsTypes...>) &&  //
             (VariableConcept<
                 std::invoke_result_t<CallableType, ArgumentsTypes...>>));
  /*!
   * \brief compute the gradient of a free function with respect to the
   * variables designated by the indices `idx`.
   * \tparam idx: indices of the
   * variables with respect to which the gradients are computed. \tparam F:
   * pointer to the free function \tparam ArgumentsTypes: types of the arguments
   * passed to the free function \param[in] f: free function warpper \param[in]
   * args: arguments passed to the free function
   */
  template <std::size_t... idx,
            internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeReverseModeGradient(internals::FunctionWrapper<F>,
                                  ArgumentsTypes&&...)  //
    requires(
        (sizeof...(ArgumentsTypes) > 0) &&
        (sizeof...(ArgumentsTypes) < 3) &&  //
        (sizeof...(idx) > 0) && (sizeof...(idx) <= sizeof...(ArgumentsTypes)) &&
        ((idx < sizeof...(ArgumentsTypes)) && ...) &&
        (std::is_invocable_v<decltype(F), ArgumentsTypes...>) &&  //
        (VariableConcept<std::invoke_result_t<decltype(F), ArgumentsTypes...>>))
  ;
  /*!
   * \brief compute the gradient of a free function with respect to all
   * variables \tparam F: pointer to the free function \tparam ArgumentsTypes:
   * types of the arguments passed to the free function \param[in] f: free
   * function warpper \param[in] args: arguments passed to the free function
   */
  template <internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeReverseModeGradient(internals::FunctionWrapper<F>,
                                  ArgumentsTypes&&...)  //
    requires(
        (std::is_invocable_v<decltype(F), ArgumentsTypes...>) &&  //
        (VariableConcept<std::invoke_result_t<decltype(F), ArgumentsTypes...>>))
  ;

}  // namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/computeReverseModeGradient.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_COMPUTEREVERSEMODEGRADIENT_HXX */
