/*!
 * \file   TFEL/Math/Enzyme/autodiff.ixx
 * \brief
 * \author Thomas Helfer
 * \date   19/08/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_AUTODIFF_IXX
#define LIB_TFEL_MATH_ENZYME_AUTODIFF_IXX

#include <tuple>
#include <utility>
#include <type_traits>
#include "TFEL/Math/General/DerivativeType.hxx"

namespace tfel::math::enzyme::internals {

  template <std::size_t N, typename GradientType>
  struct GradientHolder {
    GradientType value;
  };

  template <std::size_t N,
            typename CurrentGradientType,
            typename... GradientsTypes>
  struct PackedGradientsImplementation
      : GradientHolder<N, CurrentGradientType>,
        PackedGradientsImplementation<N + 1, GradientsTypes...> {};

  template <std::size_t N, typename GradientType>
  struct PackedGradientsImplementation<N, GradientType>
      : GradientHolder<N, GradientType> {};

}  // namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <typename... GradientsTypes>
  struct PackedGradients
      : internals::PackedGradientsImplementation<0, GradientsTypes...> {};

  template <std::size_t N, typename... GradientsTypes>
  auto& get(PackedGradients<GradientsTypes...>& gradients) noexcept  //
    requires(N < sizeof...(GradientsTypes))
  {
    using DerivativeType =
        std::tuple_element_t<N, std::tuple<GradientsTypes...>>;
    return static_cast<internals::GradientHolder<N, DerivativeType>&>(gradients)
        .value;
  }  // end of get

  template <std::size_t N, typename... GradientsTypes>
  const auto& get(
      const PackedGradients<GradientsTypes...>& gradients) noexcept  //
    requires(N < sizeof...(GradientsTypes))
  {
    using DerivativeType =
        std::tuple_element_t<N, std::tuple<GradientsTypes...>>;
    return static_cast<const internals::GradientHolder<N, DerivativeType>&>(
               gradients)
        .value;
  }  // end of get

}  // end of namespace tfel::math::enzyme

namespace std {

  template <typename... GradientsTypes>
  struct tuple_size<::tfel::math::enzyme::PackedGradients<GradientsTypes...>>
      : integral_constant<size_t, sizeof...(GradientsTypes)> {};

  template <std::size_t N, typename... GradientsTypes>
  struct tuple_element<N,
                       ::tfel::math::enzyme::PackedGradients<GradientsTypes...>>
      : tuple_element<N, std::tuple<GradientsTypes...>> {};

}  // namespace std

namespace tfel::math::enzyme::internals {

  template <std::size_t... idx,
            internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeScalarFunctionGradient(const CallableType&,
                                     ArgumentsTypes&&...)  //
    requires(
        (sizeof...(ArgumentsTypes) > 0) &&
        (sizeof...(ArgumentsTypes) < 3) &&  //
        (sizeof...(idx) > 0) && (sizeof...(idx) <= sizeof...(ArgumentsTypes)) &&
        ((idx < sizeof...(ArgumentsTypes)) && ...) &&
        (std::is_invocable_v<CallableType, ArgumentsTypes...>) &&  //
        (ScalarConcept<std::invoke_result_t<CallableType, ArgumentsTypes...>>));

  template <internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeScalarFunctionGradient(const CallableType&,
                                     ArgumentsTypes&&...)  //
    requires(
        (std::is_invocable_v<CallableType, ArgumentsTypes...>) &&  //
        (ScalarConcept<std::invoke_result_t<CallableType, ArgumentsTypes...>>));

  template <std::size_t... idx,
            internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeScalarFunctionGradient(internals::FunctionWrapper<F>,
                                     ArgumentsTypes&&...)  //
    requires(
        (sizeof...(ArgumentsTypes) > 0) &&
        (sizeof...(ArgumentsTypes) < 3) &&  //
        (sizeof...(idx) > 0) && (sizeof...(idx) <= sizeof...(ArgumentsTypes)) &&
        ((idx < sizeof...(ArgumentsTypes)) && ...) &&
        (std::is_invocable_v<decltype(F), ArgumentsTypes...>) &&  //
        (ScalarConcept<std::invoke_result_t<decltype(F), ArgumentsTypes...>>));

  template <internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeScalarFunctionGradient(internals::FunctionWrapper<F>,
                                     ArgumentsTypes&&...)  //
    requires(
        (std::is_invocable_v<decltype(F), ArgumentsTypes...>) &&  //
        (ScalarConcept<std::invoke_result_t<decltype(F), ArgumentsTypes...>>));

  template <EnzymeCallableConcept CallableType,
            typename CallableArgumentType,
            typename ArgumentType>
  auto computeScalarFunctionGradientImplementation(
      const CallableType& c,
      const TypeList<CallableArgumentType>&,
      ArgumentType&& arg)  //
    requires(std::is_invocable_v<CallableType, ArgumentType>)
  {
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
  constexpr void checkRedundancy() noexcept
    requires(sizeof...(idx) > 0)
  {
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
  auto computeScalarFunctionGradientImplementation(
      const CallableType& c,
      const TypeList<CallableArgumentType0, CallableArgumentType1>&,
      ArgumentType0&& arg0,
      ArgumentType1&& arg1)  //
    requires((std::is_invocable_v<CallableType,
                                  ArgumentType0,
                                  ArgumentType1>) &&  //
             (sizeof...(idx) > 0) &&                  //
             (sizeof...(idx) <= 2) &&                 //
             ((idx < 2) && ...))
  {
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
      using GradientType0 =
          derivative_type<CallableResultType,
                          std::decay_t<CallableArgumentType0>>;
      using GradientType1 =
          derivative_type<CallableResultType,
                          std::decay_t<CallableArgumentType1>>;
      using ResultType = PackedGradients<GradientType0, GradientType1>;
      if constexpr ((ScalarConcept<std::decay_t<CallableArgumentType0>>) &&  //
                    (ScalarConcept<std::decay_t<CallableArgumentType1>>)) {
        return __enzyme_autodiff<ResultType>(
            wrapper_ptr, enzyme_const,
            c_ptr,  //
            enzyme_out,
            convertToEnzymeArgument<CallableArgumentType0>(arg0),  //
            enzyme_out, convertToEnzymeArgument<CallableArgumentType1>(arg1));
      } else if constexpr (ScalarConcept<std::decay_t<CallableArgumentType0>>) {
        auto r = ResultType{};
        std::get<0>(r) = __enzyme_autodiff<GradientType0>(
            wrapper_ptr, enzyme_const,
            c_ptr,  //
            enzyme_out,
            convertToEnzymeArgument<CallableArgumentType0>(arg0),  //
            enzyme_dup, convertToEnzymeArgument<CallableArgumentType1>(arg1),
            &(get<1>(r)));
        return r;
      } else if constexpr (ScalarConcept<std::decay_t<CallableArgumentType1>>) {
        auto r = ResultType{};
        std::get<1>(r) = __enzyme_autodiff<GradientType1>(
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
  auto computeScalarFunctionGradient(const CallableType& c,
                                     ArgumentsTypes&&... args)  //
    requires(
        (sizeof...(ArgumentsTypes) > 0) &&
        (sizeof...(ArgumentsTypes) < 3) &&  //
        (sizeof...(idx) > 0) && (sizeof...(idx) <= sizeof...(ArgumentsTypes)) &&
        ((idx < sizeof...(ArgumentsTypes)) && ...) &&
        (std::is_invocable_v<CallableType, ArgumentsTypes...>) &&  //
        (ScalarConcept<std::invoke_result_t<CallableType, ArgumentsTypes...>>))
  {
    constexpr auto nargs = sizeof...(ArgumentsTypes);
    if constexpr (nargs == 1) {
      return computeScalarFunctionGradientImplementation(
          c, getArgumentsList<CallableType>(),
          std::forward<ArgumentsTypes>(args)...);
    } else if constexpr (nargs == 2) {
      return computeScalarFunctionGradientImplementation<idx...>(
          c, getArgumentsList<CallableType>(),
          std::forward<ArgumentsTypes>(args)...);
    }
  }  // end of computeScalarFunctionGradient

  template <std::size_t idx,
            EnzymeCallableConcept CallableType,
            typename... CallableArgumentsTypes,
            typename... ArgumentsTypes>
  auto computeMathObjectFunctionGradient(
      const CallableType& c,
      const TypeList<CallableArgumentsTypes...>,
      ArgumentsTypes&&... args)  //
    requires((sizeof...(CallableArgumentsTypes) == sizeof...(ArgumentsTypes)) &&
             (idx < sizeof...(ArgumentsTypes)) &&
             (std::is_invocable_v<CallableType, ArgumentsTypes...>) &&  //
             (MathObjectConcept<
                 std::invoke_result_t<CallableType, ArgumentsTypes...>>))
  {
    using VariableType =
        std::tuple_element_t<idx, std::tuple<CallableArgumentsTypes...>>;
    using CallableResultType =
        std::invoke_result_t<CallableType, CallableArgumentsTypes...>;
    using ResultType =
        derivative_type<CallableResultType, std::decay_t<VariableType>>;
    using size_type = typename ResultType::size_type;
    auto r = ResultType{};
    for (size_type i = 0; i != r.size(0); ++i) {
      auto wrapper = [c, i](CallableArgumentsTypes... wargs) {
        auto result = c(wargs...);
        return result[i];
      };
      const auto row = computeGradient<idx>(wrapper, args...);
      if constexpr (ScalarConcept<decltype(row)>) {
        r[i] = row;
      } else {
        for (size_type j = 0; j != row.size(); ++j) {
          r(i, j) = row[j];
        }
      }
    }
    return r;
  }

  template <std::size_t... idx,
            IsFunctionPointerConcept auto F,
            typename... CallableArgumentsTypes,
            typename... ArgumentsTypes>
  auto computeGradientImplementation(FunctionWrapper<F>,
                                     const TypeList<CallableArgumentsTypes...>&,
                                     ArgumentsTypes&&... args)  //
    requires((sizeof...(CallableArgumentsTypes) == sizeof...(ArgumentsTypes)) &&
             (std::is_invocable_v<decltype(F), CallableArgumentsTypes...>))
  {
    auto c = [](const CallableArgumentsTypes... wargs) { return F(wargs...); };
    return ::tfel::math::enzyme::computeGradient<idx...>(
        c, std::forward<ArgumentsTypes>(args)...);
  }  // end of computeGradientImplementation

  template <std::size_t... idx,
            EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeGradientImplementation(const std::index_sequence<idx...>,
                                     const CallableType& c,
                                     ArgumentsTypes&&... arg)  //
    requires(std::is_invocable_v<CallableType, ArgumentsTypes...>)
  {
    return ::tfel::math::enzyme::computeGradient<idx...>(
        c, std::forward<ArgumentsTypes>(arg)...);
  }  // end of computeGradientImplementation

  template <std::size_t... idx,
            IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeGradientImplementation(const std::index_sequence<idx...>,
                                     FunctionWrapper<F> f,
                                     ArgumentsTypes&&... arg)  //
    requires(std::is_invocable_v<decltype(F), ArgumentsTypes...>)
  {
    return ::tfel::math::enzyme::computeGradient<idx...>(
        f, std::forward<ArgumentsTypes>(arg)...);
  }  // end of computeGradientImplementation

}  // namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <std::size_t... idx,
            internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeGradient(const CallableType& c,
                       ArgumentsTypes&&... args)  //
    requires((sizeof...(ArgumentsTypes) > 0) &&
             (sizeof...(ArgumentsTypes) < 3) &&  //
             (sizeof...(idx) > 0) &&
             (sizeof...(idx) <= sizeof...(ArgumentsTypes)) &&
             ((idx < sizeof...(ArgumentsTypes)) && ...) &&
             (std::is_invocable_v<CallableType, ArgumentsTypes...>) &&  //
             (VariableConcept<
                 std::invoke_result_t<CallableType, ArgumentsTypes...>>))
  {
    using CallableResultType =
        std::invoke_result_t<CallableType, ArgumentsTypes...>;
    if constexpr (ScalarConcept<CallableResultType>) {
      return internals::computeScalarFunctionGradient<idx...>(
          c, std::forward<ArgumentsTypes>(args)...);
    } else {
      static_assert(sizeof...(idx) == 1,
                    "callable returning a non-scalar value can't be "
                    "differentiated with respect to more than a variable");
      return internals::computeMathObjectFunctionGradient<idx...>(
          c, internals::getArgumentsList<CallableType>(),
          std::forward<ArgumentsTypes>(args)...);
    }
  }  // end of computeGradient

  template <std::size_t... idx,
            internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeGradient(internals::FunctionWrapper<F> f,
                       ArgumentsTypes&&... args)  //
    requires(
        (sizeof...(ArgumentsTypes) > 0) &&
        (sizeof...(ArgumentsTypes) < 3) &&  //
        (sizeof...(idx) > 0) && (sizeof...(idx) <= sizeof...(ArgumentsTypes)) &&
        ((idx < sizeof...(ArgumentsTypes)) && ...) &&
        (std::is_invocable_v<decltype(F), ArgumentsTypes...>) &&  //
        (VariableConcept<std::invoke_result_t<decltype(F), ArgumentsTypes...>>))
  {
    return internals::computeGradientImplementation<idx...>(
        f, internals::getArgumentsList<decltype(F)>(),
        std::forward<ArgumentsTypes>(args)...);
  }  // end of computeGradient

  template <internals::EnzymeCallableConcept CallableType,
            typename... ArgumentsTypes>
  auto computeGradient(const CallableType& c,
                       ArgumentsTypes&&... args)                        //
    requires((std::is_invocable_v<CallableType, ArgumentsTypes...>) &&  //
             (VariableConcept<
                 std::invoke_result_t<CallableType, ArgumentsTypes...>>))
  {
    return internals::computeGradientImplementation(
        std::make_index_sequence<sizeof...(ArgumentsTypes)>{}, c,
        std::forward<ArgumentsTypes>(args)...);
  }  // end of computeGradient

  template <internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto computeGradient(internals::FunctionWrapper<F> f,
                       ArgumentsTypes&&... args)  //
    requires(
        (std::is_invocable_v<decltype(F), ArgumentsTypes...>) &&  //
        (VariableConcept<std::invoke_result_t<decltype(F), ArgumentsTypes...>>))
  {
    return internals::computeGradientImplementation(
        std::make_index_sequence<sizeof...(ArgumentsTypes)>{}, f,
        std::forward<ArgumentsTypes>(args)...);
  }  // end of computeGradient

}  // namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_AUTODIFF_IXX */
