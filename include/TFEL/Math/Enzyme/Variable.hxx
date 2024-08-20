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
   * \brief a concept used to imposed constraints on the variable with respect
   * to which a callable object can be differentiated.
   */
  template <typename VariableType>
  concept VariableConcept = (!internals::isTemporary<VariableType>()) &&
                            ((ScalarConcept<VariableType>) ||
                             (MathObjectConcept<VariableType>));

  /*!
   * \brief a simple structure containing the value of a variable and its
   * increment
   */
  template <typename VariableType>
  struct VariableValueAndIncrement {
    VariableType value;
    VariableType increment;
  };

  /*!
   * \brief an helper function to build an object of type
   * `VariableValueAndIncrement`
   */
  template <typename VariableType, typename ValueType, typename IncrementType>
  VariableValueAndIncrement<VariableType> make_vdv(ValueType&&, IncrementType&&)
    requires((std::is_convertible_v<ValueType, VariableType>) &&
             (std::is_convertible_v<IncrementType, VariableType>));

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
  constexpr auto countNumberOfVariableValueAndIncrement() noexcept {
    return ((isVariableValueAndIncrement<Types>() ? 1 : 0) + ...);
  }  // end of countNumberOfVariableValueAndIncrement

  template <typename SourceType, typename DestinationType>
  struct IsConvertible<VariableValueAndIncrement<SourceType>, DestinationType>
      : std::is_convertible<SourceType, DestinationType> {};

} // end of namespace tfel::math::enzyme::internals 

#include "TFEL/Math/Enzyme/Variable.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_VARIABLE_HXX */
