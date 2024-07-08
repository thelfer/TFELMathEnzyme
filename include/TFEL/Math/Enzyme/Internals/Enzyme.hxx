/*!
 * \file   TFEL/Math/Enzyme/Internals/Enzyme.hxx
 * \brief  This header declares some functions and variables
 *         required by enzyme.
 * \author Thomas Helfer
 * \date   08/07/2024
 */

#ifndef LIB_TFEL_MATH_ENZYME_INTERNALS_ENZYME_HXX
#define LIB_TFEL_MATH_ENZYME_INTERNALS_ENZYME_HXX

/*!
 * \brief specifier used to introduce duplicated variables
 * \note see the enzyme documentation for details
 */
extern int enzyme_dup;
/*!
 * \brief specifier used to introduce a variable with respect to which a
 * callable is differentiated
 */
extern int enzyme_out;
//! \brief specifier used to introduce a standard variables
extern int enzyme_const;

/*!
 * \brief Enzyme'entry point for forward differentiation
 * \tparam ResultType: type of the result of the callable pointed by ptr
 * \tparam ArgumentsTypes: types of the arguments of the callable pointed by ptr
 * \param[in] ptr:
 */
template <typename ResultType, typename... ArgumentsTypes>
ResultType __enzyme_fwddiff(void*, ArgumentsTypes...);

#endif /* LIB_TFEL_MATH_ENZYME_INTERNALS_ENZYME_HXX */
