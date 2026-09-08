/*!
 * \file   TFEL/Math/Enzyme/getForwardModeDerivativeFunction.ixx
 * \brief  This file implements the computeForwardModeDerivative function
 * \author Thomas Helfer
 * \date   09/08/2024
 * \copyright Copyright (C) 2006-2024 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#ifndef LIB_TFEL_MATH_ENZYME_GETFORWARDMODEDERIVATIVEFUNCTION_IXX
#define LIB_TFEL_MATH_ENZYME_GETFORWARDMODEDERIVATIVEFUNCTION_IXX

namespace tfel::math::enzyme::internals {

  /*!
   * \brief Implementation of getForwardModeDerivativeFunction
   * \tparam N: current index (must be 0)
   * \tparam Ns: remaining indices
   * \tparam CallableType: the callable type
   * \tparam CallableArgumentType0: the argument type
   * \param[in] c: the callable
   * \param[in] args_list: type list of callable arguments (unused, for SFINAE)
   * \return a callable that computes the forward mode derivative
   */
  template <std::size_t N,
            std::size_t... Ns,
            internals::EnzymeCallableConcept CallableType,
            typename CallableArgumentType0>
  auto getForwardModeDerivativeFunctionImplementation(
      const CallableType& c,
      const TypeList<CallableArgumentType0> args_list) requires(N == 0) {
    auto dc = [c](CallableArgumentType0 warg) {
      return ::tfel::math::enzyme::computeForwardModeDerivative(c, warg);
    };
    if constexpr (sizeof...(Ns) == 0) {
      return dc;
    } else {
      return getForwardModeDerivativeFunctionImplementation<Ns...>(dc,
                                                                   args_list);
    }
  }  // end of getForwardModeDerivativeFunctionImplementation

}  // end of namespace tfel::math::enzyme::internals

namespace tfel::math::enzyme {

  /*!
   * \brief Get a function that computes the forward mode derivative
   * \tparam Ns: indices specifying which arguments to differentiate with
   * respect to \tparam CallableType: the callable type \param[in] c: the
   * callable \return a callable that computes the forward mode derivative
   */
  template <std::size_t... Ns, internals::EnzymeCallableConcept CallableType>
  auto getForwardModeDerivativeFunction(const CallableType& c) requires(
      sizeof...(Ns) > 0) {
    return internals::getForwardModeDerivativeFunctionImplementation<Ns...>(
        c, internals::getArgumentsList<CallableType>());
  }  // end of getForwardModeDerivativeFunction

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_GETFORWARDMODEDERIVATIVEFUNCTION_IXX */
