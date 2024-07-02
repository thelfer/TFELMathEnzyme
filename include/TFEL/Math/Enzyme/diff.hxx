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

#include <cstddef>
#include <type_traits>
#include "TFEL/Math/General/MathObjectTraits.hxx"
#include "TFEL/Math/Enzyme/Internals/IsTemporary.hxx"

namespace tfel::math::enzyme {

  /*!
   * \brief a concept used to imposed constraints on the variable with respect
   * to which a callable object can be differentiated.
   */
  template <typename VariableType>
  concept DiffVariableTypeConcept =
      (!internals::isTemporary<VariableType>()) &&
      ((ScalarConcept<VariableType>) || (MathObjectConcept<VariableType>));

  /*!
   * \returns a callable object computing the derivative of the callable object
   * in argument
   * \tparam CallableType: type of the callable
   * \tparam VariableType: type of the variable with respect to which the
   * callable is derivated
   * \param[in] c: callable object
   */
  template <DiffVariableTypeConcept VariableType, typename CallableType>
  auto getCallableDerivative(const CallableType&)
    requires(std::is_invocable_v<CallableType, const VariableType>);

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
            typename CallableType,
            DiffVariableTypeConcept VariableType>
  TFEL_HOST_DEVICE auto diff(const CallableType&, const VariableType&)  //
    requires((std::is_invocable_v<CallableType, const VariableType>) &&
             (N >= 1));

}  // end of namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/diff.ixx"