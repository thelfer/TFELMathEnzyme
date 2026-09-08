/*!
 * \file   TFEL/Math/Enzyme/Variable.hxx
 * \brief  This header introduces the VariableConcept concept
 * \author Thomas Helfer
 * \date   08/07/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_VARIABLE_HXX
#define LIB_TFEL_MATH_ENZYME_VARIABLE_HXX

#include "TFEL/Math/General/MathObjectTraits.hxx"
#include "TFEL/Math/Enzyme/Internals/Enzyme.hxx"
#include "TFEL/Math/Enzyme/Internals/IsTemporary.hxx"

namespace tfel::math::enzyme {

  /*!
   * \brief A concept used to impose constraints on the variable with respect
   * to which a callable object can be differentiated.
   * \tparam VariableType: the type to check
   * \note The variable must not be temporary and must be either a scalar or a
   * math object
   */
  template <typename VariableType>
  concept VariableConcept = (!internals::isTemporary<VariableType>()) &&
                            ((ScalarConcept<VariableType>) ||
                             (MathObjectConcept<VariableType>));

  /*!
   * \brief A simple structure containing the value of a variable and its
   * increment for use with Enzyme's automatic differentiation
   * \tparam VariableType: the type of the variable
   */
  template <typename VariableType>
  struct VariableValueAndIncrement {
    /*!\brief The value of the variable */
    VariableType value;
    /*!\brief The increment of the variable */
    VariableType increment;
  };

  /*!
   * \brief A helper function to build an object of type
   * `VariableValueAndIncrement` \tparam VariableType: the type of the variable
   * \tparam ValueType: the type of the value
   * \tparam IncrementType: the type of the increment
   * \param[in] value: the value of the variable
   * \param[in] increment: the increment of the variable
   * \return a VariableValueAndIncrement object
   */
  template <typename VariableType, typename ValueType, typename IncrementType>
  VariableValueAndIncrement<VariableType>
  make_vdv(ValueType&&, IncrementType&&) requires(
      (std::is_convertible_v<ValueType, VariableType>)&&(
          std::is_convertible_v<IncrementType, VariableType>));

}  // end of namespace tfel::math::enzyme

namespace tfel::math::enzyme::internals {

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
      requires(sizeof...(Types) > 0) {
    return ((isVariableValueAndIncrement<Types>() ? 1 : 0) + ...);
  }  // end of countNumberOfVariableValueAndIncrement

  template <typename SourceType, typename DestinationType>
  struct IsConvertible<VariableValueAndIncrement<SourceType>, DestinationType>
      : std::is_convertible<SourceType, DestinationType> {};

}  // end of namespace tfel::math::enzyme::internals

#include "TFEL/Math/Enzyme/Variable.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_VARIABLE_HXX */
