/*!
 * \file   TFEL/Math/Enzyme/Internals/CallEnzymeFwdDiff.ixx
 * \brief  This header implements the callEnzymeForwardDifferentiate function
 * \author Thomas Helfer
 * \date   08/07/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_CALLENZYMEFWDDIFF_IXX
#define LIB_TFEL_MATH_ENZYME_CALLENZYMEFWDDIFF_IXX

#include <tuple>

namespace tfel::math::enzyme::internals {

  namespace details {

    template <std::size_t I,
              std::size_t N,
              typename ResultType,
              typename... ArgumentsTypes>
    [[nodiscard]] ResultType callEnzymeForwardDifferentiateImplementation(
        std::tuple<ArgumentsTypes...>&& pargs) noexcept {
      return std::apply(__enzyme_fwddiff<ResultType, ArgumentsTypes...>,
                        std::forward<std::tuple<ArgumentsTypes...>>(pargs));
    }

    template <std::size_t I,
              std::size_t N,
              typename ResultType,
              typename PackedArguments,
              typename ArgumentType,
              VariableConcept... ArgumentsTypes>
    [[nodiscard]] ResultType callEnzymeForwardDifferentiateImplementation(
        PackedArguments&& pargs,
        ArgumentType&& arg,
        ArgumentsTypes&&... args) noexcept {
      const auto enzyme_specifier = [] {
        if constexpr (I == N) {
          return enzyme_out;
        }
        return enzyme_const;
      }();
      callEnzymeForwardDifferentiateImplementation<I + 1, N, ResultType>(
          std::tuple_cat(pargs, std::forward_as_tuple(enzyme_specifier, arg)),
          std::forward<ArgumentsTypes>(args)...);
    }

  }  // namespace details

  template <std::size_t N,
            typename ResultType,
            VariableConcept... ArgumentsTypes>
  [[nodiscard]] ResultType callEnzymeForwardDifferentiate(
      void* const ptr,
      ArgumentsTypes&&... args) noexcept  //
      requires((sizeof...(ArgumentsTypes) != 0) &&
               (N < sizeof...(ArgumentsTypes))) {
    return details::callEnzymeForwardDifferentiateImplementation<0, N,
                                                                 ResultType>(
        std::make_tuple(ptr), std::forward<ArgumentsTypes>(args)...);
  }  // end of callEnzymeForwardDifferentiate

}  // namespace tfel::math::enzyme::internals

#endif /* LIB_TFEL_MATH_ENZYME_CALLENZYMEFWDDIFF_IXX */
