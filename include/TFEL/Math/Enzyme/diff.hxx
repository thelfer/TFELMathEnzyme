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
#include "TFEL/Math/Enzyme/Internals/FunctionUtilities.hxx"

namespace tfel::math::enzyme::internals {

  /*!
   * \brief A simple concept to select a suitable callable, i.e. a type which
   * has a call operator but is not a function, nor a function pointer.
   *
   * \note Regular functions are treated separately due to the way Enzyme works:
   * they must be passed as template parameters and not by pointer, otherwise,
   * the symbol associated with the function is not seen by Enzyme.
   */
  template <typename CallableType>
  concept EnzymeCallableConcept =
      (!isFunction<CallableType>()) && (!isFunctionPointer<CallableType>()) &&
      (hasCallOperator<CallableType>());

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  /*!
   * \brief compute the increment of a callable given the values of the variable
   * and its increment.
   * \tparam CallableType: type of the callable
   * \tparam ArgumentType0: type of the argument
   * \param[in] c: callable 
   * \param[in] arg0: argument containing both the value of the variable and its increment
   * \return the callable increment
   */
  template <internals::EnzymeCallableConcept CallableType,
            typename ArgumentType0>
  auto diff(const CallableType&, ArgumentType0&&)
    requires((internals::isVariableValueAndIncrement<ArgumentType0>()) &&
             (internals::getArgumentsSize<CallableType>() == 1u));

  /*!
   * \brief compute the increment of a callable given the values the variables
   * and the increment of one of these variables.
   * \tparam CallableType: type of the callable
   * \tparam ArgumentType0: type of the first argument
   * \tparam ArgumentType1: type of the second argument
   * \param[in] c: callable 
   * \param[in] arg0: first argument
   * \param[in] arg1: second argument
   * \return the callable increment
   * \note One and only one argument must be a VariableValueAndIncrement object.
   */
  template <internals::EnzymeCallableConcept CallableType,
            typename ArgumentType0,
            typename ArgumentType1>
  auto diff(const CallableType&, ArgumentType0&&, ArgumentType1&&)
    requires((internals::getArgumentsSize<CallableType>() == 2u));

  /*!
   * \brief compute the increment of a callable given the values the variables
   * and the increment of one of these variables.
   * \tparam CallableType: type of the callable
   * \tparam ArgumentType0: type of the first argument
   * \tparam ArgumentType1: type of the second argument
   * \tparam ArgumentType2: type of the third argument
   * \param[in] c: callable 
   * \param[in] arg0: first argument
   * \param[in] arg1: second argument
   * \param[in] arg2: third argument
   * \return the callable increment
   * \note One and only one argument must be a VariableValueAndIncrement object.
   */
  template <internals::EnzymeCallableConcept CallableType,
            typename ArgumentType0,
            typename ArgumentType1,
            typename ArgumentType2>
  auto diff(const CallableType&,
            ArgumentType0&&,
            ArgumentType1&&,
            ArgumentType2&&)
    requires((internals::getArgumentsSize<CallableType>() == 3u));

  /*!
   * \brief compute the increment of a callable given the values the variables
   * and the increment of one of these variables.
   * \tparam CallableType: type of the callable
   * \tparam ArgumentType0: type of the first argument
   * \tparam ArgumentType1: type of the second argument
   * \tparam ArgumentType2: type of the third argument
   * \tparam ArgumentType3: type of the fourth argument
   * \param[in] c: callable 
   * \param[in] arg0: first argument
   * \param[in] arg1: second argument
   * \param[in] arg2: third argument
   * \param[in] arg3: fourth argument
   * \return the callable increment
   * \note One and only one argument must be a VariableValueAndIncrement object.
   */
  template <internals::EnzymeCallableConcept CallableType,
            typename ArgumentType0,
            typename ArgumentType1,
            typename ArgumentType2,
            typename ArgumentType3>
  auto diff(const CallableType&,
            ArgumentType0&&,
            ArgumentType1&&,
            ArgumentType2&&,
            ArgumentType3&&)
    requires((internals::getArgumentsSize<CallableType>() == 4u));

  //! \brief an inline variable used to generate function wrappers
  template <internals::IsFunctionPointerConcept auto F>
  constexpr inline auto function = internals::FunctionWrapper<F>{};

  /*!
   * \brief helper function to compute the differential of a regular function.
   *
   * Regular functions must be passed as template parameters and not by pointer,
   * otherwise, the symbol associated with the function is not seen by Enzyme
   */
  template <internals::IsFunctionPointerConcept auto F,
            typename... ArgumentsTypes>
  auto diff(internals::FunctionWrapper<F>, ArgumentsTypes&&...);

}  // end of namespace tfel::math::enzyme

#include "TFEL/Math/Enzyme/diff.ixx"

#endif /* LIB_TFEL_MATH_ENZYME_DIFF_HXX */
