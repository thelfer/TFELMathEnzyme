/*!
 * \file   TFEL/Math/Enzyme/computeReverseModeDerivative.ixx
 * \brief
 * \author Thomas Helfer
 * \date   19/08/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_COMPUTEREVERSEMODEDERIVATIVE_IXX
#define LIB_TFEL_MATH_ENZYME_COMPUTEREVERSEMODEDERIVATIVE_IXX

#include <tuple>
#include <utility>
#include <type_traits>
#include "TFEL/Math/General/DerivativeType.hxx"

namespace tfel::math::enzyme::internals {

  template <std::size_t N, typename DerivativeType>
  struct DerivativeHolder {
    DerivativeType value;
  };

  template <std::size_t N,
            typename CurrentDerivativeType,
            typename... DerivativesTypes>
  struct PackedDerivativesImplementation
      : DerivativeHolder<N, CurrentDerivativeType>,
        PackedDerivativesImplementation<N + 1, DerivativesTypes...> {};

  template <std::size_t N, typename DerivativeType>
  struct PackedDerivativesImplementation<N, DerivativeType>
      : DerivativeHolder<N, DerivativeType> {};

}  // namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <typename... DerivativesTypes>
  struct PackedDerivatives
      : internals::PackedDerivativesImplementation<0, DerivativesTypes...> {};

  template <std::size_t N, typename... DerivativesTypes>
  auto& get(PackedDerivatives<DerivativesTypes...>& derivatives) noexcept  //
      requires(N < sizeof...(DerivativesTypes)) {
    using DerivativeType =
        std::tuple_element_t<N, std::tuple<DerivativesTypes...>>;
    return static_cast<internals::DerivativeHolder<N, DerivativeType>&>(derivatives)
        .value;
  }  // end of get

  template <std::size_t N, typename... DerivativesTypes>
  const auto& get(
      const PackedDerivatives<DerivativesTypes...>& derivatives) noexcept  //
      requires(N < sizeof...(DerivativesTypes)) {
    using DerivativeType =
        std::tuple_element_t<N, std::tuple<DerivativesTypes...>>;
    return static_cast<const internals::DerivativeHolder<N, DerivativeType>&>(
               derivatives)
        .value;
  }  // end of get

}  // end of namespace tfel::math::enzyme

namespace std {

  template <typename... DerivativesTypes>
  struct tuple_size<::tfel::math::enzyme::PackedDerivatives<DerivativesTypes...>>
      : integral_constant<size_t, sizeof...(DerivativesTypes)> {};

  template <std::size_t N, typename... DerivativesTypes>
  struct tuple_element<N,
                       ::tfel::math::enzyme::PackedDerivatives<DerivativesTypes...>>
      : tuple_element<N, std::tuple<DerivativesTypes...>> {};

}  // namespace std

namespace tfel::math::enzyme::internals {

  template <std::size_t... idx,
            internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeReverseModeScalarFunctionDerivative(const CallableType&,
                                                ArgumentsTypes&&...)  //
      requires((sizeof...(ArgumentsTypes) > 0) &&
               (sizeof...(ArgumentsTypes) < 3) &&  //
               (sizeof...(idx) > 0) &&
               (sizeof...(idx) <= sizeof...(ArgumentsTypes)) &&
               ((idx < sizeof...(ArgumentsTypes)) && ...) &&
               (std::is_invocable_v<CallableType, ArgumentsTypes...>)&&  //
               (ScalarConcept<
                   std::invoke_result_t<CallableType, ArgumentsTypes...>>));

  template <internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeReverseModeScalarFunctionDerivative(const CallableType&,
                                                ArgumentsTypes&&...)     //
      requires((std::is_invocable_v<CallableType, ArgumentsTypes...>)&&  //
               (ScalarConcept<
                   std::invoke_result_t<CallableType, ArgumentsTypes...>>));

  template <std::size_t... idx,
            internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeReverseModeScalarFunctionDerivative(internals::FunctionWrapper<F>,
                                                ArgumentsTypes&&...)  //
      requires((sizeof...(ArgumentsTypes) > 0) &&
               (sizeof...(ArgumentsTypes) < 3) &&  //
               (sizeof...(idx) > 0) &&
               (sizeof...(idx) <= sizeof...(ArgumentsTypes)) &&
               ((idx < sizeof...(ArgumentsTypes)) && ...) &&
               (std::is_invocable_v<decltype(F), ArgumentsTypes...>)&&  //
               (ScalarConcept<
                   std::invoke_result_t<decltype(F), ArgumentsTypes...>>));

  template <internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeReverseModeScalarFunctionDerivative(internals::FunctionWrapper<F>,
                                                ArgumentsTypes&&...)    //
      requires((std::is_invocable_v<decltype(F), ArgumentsTypes...>)&&  //
               (ScalarConcept<
                   std::invoke_result_t<decltype(F), ArgumentsTypes...>>));

  template <EnzymeCallableConcept CallableType,
            typename CallableArgumentType,
            typename ArgumentType>
  auto computeReverseModeScalarFunctionDerivativeImplementation(
      const CallableType& c,
      const TypeList<CallableArgumentType>&,
      ArgumentType&& arg)  //
      requires(std::is_invocable_v<CallableType, ArgumentType>) {
    using ResultType = derivative_type<
        std::invoke_result_t<CallableType, CallableArgumentType>,
        std::decay_t<CallableArgumentType>>;
    checkCallEnzymeArgumentsConsistency(TypeList<CallableArgumentType>{},
                                        TypeList<ArgumentType>{});
    auto wrapper = [](const CallableType* const c,
                      const CallableArgumentType wargs) { return (*c)(wargs); };
    void* const wrapper_ptr = reinterpret_cast<void*>(+wrapper);
    const void* const c_ptr = reinterpret_cast<const void*>(&c);
    if constexpr (ScalarConcept<std::decay_t<CallableArgumentType>>) {
      return __enzyme_autodiff<ResultType>(
          wrapper_ptr,          //
          enzyme_const, c_ptr,  //
          enzyme_out, convertToEnzymeArgument<CallableArgumentType>(arg));
    } else {
      auto r = ResultType{};
      __enzyme_autodiff<void>(
          wrapper_ptr,          //
          enzyme_const, c_ptr,  //
          enzyme_dup, convertToEnzymeArgument<CallableArgumentType>(arg), &r);
      return r;
    }
  }

  template <std::size_t N, std::size_t... idx>
  constexpr void checkRedundancy() noexcept requires(sizeof...(idx) > 0) {
    const auto n = (((idx == N) ? 1 : 0) + ...);
    static_assert(n < 2, "redundant indices detected");
    if constexpr (N > 0) {
      return checkRedundancy<N - 1, idx...>();
    }
  }

  template <std::size_t... idx,
            EnzymeCallableConcept CallableType,
            typename CallableArgumentType0,
            typename CallableArgumentType1,
            typename ArgumentType0,
            typename ArgumentType1>
  auto computeReverseModeScalarFunctionDerivativeImplementation(
      const CallableType& c,
      const TypeList<CallableArgumentType0, CallableArgumentType1>&,
      ArgumentType0&& arg0,
      ArgumentType1&& arg1)  //
      requires((std::is_invocable_v<CallableType,
                                    ArgumentType0,
                                    ArgumentType1>)&&  //
               (sizeof...(idx) > 0) &&                 //
               (sizeof...(idx) <= 2) &&                //
               ((idx < 2) && ...)) {
    using CallableResultType =
        std::invoke_result_t<CallableType, CallableArgumentType0,
                             CallableArgumentType1>;
    checkCallEnzymeArgumentsConsistency(
        TypeList<CallableArgumentType0, CallableArgumentType1>{},
        TypeList<ArgumentType0, ArgumentType1>{});
    auto wrapper =
        [](const CallableType* const c, const CallableArgumentType0 wargs0,
           const CallableArgumentType1 wargs1) { return (*c)(wargs0, wargs1); };
    void* const wrapper_ptr = reinterpret_cast<void*>(+wrapper);
    const void* const c_ptr = reinterpret_cast<const void*>(&c);
    checkRedundancy<1, idx...>();
    constexpr auto b0 = ((idx == 0) || ...);
    constexpr auto b1 = ((idx == 1) || ...);
    if constexpr ((b0) && (!b1)) {
      using ResultType = derivative_type<CallableResultType,
                                         std::decay_t<CallableArgumentType0>>;
      if constexpr (ScalarConcept<std::decay_t<CallableArgumentType0>>) {
        return __enzyme_autodiff<ResultType>(
            wrapper_ptr, enzyme_const, c_ptr,  //
            enzyme_out,
            convertToEnzymeArgument<CallableArgumentType0>(arg0),  //
            enzyme_const, convertToEnzymeArgument<CallableArgumentType1>(arg1));
      } else {
        auto r = ResultType{};
        __enzyme_autodiff<void>(
            wrapper_ptr, enzyme_const, c_ptr,  //
            enzyme_dup, convertToEnzymeArgument<CallableArgumentType0>(arg0),
            &r,  //
            enzyme_const, convertToEnzymeArgument<CallableArgumentType1>(arg1));
        return r;
      }
    } else if constexpr ((!b0) && (b1)) {
      using ResultType = derivative_type<CallableResultType,
                                         std::decay_t<CallableArgumentType1>>;
      if constexpr (ScalarConcept<std::decay_t<CallableArgumentType1>>) {
        return __enzyme_autodiff<ResultType>(
            wrapper_ptr, enzyme_const,
            c_ptr,  //
            enzyme_const,
            convertToEnzymeArgument<CallableArgumentType0>(arg0),  //
            enzyme_out, convertToEnzymeArgument<CallableArgumentType1>(arg1));
      } else {
        auto r = ResultType{};
        __enzyme_autodiff<void>(
            wrapper_ptr, enzyme_const,
            c_ptr,  //
            enzyme_const,
            convertToEnzymeArgument<CallableArgumentType0>(arg0),  //
            enzyme_dup, convertToEnzymeArgument<CallableArgumentType1>(arg1),
            &r);
        return r;
      }
    } else {
      using DerivativeType0 =
          derivative_type<CallableResultType,
                          std::decay_t<CallableArgumentType0>>;
      using DerivativeType1 =
          derivative_type<CallableResultType,
                          std::decay_t<CallableArgumentType1>>;
      using ResultType = PackedDerivatives<DerivativeType0, DerivativeType1>;
      if constexpr ((ScalarConcept<std::decay_t<CallableArgumentType0>>)&&  //
                    (ScalarConcept<std::decay_t<CallableArgumentType1>>)) {
        return __enzyme_autodiff<ResultType>(
            wrapper_ptr, enzyme_const,
            c_ptr,  //
            enzyme_out,
            convertToEnzymeArgument<CallableArgumentType0>(arg0),  //
            enzyme_out, convertToEnzymeArgument<CallableArgumentType1>(arg1));
      } else if constexpr (ScalarConcept<std::decay_t<CallableArgumentType0>>) {
        auto r = ResultType{};
        std::get<0>(r) = __enzyme_autodiff<DerivativeType0>(
            wrapper_ptr, enzyme_const,
            c_ptr,  //
            enzyme_out,
            convertToEnzymeArgument<CallableArgumentType0>(arg0),  //
            enzyme_dup, convertToEnzymeArgument<CallableArgumentType1>(arg1),
            &(get<1>(r)));
        return r;
      } else if constexpr (ScalarConcept<std::decay_t<CallableArgumentType1>>) {
        auto r = ResultType{};
        std::get<1>(r) = __enzyme_autodiff<DerivativeType1>(
            wrapper_ptr, enzyme_const,
            c_ptr,  //
            enzyme_dup, convertToEnzymeArgument<CallableArgumentType0>(arg0),
            &(get<0>(r)),  //
            enzyme_out, convertToEnzymeArgument<CallableArgumentType1>(arg1));
        return r;
      } else {
        auto r = ResultType{};
        __enzyme_autodiff<void>(
            wrapper_ptr, enzyme_const,
            c_ptr,  //
            enzyme_dup, convertToEnzymeArgument<CallableArgumentType0>(arg0),
            &(get<0>(r)),  //
            enzyme_dup, convertToEnzymeArgument<CallableArgumentType1>(arg1),
            &(get<1>(r)));
        return r;
      }
    }
  }

  template <std::size_t... idx,
            EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeReverseModeScalarFunctionDerivative(const CallableType& c,
                                                ArgumentsTypes&&... args)  //
      requires((sizeof...(ArgumentsTypes) > 0) &&
               (sizeof...(ArgumentsTypes) < 3) &&  //
               (sizeof...(idx) > 0) &&
               (sizeof...(idx) <= sizeof...(ArgumentsTypes)) &&
               ((idx < sizeof...(ArgumentsTypes)) && ...) &&
               (std::is_invocable_v<CallableType, ArgumentsTypes...>)&&  //
               (ScalarConcept<
                   std::invoke_result_t<CallableType, ArgumentsTypes...>>)) {
    constexpr auto nargs = sizeof...(ArgumentsTypes);
    if constexpr (nargs == 1) {
      return computeReverseModeScalarFunctionDerivativeImplementation(
          c, getArgumentsList<CallableType>(),
          std::forward<ArgumentsTypes>(args)...);
    } else if constexpr (nargs == 2) {
      return computeReverseModeScalarFunctionDerivativeImplementation<idx...>(
          c, getArgumentsList<CallableType>(),
          std::forward<ArgumentsTypes>(args)...);
    }
  }  // end of computeReverseModeScalarFunctionDerivative

  template <std::size_t idx,
            EnzymeCallableConcept CallableType,
            typename... CallableArgumentsTypes,
            typename... ArgumentsTypes>
  auto computeReverseModeMathObjectFunctionDerivative(
      const CallableType& c,
      const TypeList<CallableArgumentsTypes...>,
      ArgumentsTypes&&... args)  //
      requires((sizeof...(CallableArgumentsTypes) ==
                sizeof...(ArgumentsTypes)) &&
               (idx < sizeof...(ArgumentsTypes)) &&
               (std::is_invocable_v<CallableType, ArgumentsTypes...>)&&  //
               (MathObjectConcept<
                   std::invoke_result_t<CallableType, ArgumentsTypes...>>)) {
    using VariableType = std::decay_t<
        std::tuple_element_t<idx, std::tuple<CallableArgumentsTypes...>>>;
    using CallableResultType =
        std::invoke_result_t<CallableType, CallableArgumentsTypes...>;
    using ResultType = derivative_type<CallableResultType, VariableType>;
    constexpr auto callable_result_arity =
        CallableResultType::indexing_policy::arity;
    using size_type = typename ResultType::size_type;
    auto r = ResultType{};
    static_assert(callable_result_arity == 1);
    for (size_type i = 0; i != r.size(0); ++i) {
      auto wrapper = [c, i](CallableArgumentsTypes... wargs) {
        auto result = c(wargs...);
        return result[i];
      };
      const auto row = computeReverseModeDerivative<idx>(wrapper, args...);
      if constexpr (ScalarConcept<decltype(row)>) {
        // derivation with respect to a scalar
        r[i] = row;
      } else {
        // derivation with respect to a math object
        constexpr auto variable_result_arity =
            VariableType::indexing_policy::arity;
        static_assert((variable_result_arity == 1) ||
                      (variable_result_arity == 2));
        if constexpr (variable_result_arity == 1) {
          for (size_type vj = 0; vj != row.size(); ++vj) {
            r(i, vj) = row[vj];
          }
        } else {
          for (size_type vj = 0; vj != row.size(0); ++vj) {
            for (size_type vk = 0; vk != row.size(0); ++vk) {
              r(i, vj, vk) = row(vj, vk);
            }
          }
        }
      }
    }
    return r;
  }  // end of computeReverseModeMathObjectFunctionDerivative

  template <std::size_t... idx,
            IsFunctionPointerConcept auto F,
            typename... CallableArgumentsTypes,
            typename... ArgumentsTypes>
  auto computeReverseModeDerivativeImplementation(
      FunctionWrapper<F>,
      const TypeList<CallableArgumentsTypes...>&,
      ArgumentsTypes&&... args)  //
      requires((sizeof...(CallableArgumentsTypes) ==
                sizeof...(ArgumentsTypes)) &&
               (std::is_invocable_v<decltype(F), CallableArgumentsTypes...>)) {
    auto c = [](const CallableArgumentsTypes... wargs) { return F(wargs...); };
    return ::tfel::math::enzyme::computeReverseModeDerivative<idx...>(
        c, std::forward<ArgumentsTypes>(args)...);
  }  // end of computeReverseModeDerivativeImplementation

  template <std::size_t... idx,
            EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeReverseModeDerivativeImplementation(
      const std::index_sequence<idx...>,
      const CallableType& c,
      ArgumentsTypes&&... arg)  //
      requires(std::is_invocable_v<CallableType, ArgumentsTypes...>) {
    return ::tfel::math::enzyme::computeReverseModeDerivative<idx...>(
        c, std::forward<ArgumentsTypes>(arg)...);
  }  // end of computeReverseModeDerivativeImplementation

  template <std::size_t... idx,
            IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeReverseModeDerivativeImplementation(
      const std::index_sequence<idx...>,
      FunctionWrapper<F> f,
      ArgumentsTypes&&... arg)  //
      requires(std::is_invocable_v<decltype(F), ArgumentsTypes...>) {
    return ::tfel::math::enzyme::computeReverseModeDerivative<idx...>(
        f, std::forward<ArgumentsTypes>(arg)...);
  }  // end of computeReverseModeDerivativeImplementation

}  // namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <std::size_t... idx,
            internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeReverseModeDerivative(const CallableType& c,
                                  ArgumentsTypes&&... args)  //
      requires((sizeof...(ArgumentsTypes) > 0) &&
               (sizeof...(ArgumentsTypes) < 3) &&  //
               (sizeof...(idx) > 0) &&
               (sizeof...(idx) <= sizeof...(ArgumentsTypes)) &&
               ((idx < sizeof...(ArgumentsTypes)) && ...) &&
               (std::is_invocable_v<CallableType, ArgumentsTypes...>)&&  //
               (VariableConcept<
                   std::invoke_result_t<CallableType, ArgumentsTypes...>>)) {
    using CallableResultType =
        std::invoke_result_t<CallableType, ArgumentsTypes...>;
    if constexpr (ScalarConcept<CallableResultType>) {
      return internals::computeReverseModeScalarFunctionDerivative<idx...>(
          c, std::forward<ArgumentsTypes>(args)...);
    } else {
      static_assert(sizeof...(idx) == 1,
                    "callable returning a non-scalar value can't be "
                    "differentiated with respect to more than a variable");
      return internals::computeReverseModeMathObjectFunctionDerivative<idx...>(
          c, internals::getArgumentsList<CallableType>(),
          std::forward<ArgumentsTypes>(args)...);
    }
  }  // end of computeReverseModeDerivative

  template <std::size_t... idx,
            internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeReverseModeDerivative(internals::FunctionWrapper<F> f,
                                  ArgumentsTypes&&... args)  //
      requires((sizeof...(ArgumentsTypes) > 0) &&
               (sizeof...(ArgumentsTypes) < 3) &&  //
               (sizeof...(idx) > 0) &&
               (sizeof...(idx) <= sizeof...(ArgumentsTypes)) &&
               ((idx < sizeof...(ArgumentsTypes)) && ...) &&
               (std::is_invocable_v<decltype(F), ArgumentsTypes...>)&&  //
               (VariableConcept<
                   std::invoke_result_t<decltype(F), ArgumentsTypes...>>)) {
    return internals::computeReverseModeDerivativeImplementation<idx...>(
        f, internals::getArgumentsList<decltype(F)>(),
        std::forward<ArgumentsTypes>(args)...);
  }  // end of computeReverseModeDerivative

  template <internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeReverseModeDerivative(const CallableType& c,
                                  ArgumentsTypes&&... args)              //
      requires((std::is_invocable_v<CallableType, ArgumentsTypes...>)&&  //
               (VariableConcept<
                   std::invoke_result_t<CallableType, ArgumentsTypes...>>)) {
    return internals::computeReverseModeDerivativeImplementation(
        std::make_index_sequence<sizeof...(ArgumentsTypes)>{}, c,
        std::forward<ArgumentsTypes>(args)...);
  }  // end of computeReverseModeDerivative

  template <internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeReverseModeDerivative(internals::FunctionWrapper<F> f,
                                  ArgumentsTypes&&... args)             //
      requires((std::is_invocable_v<decltype(F), ArgumentsTypes...>)&&  //
               (VariableConcept<
                   std::invoke_result_t<decltype(F), ArgumentsTypes...>>)) {
    return internals::computeReverseModeDerivativeImplementation(
        std::make_index_sequence<sizeof...(ArgumentsTypes)>{}, f,
        std::forward<ArgumentsTypes>(args)...);
  }  // end of computeReverseModeDerivative

}  // namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_COMPUTEREVERSEMODEDERIVATIVE_IXX */
