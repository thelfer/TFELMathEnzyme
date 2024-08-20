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

  //   template <std::size_t N,
  //             EnzymeCallableConcept CallableType,
  //             typename CallableArgumentType0>
  //   auto getDerivativeImplementation(const TypeList<CallableArgumentType0>&,
  //                                    const CallableType& c)
  //     requires((getArgumentsSize<CallableType>() == 1) && (N == 0))
  //   {
  //     using ResultType =
  //         std::invoke_result_t<CallableType, CallableArgumentType0>;
  //     using DerivativeResultType =
  //         derivative_type<ResultType, std::decay_t<CallableArgumentType0>>;
  //     if constexpr (ScalarConcept<CallableArgumentType0>) {
  //       // derivative with respect to a scalar
  //       return [c](CallableArgumentType0 v) {
  //         auto vdv =
  //             VariableValueAndIncrement<std::decay_t<CallableArgumentType0>>{
  //                 .value = v, .increment = 1};
  //         return fwddiff(c, vdv);
  //       };
  //     } else if constexpr (ScalarConcept<ResultType>) {
  //       return [c](CallableArgumentType0 v) {
  //         auto vdv =
  //             VariableValueAndIncrement<std::decay_t<CallableArgumentType0>>{
  //                 .value = v, .increment = {}};
  //         auto r = DerivativeResultType{};
  //         for (typename std::decay_t<CallableArgumentType0>::size_type i = 0;
  //              i != v.size(); ++i) {
  //           vdv.increment[i] = 1;
  //           const auto dc = fwddiff(c, vdv);
  //           r(i) = dc;
  //           vdv.increment[i] = 0;
  //         }
  //         return r;
  //       };
  //     } else {
  //       // derivative with respect to a MathObject
  //       return [c](CallableArgumentType0 v) {
  //         auto vdv =
  //             VariableValueAndIncrement<std::decay_t<CallableArgumentType0>>{
  //                 .value = v, .increment = {}};
  //         auto r = DerivativeResultType{};
  //         for (typename std::decay_t<CallableArgumentType0>::size_type i = 0;
  //              i != v.size(); ++i) {
  //           vdv.increment[i] = 1;
  //           const auto dc = fwddiff(c, vdv);
  //           for (typename ResultType::size_type j = 0; j != v.size(); ++j) {
  //             r(i, j) = dc(j);
  //           }
  //           vdv.increment[i] = 0;
  //         }
  //         return r;
  //       };
  //     }
  //   }  // end of getDerivativeImplementation
  //
  //   template <std::size_t N, EnzymeCallableConcept CallableType>
  //   auto getDerivativeImplementation(const CallableType& c)
  //     requires((getArgumentsSize<CallableType>() <= 1) &&
  //              (getArgumentsSize<CallableType>() > N))
  //   {
  //     return getDerivativeImplementation<N>(getArgumentsList<CallableType>(),
  //     c);
  //   }  // end of getDerivative

  template <EnzymeCallableConcept CallableType,
            typename CallableArgumentType0,
            typename ArgumentType0>
  auto computeForwardModeGradientImplementation(
      const CallableType& c,
      const TypeList<CallableArgumentType0>&,
      ArgumentType0&& arg0)
    requires(std::is_invocable_v<CallableType, ArgumentType0>)
  {
    using ResultType =
        std::invoke_result_t<CallableType, CallableArgumentType0>;
    using DerivativeResultType =
        derivative_type<ResultType, std::decay_t<CallableArgumentType0>>;
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
      auto vdv = VariableValueAndIncrement<std::decay_t<CallableArgumentType0>>{
          .value = arg0, .increment = {}};
      auto r = DerivativeResultType{};
      for (typename std::decay_t<CallableArgumentType0>::size_type i = 0;
           i != r.size(0); ++i) {
        vdv.increment[i] = 1;
        const auto dc = fwddiff(c, vdv);
        for (typename ResultType::size_type j = 0; j != vdv.value.size(); ++j) {
          r(i, j) = dc(j);
        }
        vdv.increment[i] = 0;
      }
      return r;
    }
  }  // end of computeForwardModeGradientImplementation

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeForwardModeGradient(const CallableType& c,
                                  ArgumentsTypes&&... args)
    requires(std::is_invocable_v<CallableType, ArgumentsTypes...>)
  {
    static_assert(sizeof...(ArgumentsTypes) == 1,
                  "only callable of one variable are supported");
    return internals::computeForwardModeGradientImplementation(
        c, internals::getArgumentsList<CallableType>(),
        std::forward<ArgumentsTypes>(args)...);
  }  // end of computeForwardModeGradient

}  // end of namespace tfel::math::enzyme

namespace tfel::math::enzyme::internals {

  template <std::size_t N,
            std::size_t... Ns,
            internals::EnzymeCallableConcept CallableType,
            typename CallableArgumentType0>
  auto getForwardModeGradientFunctionImplementation(
      const CallableType& c, const TypeList<CallableArgumentType0> args_list)
    requires(N == 0)
  {
    auto dc = [c](CallableArgumentType0 warg) {
      return ::tfel::math::enzyme::computeForwardModeGradient(c, warg);
    };
    if constexpr (sizeof...(Ns) == 0) {
      return dc;
    } else {
      return getForwardModeGradientFunctionImplementation<Ns...>(dc, args_list);
    }
  }  // end of getForwardModeGradientFunctionImplementation

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <std::size_t... Ns, internals::EnzymeCallableConcept CallableType>
  auto getForwardModeGradientFunction(const CallableType& c)
    requires(sizeof...(Ns) > 0)
  {
    return internals::getForwardModeGradientFunctionImplementation<Ns...>(
        c, internals::getArgumentsList<CallableType>());
  }  // end of getForwardModeGradientFunction

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_GETDERIVATIVE_IXX */
