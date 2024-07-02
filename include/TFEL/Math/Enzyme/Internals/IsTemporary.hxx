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

#include <type_traits>
#include "TFEL/Math/Forward/qt.hxx"
#include "TFEL/Math/Forward/Expr.hxx"
#include "TFEL/Math/Array/View.hxx"

namespace tfel::math::enzyme::internals {

  template <typename T>
  struct IsTemporary : std::false_type {};

  template <typename ResultType, typename Operation>
  struct IsTemporary<tfel::math::Expr<ResultType, Operation>>
      : std::true_type {};

  template <typename MappedType, typename IndexingPolicyType>
  struct IsTemporary<tfel::math::View<MappedType, IndexingPolicyType>>
      : std::true_type {};

  template <typename UnitType, typename ValueType>
  struct IsTemporary<qt_ref<UnitType, ValueType>>
      : std::true_type {};

  template <typename UnitType, typename ValueType>
  struct IsTemporary<const_qt_ref<UnitType, ValueType>>
      : std::true_type {};

  template <typename T>
  constexpr bool isTemporary() noexcept {
    return IsTemporary<std::decay_t<T>>::value;
  }

} // end of namespace tfel::math::enzyme::internals
