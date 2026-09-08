/*!
 * \file   TFEL/Math/Enzyme/Internals/TypeList.hxx
 * \brief  This file declares the TypeList utility for managing type lists
 * \author Thomas Helfer
 * \date   09/08/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_INTERNALS_TYPELIST_HXX
#define LIB_TFEL_MATH_ENZYME_INTERNALS_TYPELIST_HXX

namespace tfel::math::enzyme::internals {

  /*!
   * \brief A simple type list container for template metaprogramming
   * \tparam Types: the types to store in the list
   */
  template <typename... Types>
  struct TypeList {};

  /*!
   * \brief Get the size of a TypeList
   * \tparam Types: the types in the list
   * \param[out] list: the TypeList instance
   * \return the number of types in the list
   */
  template <typename... Types>
  constexpr std::size_t getTypeListSize(const TypeList<Types...>) {
    return sizeof...(Types);
  }

}  // end of namespace tfel::math::enzyme::internals

#endif /* LIB_TFEL_MATH_ENZYME_INTERNALS_TYPELIST_HXX */
