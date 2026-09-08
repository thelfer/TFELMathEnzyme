/*!
 * \file   TFEL/Math/Enzyme/getReverseModeDerivativeFunction.ixx
 * \brief  This file implements the getReverseModeDerivativeFunction function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_GETREVERSEMODEDERIVATIVEFUNCTION_IXX
#define LIB_TFEL_MATH_ENZYME_GETREVERSEMODEDERIVATIVEFUNCTION_IXX

namespace tfel::math::enzyme::internals {

  /*!
   * \brief Implementation of getReverseModeDerivativeFunction
   * \tparam N: current index
   * \tparam Ns: remaining indices
   * \tparam CallableType: the callable type
   * \tparam CallableArgumentsTypes: the argument types
   * \param[in] c: the callable
   * \param[in] args_list: type list of callable arguments (unused, for SFINAE)
   * \return a callable that computes the reverse mode derivative
   */
  template <std::size_t N,
            std::size_t... Ns,
            internals::EnzymeCallableConcept CallableType,
            typename... CallableArgumentsTypes>
  auto getReverseModeDerivativeFunctionImplementation(
      const CallableType& c,
      const TypeList<CallableArgumentsTypes...> args_list) {
    auto dc = [c](CallableArgumentsTypes... wargs) {
      return ::tfel::math::enzyme::computeReverseModeDerivative<N>(c, wargs...);
    };
    if constexpr (sizeof...(Ns) == 0) {
      return dc;
    } else {
      return getReverseModeDerivativeFunctionImplementation<Ns...>(dc,
                                                                   args_list);
    }
  }  // end of getReverseModeDerivativeFunctionImplementation

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  /*!
   * \brief Get a function that computes the reverse mode derivative
   * \tparam Ns: indices specifying which arguments to differentiate with
   * respect to \tparam CallableType: the callable type \param[in] c: the
   * callable \return a callable that computes the reverse mode derivative
   */
  template <std::size_t... Ns, internals::EnzymeCallableConcept CallableType>
  auto getReverseModeDerivativeFunction(const CallableType& c) requires(
      sizeof...(Ns) > 0) {
    return internals::getReverseModeDerivativeFunctionImplementation<Ns...>(
        c, internals::getArgumentsList<CallableType>());
  }  // end of getReverseModeDerivativeFunction

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_GETREVERSEMODEDERIVATIVEFUNCTION_IXX */
