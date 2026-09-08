/*!
 * \file   TFEL/Math/Enzyme/Variable.ixx
 * \brief  This header implements the make_vdv function
 * \author Thomas Helfer
 * \date   08/07/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_VARIABLE_IXX
#define LIB_TFEL_MATH_ENZYME_VARIABLE_IXX

namespace tfel::math::enzyme {

  /*!
   * \brief Create a VariableValueAndIncrement from value and increment
   * \tparam VariableType: the type of the variable
   * \tparam ValueType: the type of the value
   * \tparam IncrementType: the type of the increment
   * \param[in] value: the value of the variable
   * \param[in] increment: the increment of the variable
   * \return a VariableValueAndIncrement object
   */
  template <typename VariableType, typename ValueType, typename IncrementType>
  VariableValueAndIncrement<VariableType>
  make_vdv(ValueType&& value, IncrementType&& increment) requires(
      (std::is_convertible_v<ValueType, VariableType>)&&(
          std::is_convertible_v<IncrementType, VariableType>)) {
    return {.value = static_cast<VariableType>(value),
            .increment = static_cast<VariableType>(increment)};
  }  // end of make_vdv

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_VARIABLE_IXX */
