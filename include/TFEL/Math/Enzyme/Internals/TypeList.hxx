/*!
 * \file   TFEL/Math/Enzyme/Internals/TypeList.hxx
 * \brief
 * \author Thomas Helfer
 * \date   09/08/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_INTERNALS_TYPELIST_HXX
#define LIB_TFEL_MATH_ENZYME_INTERNALS_TYPELIST_HXX

namespace tfel::math::enzyme::internals {

  template <typename... Types>
  struct TypeList {};

  template <typename... Types>
  constexpr std::size_t getTypeListSize(const TypeList<Types...>) {
    return sizeof...(Types);
  }

}  // end of namespace tfel::math::enzyme::internals

#endif /* LIB_TFEL_MATH_ENZYME_INTERNALS_TYPELIST_HXX */
