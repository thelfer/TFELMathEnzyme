/*!
 * \file   TFEL/Math/Enzyme/diff.ixx
 * \brief  This file implements the diff function
 * \author Thomas Helfer
 * \date   02/07/2025
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_DIFF_IXX
#define LIB_TFEL_MATH_ENZYME_DIFF_IXX 1

#include "TFEL/Math/General/DerivativeType.hxx"
#include "TFEL/Math/Enzyme/GetPartialDerivative.hxx"
#include "TFEL/Math/Enzyme/Internals/Enzyme.hxx"


namespace tfel::math::enzyme::internals {

  template <ScalarConcept ScalarVariableType,
            std::invocable<ScalarVariableType> CallableType>
  TFEL_HOST_DEVICE auto diff_wrt_scalar_variable(const CallableType &c,
                                                 const ScalarVariableType x) {
    using callable_returned_type =
        std::invoke_result_t<CallableType, const ScalarVariableType>;
    using diff_returned_type =
        derivative_type<callable_returned_type, ScalarVariableType>;
    using variable_base_type = base_type<ScalarVariableType>;
    auto wrapper = [](CallableType *c2, const variable_base_type x2) {
      if constexpr (isQuantity<callable_returned_type>()){
        const auto r = (*c2)(ScalarVariableType{x2});
        return r.getValue();
      } else {
        return (*c2)(ScalarVariableType{x2});
      }
    };
    void *const wrapper_ptr = reinterpret_cast<void *>(+wrapper);
    void *const c_ptr =
        reinterpret_cast<void *>(const_cast<CallableType *>(&c));
    auto xv = base_type_cast(x);
    auto dx = variable_base_type{1};
    return diff_returned_type{
        __enzyme_fwddiff<base_type<callable_returned_type>>(
            wrapper_ptr, enzyme_const, c_ptr, enzyme_dup, xv, dx)};
  }

  template <MathObjectConcept VariableType,
            std::invocable<VariableType> CallableType>
  TFEL_HOST_DEVICE auto diff_wrt_math_object_variable(const CallableType &c,
                                                      const VariableType x) {
    using returned_type = std::invoke_result_t<CallableType, const VariableType>;
    auto wrapper = [](CallableType *c2, const VariableType x2) {
      return (*c2)(x2);
    };
    void *const wrapper_ptr = reinterpret_cast<void *>(+wrapper);
    void *const c_ptr =
        reinterpret_cast<void *>(const_cast<CallableType *>(&c));
    auto r = derivative_type<returned_type, VariableType>{};
    for (typename VariableType::size_type i = 0; i != r.size(0); ++i) {
      auto dx = VariableType{0};
      dx[i] = 1;
      if constexpr (isScalar<returned_type>()) {
        r[i] = __enzyme_fwddiff<returned_type>(wrapper_ptr, enzyme_const, c_ptr,
                                             enzyme_dup, x, dx);
      } else {
        const auto row = __enzyme_fwddiff<returned_type>(
            wrapper_ptr, enzyme_const, c_ptr, enzyme_dup, x, dx);
        for (typename VariableType::size_type j = 0; j != r.size(1); ++j) {
          r(i, j) = row[j];
        }
      }
    }
    return r;
  }

  template <typename VariableType, std::invocable<VariableType> CallableType>
  TFEL_HOST_DEVICE auto diff(const CallableType &c, const VariableType x)  //
    requires((ScalarConcept<VariableType>) || (MathObjectConcept<VariableType>))
  {
    if constexpr (ScalarConcept<VariableType>) {
      return diff_wrt_scalar_variable(c, x);
    } else {
      return diff_wrt_math_object_variable(c, x);
    }
  }  // end of diff

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <std::size_t N,
            VariableConcept VariableType,
            std::invocable<VariableType> CallableType>
  TFEL_HOST_DEVICE auto diff(const CallableType &c, const VariableType &x)  //
    requires(N >= 1)
  {
    using returned_type = std::invoke_result_t<CallableType, const VariableType>;
    static_assert(!internals::isTemporary<returned_type>(),
                  "temporary types (views, expressions) are not allowed");
    if constexpr (N == 1) {
      return internals::diff(c, x);
    } else {
      //       const auto w = [&c](const VariableType v) {
      //         return diff<1u, CallableType, VariableType>(c, v);
      //       };
      const auto w = getDerivative<VariableType>(c);
      return diff<N - 1, decltype(w), VariableType>(w, x);
    }
  }  // end of diff

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_DIFF_IXX */
