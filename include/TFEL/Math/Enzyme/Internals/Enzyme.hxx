/*!
 * \file   TFEL/Math/Enzyme/Internals/Enzyme.hxx
 * \brief  This header declares some functions and variables
 *         required by enzyme.
 * \author Thomas Helfer
 * \date   08/07/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_INTERNALS_ENZYME_HXX
#define LIB_TFEL_MATH_ENZYME_INTERNALS_ENZYME_HXX

#include <utility>
#include <type_traits>
#include <enzyme/enzyme>
#include "TFEL/Math/Enzyme/Internals/TypeList.hxx"

namespace tfel::math::enzyme::internals {

  template <typename T>
  inline constexpr bool isEnzymeType = false;

  template <typename T>
  inline constexpr bool isEnzymeType<::enzyme::Const<T>> = true;

  template <typename T>
  inline constexpr bool isEnzymeType<::enzyme::Duplicated<T>> = true;

  template <typename T>
  struct EnzymeValueType;

  template <typename T>
  struct EnzymeValueType<::enzyme::Const<T>> {
    using type = T;
  };

  template <typename T>
  struct EnzymeValueType<::enzyme::Duplicated<T>> {
    using type = T;
  };

  /*!
   * \brief Wrapper for callable objects to be used with Enzyme.
   * \tparam F: The callable type.
   * \tparam L: The type list of arguments.
   */
  template <typename F, typename L>
  inline constexpr auto callable_wrapper = nullptr;

  /*!
   * \brief Specialization of callable_wrapper for TypeList.
   * \tparam F: The callable type.
   * \tparam Args: The argument types.
   * \param[in] ptr: Pointer to the callable object.
   * \param[in] args: The arguments to pass to the callable.
   * \return The result of calling the callable with the arguments.
   */
  template <typename F, typename... Args>
  inline constexpr auto callable_wrapper<F, TypeList<Args...>> =
      [](std::conditional_t<std::is_const_v<F>,  //
                            const void* const,
                            void* const> ptr,
         const Args... args) { return (*reinterpret_cast<F*>(ptr))(args...); };

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  /*!
   * \brief Enumeration of differentiation modes.
   */
  enum struct Mode { FORWARD, REVERSE };

}  // end of namespace tfel::math::enzyme

namespace tfel::math::enzyme::internals {

  /*!
   * \brief Check that the indices of the variables with respect to which
   *   derivatives are computed are consistent with the differentiation mode.
   * \tparam m: The differentiation mode.
   * \tparam idx: The indices of the variables with respect to which
   *   derivatives are computed.
   * \return true if the indices are valid for this mode.
   * \note In forward mode, a single derivative with respect to the first
   *   argument is computed: the only valid index is 0 and it cannot be
   *   repeated. Higher order derivatives require the getter functions, such as
   *   `tfel::math::enzyme::getDerivativeFunction<Mode::FORWARD, 0, 0>`.
   */
  template <::tfel::math::enzyme::Mode m, std::size_t... idx>
  constexpr bool checkIndices() noexcept {
    if constexpr (m == ::tfel::math::enzyme::Mode::REVERSE) {
      return true;
    } else {
      return (sizeof...(idx) == 1) && ((idx == 0) && ...);
    }
  }  // end of checkIndices

}  // end of namespace tfel::math::enzyme::internals

#endif /* LIB_TFEL_MATH_ENZYME_INTERNALS_ENZYME_HXX */
