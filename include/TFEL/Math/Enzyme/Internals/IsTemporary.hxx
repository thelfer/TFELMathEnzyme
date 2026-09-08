/*!
 * \file   TFEL/Math/Enzyme/Internals/IsTemporary.hxx
 * \brief  This file declares the IsTemporary traits
 * \author Thomas Helfer
 * \date   02/07/2025
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_INTERNALS_ISTEMPORARY_LIB
#define LIB_TFEL_MATH_ENZYME_INTERNALS_ISTEMPORARY_LIB 1

#include <type_traits>
#include "TFEL/Math/Forward/qt.hxx"
#include "TFEL/Math/Forward/Expr.hxx"
#include "TFEL/Math/Array/View.hxx"

namespace tfel::math::enzyme::internals {

  /*!
   * \brief Primary template for checking if a type is temporary
   * \tparam T: the type to check
   */
  template <typename T>
  struct IsTemporary : std::false_type {};

  /*!
   * \brief Specialization for TFEL expression types
   * \tparam ResultType: the result type of the expression
   * \tparam Operation: the operation type
   */
  template <typename ResultType, typename Operation>
  struct IsTemporary<tfel::math::Expr<ResultType, Operation>> : std::true_type {
  };

  /*!
   * \brief Specialization for TFEL View types
   * \tparam MappedType: the mapped type
   * \tparam IndexingPolicyType: the indexing policy type
   */
  template <typename MappedType, typename IndexingPolicyType>
  struct IsTemporary<tfel::math::View<MappedType, IndexingPolicyType>>
      : std::true_type {};

  /*!
   * \brief Specialization for qt_ref types
   * \tparam UnitType: the unit type
   * \tparam ValueType: the value type
   */
  template <typename UnitType, typename ValueType>
  struct IsTemporary<qt_ref<UnitType, ValueType>> : std::true_type {};

  /*!
   * \brief Specialization for const_qt_ref types
   * \tparam UnitType: the unit type
   * \tparam ValueType: the value type
   */
  template <typename UnitType, typename ValueType>
  struct IsTemporary<const_qt_ref<UnitType, ValueType>> : std::true_type {};

  /*!
   * \brief Check if a type is temporary
   * \tparam T: the type to check
   * \return true if T is a temporary type
   */
  template <typename T>
  constexpr bool isTemporary() noexcept {
    return IsTemporary<std::decay_t<T>>::value;
  }

}  // end of namespace tfel::math::enzyme::internals

#endif /* LIB_TFEL_MATH_ENZYME_INTERNALS_ISTEMPORARY_LIB */