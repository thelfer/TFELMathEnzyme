/*!
 * \file   TFEL/Math/Enzyme/Variable.hxx
 * \brief  This header introduces the VariableConcept concept
 * \author Thomas Helfer
 * \date   08/07/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_VARIABLE_HXX
#define LIB_TFEL_MATH_ENZYME_VARIABLE_HXX

#include "TFEL/Math/General/MathObjectTraits.hxx"
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

} // end of tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_VARIABLE_HXX */
