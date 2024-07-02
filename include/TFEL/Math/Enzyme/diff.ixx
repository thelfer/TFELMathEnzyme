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

extern int enzyme_dup;
extern int enzyme_out;
extern int enzyme_const;

template < typename return_type, typename ... T >
return_type __enzyme_fwddiff(void*, T ... );

namespace tfel::math::enzyme::internals {

  template <typename CallableType, ScalarConcept ScalarVariableType>
  TFEL_HOST_DEVICE auto diff_wrt_scalar_variable(const CallableType &c,
                                                 const ScalarVariableType x)  //
    requires(std::is_invocable_v<CallableType, const ScalarVariableType>)
  {
    using result_type =
        std::invoke_result_t<CallableType, const ScalarVariableType>;
    auto wrapper = [](CallableType *c2, const ScalarVariableType x2) {
      return (*c2)(x2);
    };
    void *const wrapper_ptr = reinterpret_cast<void *>(+wrapper);
    void *const c_ptr =
        reinterpret_cast<void *>(const_cast<CallableType *>(&c));
    auto dx = ScalarVariableType{1};
    return __enzyme_fwddiff<result_type>(wrapper_ptr, enzyme_const, c_ptr,
                                         enzyme_dup, x, dx);
  }

  template <typename CallableType, MathObjectConcept VariableType>
  TFEL_HOST_DEVICE auto diff_wrt_math_object_variable(const CallableType &c,
                                                      const VariableType x)  //
    requires(std::is_invocable_v<CallableType, const VariableType>)
  {
    using result_type = std::invoke_result_t<CallableType, const VariableType>;
    auto wrapper = [](CallableType *c2, const VariableType x2) {
      return (*c2)(x2);
    };
    void *const wrapper_ptr = reinterpret_cast<void *>(+wrapper);
    void *const c_ptr =
        reinterpret_cast<void *>(const_cast<CallableType *>(&c));
    auto r = derivative_type<result_type, VariableType>{};
    for (typename VariableType::size_type i = 0; i != r.size(0); ++i) {
      auto dx = VariableType{0};
      dx[i] = 1;
      if constexpr (isScalar<result_type>()) {
        r[i] = __enzyme_fwddiff<result_type>(wrapper_ptr, enzyme_const, c_ptr,
                                             enzyme_dup, x, dx);
      } else {
        const auto row = __enzyme_fwddiff<result_type>(
            wrapper_ptr, enzyme_const, c_ptr, enzyme_dup, x, dx);
        for (typename VariableType::size_type j = 0; j != r.size(1); ++j) {
          r(i, j) = row[j];
        }
      }
    }
    return r;
  }

  template <typename CallableType, typename VariableType>
  TFEL_HOST_DEVICE auto diff(const CallableType &c, const VariableType x)  //
    requires(((ScalarConcept<VariableType>) ||
              (MathObjectConcept<VariableType>)) &&
             (std::is_invocable_v<CallableType, const VariableType>))
  {
    if constexpr (ScalarConcept<VariableType>) {
      return diff_wrt_scalar_variable(c, x);
    } else {
      return diff_wrt_math_object_variable(c, x);
    }
  }  // end of diff

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  template <typename VariableType>
  auto getCallableDerivative(const auto &);

  template <std::size_t N, typename CallableType, typename VariableType>
  TFEL_HOST_DEVICE auto diff(const CallableType &c, const VariableType& x)  //
    requires(((ScalarConcept<VariableType>) ||
              (MathObjectConcept<VariableType>)) &&
             (std::is_invocable_v<CallableType, const VariableType>) &&
             (N >= 1))
  {
    if constexpr (N == 1) {
      return internals::diff(c, x);
    } else {
      //       const auto w = [&c](const VariableType v) {
      //         return diff<1u, CallableType, VariableType>(c, v);
      //       };
      const auto w = getCallableDerivative<VariableType>(c);
      return diff<N - 1, decltype(w), VariableType>(w, x);
    }
  }  // end of diff

  template <typename VariableType>
  auto getCallableDerivative(const auto &c) {
    return [&c](const VariableType &x) { return diff<1u>(c, x); };
  }

}  // end of namespace tfel::math::enzyme
