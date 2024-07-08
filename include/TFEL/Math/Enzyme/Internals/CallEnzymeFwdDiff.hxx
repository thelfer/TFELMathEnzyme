/*!
 * \file   TFEL/Math/Enzyme/Internals/CallEnzymeFwdDiff.hxx
 * \brief  This header declares the callEnzymeForwardDifferentiate function
 * \author Thomas Helfer
 * \date   08/07/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_CALLENZYMEFWDDIFF_HXX
#define LIB_TFEL_MATH_ENZYME_CALLENZYMEFWDDIFF_HXX

#include "TFEL/Math/Enzyme/Variable.hxx"
#include "TFEL/Math/Enzyme/Internals/Enzyme.hxx"

namespace tfel::math::enzyme::internals {

  /*!
   * \brief a small wrapper around __enzyme_ffwddiff
   * \tparam N: index of the variable with respect to which the callable is
   * differentiated
   * \param[in] ptr: function pointer
   * \param[in] args: arguments
   */
  template <std::size_t N,
            typename ResultType,
            VariableConcept... ArgumentsTypes>
  [[nodiscard]] ResultType callEnzymeForwardDifferentiate(
      void* const,
      ArgumentsTypes&&...) noexcept  //
      requires((sizeof...(ArgumentsTypes) != 0) &&
               (N < sizeof...(ArgumentsTypes)));

}  // namespace tfel::math::enzyme::internals

#include "TFEL/Math/Enzyme/Internals/CallEnzymeFwdDiff.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_CALLENZYMEFWDDIFF_HXX */
