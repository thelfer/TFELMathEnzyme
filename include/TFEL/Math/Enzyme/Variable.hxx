/*!
 * \file   TFEL/Math/Enzyme/Variable.hxx
 * \brief  This header introduces the VariableConcept concept
 * \author Thomas Helfer
 * \date   08/07/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_VARIABLE_HXX
#define LIB_TFEL_MATH_ENZYME_VARIABLE_HXX

#include <cstddef>
#include <type_traits>
#include "TFEL/Math/General/MathObjectTraits.hxx"
#include "TFEL/Math/Enzyme/Internals/Enzyme.hxx"
#include "TFEL/Math/Enzyme/Internals/IsTemporary.hxx"

namespace tfel::math::enzyme {

  /*!
   * \brief A concept checking that a math object has a size known at
   *   compile time.
   * \tparam MathObjectType: The type to check.
   * \note Derivatives of (or with respect to) math objects whose size is
   *   only known at run time are not supported: the derivative type
   *   (`tfel::math::derivative_type`) is only defined for statically sized
   *   objects.
   */
  template <typename MathObjectType>
  concept FixedSizeConcept = requires {
    std::remove_cvref_t<MathObjectType>::indexing_policy::hasFixedSizes;
  } && (std::remove_cvref_t<MathObjectType>::indexing_policy::hasFixedSizes);

  /*!
   * \brief A concept used to impose constraints on the variable with respect
   *   to which a callable object can be differentiated.
   * \tparam VariableType: The type to check.
   * \note The variable must not be temporary and must be either a scalar or a
   *   statically sized math object.
   */
  template <typename VariableType>
  concept VariableConcept =
      (!internals::isTemporary<VariableType>()) &&
      ((ScalarConcept<VariableType>) ||
       ((MathObjectConcept<VariableType>) && (FixedSizeConcept<VariableType>)));

  /*!
   * \brief A simple structure containing the value of a variable and its
   *   increment for use with Enzyme's automatic differentiation.
   * \tparam VariableType: The type of the variable.
   */
  template <typename VariableType>
  struct VariableValueAndIncrement {
    /*! \brief The value of the variable. */
    VariableType value;
    /*! \brief The increment of the variable. */
    VariableType increment;
  };

  /*!
   * \brief A helper function to build an object of type
   *   `VariableValueAndIncrement`.
   * \tparam VariableType: The type of the variable.
   * \tparam ValueType: The type of the value.
   * \tparam IncrementType: The type of the increment.
   * \param[in] value: The value of the variable.
   * \param[in] increment: The increment of the variable.
   * \return A VariableValueAndIncrement object.
   */
  template <typename VariableType, typename ValueType, typename IncrementType>
  VariableValueAndIncrement<VariableType> make_vdv(ValueType&&, IncrementType&&)
    requires((std::is_convertible_v<ValueType, VariableType>) &&
             (std::is_convertible_v<IncrementType, VariableType>));

}  // end of namespace tfel::math::enzyme

namespace tfel::math::enzyme::internals {

  /*!
   * \brief Returns the number of indices required to address an element of an
   *   object.
   * \tparam T: The type to check.
   * \return 0 for a scalar, 1 for a vector like math object (such as
   *   `tfel::math::stensor` or `tfel::math::tvector`), 2 for a matrix like math
   *   object (such as `tfel::math::tmatrix` or `tfel::math::st2tost2`).
   */
  template <typename T>
  constexpr std::size_t getArity() noexcept {
    using Type = std::remove_cvref_t<T>;
    if constexpr (ScalarConcept<Type>) {
      return 0;
    } else {
      return Type::indexing_policy::arity;
    }
  }  // end of getArity

  template <typename VariableType>
  struct IsVariableValueAndIncrement : std::false_type {};

  template <typename VariableType>
  struct IsVariableValueAndIncrement<
      ::tfel::math::enzyme::VariableValueAndIncrement<VariableType>>
      : std::true_type {};

  template <typename VariableType>
  constexpr bool isVariableValueAndIncrement() noexcept {
    return IsVariableValueAndIncrement<std::decay_t<VariableType>>::value;
  }

  template <typename... Types>
  constexpr auto countNumberOfVariableValueAndIncrement() noexcept
    requires(sizeof...(Types) > 0)
  {
    return ((isVariableValueAndIncrement<Types>() ? 1 : 0) + ...);
  }  // end of countNumberOfVariableValueAndIncrement

}  // end of namespace tfel::math::enzyme::internals

#include "TFEL/Math/Enzyme/Variable.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_VARIABLE_HXX */
