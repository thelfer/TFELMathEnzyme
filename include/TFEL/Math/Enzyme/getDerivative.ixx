/*!
 * \file   TFEL/Math/Enzyme/getDerivative.ixx
 * \brief  This file implements the getDerivative function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_GETDERIVATIVE_IXX
#define LIB_TFEL_MATH_ENZYME_GETDERIVATIVE_IXX

#include "TFEL/Math/General/DerivativeType.hxx"

namespace tfel::math::enzyme::internals {

  template <std::size_t N,
            EnzymeCallableConcept CallableType,
            typename CallableArgumentType0>
  auto getDerivativeImplementation(
      const TypeList<CallableArgumentType0>&,
      const CallableType& c) requires((getArgumentsSize<CallableType>() == 1) &&
                                      (N == 0)) {
    using ResultType =
        std::invoke_result_t<CallableType, CallableArgumentType0>;
    using DerivativeResultType =
        derivative_type<ResultType, std::decay_t<CallableArgumentType0>>;
    if constexpr (ScalarConcept<CallableArgumentType0>) {
      // derivative with respect to a scalar
      return [c](CallableArgumentType0 v) {
        auto vdv = VariableValueAndIncrement<std::decay_t<CallableArgumentType0>>{
            .value = v, .increment = 1};
        return fwddiff(c, vdv);
      };
    } else if constexpr(ScalarConcept<ResultType>) {
      return [c](CallableArgumentType0 v) {
        auto vdv =
            VariableValueAndIncrement<std::decay_t<CallableArgumentType0>>{
                .value = v, .increment = {}};
        auto r = DerivativeResultType{};
        for (typename std::decay_t<CallableArgumentType0>::size_type i = 0; i != v.size();
             ++i) {
          vdv.increment[i] = 1;
          const auto dc = fwddiff(c, vdv);
          r(i) = dc;
          vdv.increment[i] = 0;
        }
        return r;
      };
    } else {
      // derivative with respect to a MathObject
      return [c](CallableArgumentType0 v) {
        auto vdv =
            VariableValueAndIncrement<std::decay_t<CallableArgumentType0>>{
                .value = v, .increment = {}};
        auto r = DerivativeResultType{};
        for (typename std::decay_t<CallableArgumentType0>::size_type i = 0;
             i != v.size(); ++i) {
          vdv.increment[i] = 1;
          const auto dc = fwddiff(c, vdv);
          for (typename ResultType::size_type j = 0; j != v.size(); ++j) {
            r(i, j) = dc(j);
          }
          vdv.increment[i] = 0;
        }
        return r;
      };
    }
  }  // end of getDerivativeImplementation

  template <std::size_t N, EnzymeCallableConcept CallableType>
  auto getDerivativeImplementation(const CallableType& c)
    requires((getArgumentsSize<CallableType>() <= 1) &&
             (getArgumentsSize<CallableType>() > N))
  {
    return getDerivativeImplementation<N>(getArgumentsList<CallableType>(), c);
  } // end of getDerivative

} // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  //   template <internals::EnzymeCallableConcept CallableType typename
  //   ArgumentType> auto computeForwardModeDerivative(const CallableType& c,
  //   ArgumentType&& arg)
  //     requires(std::is_invocable_v<CallableType, ArgumentType>)
  //   {
  //     return internals::computeForwardModeDerivativeImplementations(
  //         const CallableType& c, ArgumentType&& arg)
  //   }  // end of computeForwardModeDerivative

  template <std::size_t N,
            std::size_t... Ns,
            internals::EnzymeCallableConcept CallableType>
  auto getDerivative(const CallableType& c) {
    if constexpr (sizeof...(Ns) == 0) {
      return internals::getDerivativeImplementation<N>(c);
    } else {
      return getDerivative<N>(getDerivative<Ns...>(c));
    }
  }  // end of getDerivative

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_GETDERIVATIVE_IXX */

