/*!
 * \file   TFEL/Math/Enzyme/GetPartialDerivative.ixx
 * \brief  This hearder implements the getPartialDerivative functions
 * \author Thomas Helfer
 * \date   08/07/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_GETPARTIALDERIVATIVE_IXX
#define LIB_TFEL_MATH_ENZYME_GETPARTIALDERIVATIVE_IXX

namespace tfel::math::enzyme{

  template <std::size_t N,
            VariableConcept... VariablesTypes,
            typename CallableType>
  [[nodiscard]] auto
  getPartialDerivative(const CallableType& c) noexcept requires(
      (std::is_invocable_v<CallableType, VariablesTypes...>)&&  //
      (N < sizeof...(VariablesTypes))) {
    return [c](const VariablesTypes&... variables) {
      using ReturnedType =
          std::invoke_result_t<CallableType, VariablesTypes...>;
      auto wrapper = [](CallableType* const p, const VariablesTypes&... v) {
        return (*p)(v...);
      };
      void* const wrapper_ptr = reinterpret_cast<void*>(+wrapper);
      return internals::callEnzymeForwardDifferentiate<1 + N, ReturnedType>(
          wrapper_ptr, &c, variables...);
    };
  }  // end of getPartialDerivative

  template <VariableConcept VariableType, typename CallableType>
  [[nodiscard]] auto getDerivative(const CallableType& c) noexcept requires(
      std::is_invocable_v<CallableType, VariableType>) {
    return getPartialDerivative<0, VariableType>(c);
  }  // end of getPartialDerivative

} // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_GETPARTIALDERIVATIVE_IXX */
