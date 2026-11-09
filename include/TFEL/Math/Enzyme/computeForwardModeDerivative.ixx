/*!
 * \file   TFEL/Math/Enzyme/computeForwardModeDerivative.ixx
 * \brief  This file implements the computeForwardModeDerivative function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_COMPUTEFORWARDMODEDERIVATIVE_IXX
#define LIB_TFEL_MATH_ENZYME_COMPUTEFORWARDMODEDERIVATIVE_IXX

#include "TFEL/Math/General/DerivativeType.hxx"
#include "TFEL/Math/Enzyme/fwddiff.hxx"

namespace tfel::math::enzyme::internals {

  /*!
   * \brief Implementation of computeForwardModeDerivative
   * \tparam CallableType: the callable type
   * \tparam CallableArgumentType0: the expected argument type
   * \tparam ArgumentType0: the actual argument type
   * \param[in] c: the callable
   * \param[in] args_list: type list of callable arguments (unused, for SFINAE)
   * \param[in] arg0: the argument
   * \return the derivative of the callable with respect to arg0
   */
  template <EnzymeCallableConcept CallableType,
            typename CallableArgumentType0,
            typename ArgumentType0>
  auto computeForwardModeDerivativeImplementation(
      const CallableType& c,
      const TypeList<CallableArgumentType0>&,
      ArgumentType0&& arg0)
    requires(std::is_invocable_v<CallableType, ArgumentType0>)
  {
    using ResultType =
        std::invoke_result_t<CallableType, CallableArgumentType0>;
    using DerivativeResultType =
        derivative_type<ResultType, std::decay_t<CallableArgumentType0>>;
    // the derivative with respect to a matrix like object is a 3 or 4-index
    // object, that the implementation below cannot fill, as it only accesses
    // the derivative using two indices at most
    static_assert((internals::getArity<CallableArgumentType0>() <= 1) &&
                      (internals::getArity<ResultType>() <= 1),
                  "forward mode only supports differentiating callables of a "
                  "scalar or a vector like math object with respect to a "
                  "scalar or a vector like math object");
    if constexpr (ScalarConcept<CallableArgumentType0>) {
      // derivative with respect to a scalar
      auto vdv = VariableValueAndIncrement<std::decay_t<CallableArgumentType0>>{
          .value = arg0, .increment = 1};
      return fwddiff(c, vdv);
    } else if constexpr (ScalarConcept<ResultType>) {
      auto vdv = VariableValueAndIncrement<std::decay_t<CallableArgumentType0>>{
          .value = arg0, .increment = {}};
      auto r = DerivativeResultType{};
      for (typename std::decay_t<CallableArgumentType0>::size_type i = 0;
           i != vdv.value.size(); ++i) {
        vdv.increment[i] = 1;
        const auto dc = fwddiff(c, vdv);
        r(i) = dc;
        vdv.increment[i] = 0;
      }
      return r;
    } else {
      // derivative with respect to a MathObject
      // the derivative is stored as `r(i, j) = d\,result(i) / d\,variable(j)`,
      // following the same convention as the reverse mode (and as TFEL, where
      // a derivative maps increments to increments: `increment_result =
      // derivative * increment_variable`)
      auto vdv = VariableValueAndIncrement<std::decay_t<CallableArgumentType0>>{
          .value = arg0, .increment = {}};
      auto r = DerivativeResultType{};
      for (typename std::decay_t<CallableArgumentType0>::size_type j = 0;
           j != vdv.value.size(); ++j) {
        vdv.increment[j] = 1;
        const auto dc = fwddiff(c, vdv);
        for (typename ResultType::size_type i = 0; i != r.size(0); ++i) {
          r(i, j) = dc(i);
        }
        vdv.increment[j] = 0;
      }
      return r;
    }
  }  // end of computeForwardModeDerivativeImplementation

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  /*!
   * \brief Compute the forward mode derivative of a callable
   * \tparam CallableType: the callable type
   * \tparam ArgumentsTypes: the argument types
   * \param[in] c: the callable
   * \param[in] args: the arguments to pass to the callable
   * \return the derivative of the callable with respect to its arguments
   * \note Only callables with a single argument are supported
   */
  template <internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeForwardModeDerivative(const CallableType& c,
                                    ArgumentsTypes&&... args)
    requires(std::is_invocable_v<CallableType, ArgumentsTypes...>)
  {
    static_assert(sizeof...(ArgumentsTypes) == 1,
                  "only callable of one variable are supported");
    return internals::computeForwardModeDerivativeImplementation(
        c, internals::getArgumentsList<CallableType>(),
        std::forward<ArgumentsTypes>(args)...);
  }  // end of computeForwardModeDerivative

}  // end of namespace tfel::math::enzyme

namespace tfel::math::enzyme::internals {

  /*!
   * \brief Implementation of computeForwardModeDerivative for function
   * pointers
   * \tparam F: the function pointer
   * \tparam CallableArgumentType0: the argument type of the function
   * \tparam ArgumentType0: the type of the argument passed
   * \param[in] f: the function wrapper
   * \param[in] args_list: type list of the argument of the function (unused,
   * for SFINAE)
   * \param[in] arg0: the argument
   * \return the derivative of the function with respect to its argument
   */
  template <internals::IsFunctionPointerConcept auto F,
            typename CallableArgumentType0,
            typename ArgumentType0>
  auto computeForwardModeDerivativeImplementation(
      internals::FunctionWrapper<F>,
      const TypeList<CallableArgumentType0>&,
      ArgumentType0&& arg0)
    requires(std::is_invocable_v<decltype(F), ArgumentType0>)
  {
    auto c = [](const CallableArgumentType0 warg) { return F(warg); };
    return ::tfel::math::enzyme::computeForwardModeDerivative(c, arg0);
  }  // end of computeForwardModeDerivativeImplementation

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  /*!
   * \brief Compute the forward mode derivative of a function pointer
   * \tparam F: the function pointer
   * \tparam ArgumentType0: the type of the argument passed
   * \param[in] f: the function wrapper
   * \param[in] arg0: the argument
   * \return the derivative of the function with respect to its argument
   */
  template <internals::IsFunctionPointerConcept auto F, typename ArgumentType0>
  auto computeForwardModeDerivative(internals::FunctionWrapper<F> f,
                                    ArgumentType0&& arg0)
    requires(std::is_invocable_v<decltype(F), ArgumentType0>)
  {
    return internals::computeForwardModeDerivativeImplementation(
        f, internals::getArgumentsList<decltype(F)>(),
        std::forward<ArgumentType0>(arg0));
  }  // end of computeForwardModeDerivative

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_COMPUTEFORWARDMODEDERIVATIVE_IXX */
