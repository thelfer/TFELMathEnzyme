/*!
 * \file   TFEL/Math/Enzyme/diff.hxx
 * \brief  This file declares the diff function
 * \author Thomas Helfer
 * \date   02/07/2025
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_DIFF_HXX
#define LIB_TFEL_MATH_ENZYME_DIFF_HXX 1

#include <cstddef>
#include "TFEL/Math/Enzyme/Variable.hxx"

namespace tfel::math::enzyme {

  /*!
   * \brief evaluate the Nth derivative of a callable for the given value of the
   * variable
   * \tparam N: derivative order
   * \tparam CallableType: type of the callable to be differentiated
   * \tparam VariableType: type of the variable
   * \param[in] c: callable object
   * \param[in] x: variable value
   */
  template <std::size_t N = 1,
            VariableConcept VariableType,
            std::invocable<VariableType> CallableType>
  TFEL_HOST_DEVICE auto diff(const CallableType&, const VariableType&)  //
      requires(N >= 1);

}  // end of namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/diff.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_DIFF_HXX */
