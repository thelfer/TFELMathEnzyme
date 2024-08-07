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

#include <tuple>
#include <cstddef>
#include <utility>
#include <type_traits>
#include <iostream>
#include "TFEL/Math/Enzyme/Variable.hxx"
#include "TFEL/Math/Enzyme/Internals/Enzyme.hxx"

namespace tfel::math::enzyme {

  template <typename ResultType, VariableConcept... VariablesTypes>
  using function_ptr = ResultType (*)(VariablesTypes...);

  template <typename T>
  struct is_function_pointer : std::false_type {};

  template <typename ResultType, typename... VariablesTypes>
  struct is_function_pointer<ResultType (*)(VariablesTypes...)>
      : std::true_type {};

  template <std::size_t N,
            typename ResultType,
            VariableConcept... VariablesTypes,
            typename... ArgumentsTypes>
  void diff(function_ptr<ResultType, VariablesTypes...> f,
            ArgumentsTypes&&... arguments)
    requires((std::is_invocable_v<function_ptr<ResultType, VariablesTypes...>,
                                  ArgumentsTypes...>) &&
             (N < sizeof...(ArgumentsTypes)))
  {
    f(arguments...);
  }  // end of diff

  template <std::size_t N,
            typename TrivialLambdaType,
            typename... ArgumentsTypes>
  void diff(const TrivialLambdaType& c, ArgumentsTypes&&... arguments)
    requires((is_function_pointer<
                 decltype(+std::declval<TrivialLambdaType>())>::value) &&
             (std::is_invocable_v<TrivialLambdaType, ArgumentsTypes...>) &&
             (N < sizeof...(ArgumentsTypes)))
  {
    diff<N>(+c, std::forward<ArgumentsTypes>(arguments)...);
  }  // end of diff

  template <VariableConcept VariableType>
  struct VariableValueAndIncrement {
    VariableType value;
    VariableType increment;
  };

  template <typename T>
  struct IsVariableValueAndIncrement : std::false_type {};

  template <VariableConcept VariableType>
  struct IsVariableValueAndIncrement<VariableValueAndIncrement<VariableType>>
      : std::true_type {};

  template <typename VariableType>
  constexpr bool isVariableValueAndIncrement() noexcept {
    return IsVariableValueAndIncrement<VariableType>::value;
  }

  template <typename... Types>
  struct TypeList {};

  //   template <typename CurrentVariableType,
  //             typename... VariablesTypes,
  //             typename CurrentArgumentType,
  //             typename... ArgumentsTypes>
  //   requires(sizeof...(VariablesTypes) == sizeof...(ArgumentsTypes)) auto
  //   transformArguments(
  //       const TypeList<CurrentVariableType, VariablesTypes...>&,
  //       CurrentArgumentType&& argument,
  //       ArgumentsTypes&&... next_arguments) {
  //     const auto cargs = [](CurrentArgumentType&& carg) {
  //       if constexpr (isVariableValueAndIncrement<
  //                         std::decay_t<CurrentArgumentType>>()) {
  //         return std::forward_as_tuple(enzyme_dup, carg.value,
  //                                      carg.increment);
  //       } else {
  //         if constexpr (std::is_same_v<std::decay_t<CurrentVariableType>,
  //                                      std::decay_t<CurrentArgumentType>>) {
  //           if constexpr (std::is_reference_v<CurrentVariableType>) {
  //             return std::forward_as_tuple<CurrentArgumentType *>(&carg);
  //           } else {
  //             return std::forward_as_tuple(carg);
  //           }
  //         } else {
  //           return std::tuple<CurrentArgumentType>{carg};
  //         }
  //       }
  //     }(std::forward<CurrentArgumentType>(argument));
  //     if constexpr (sizeof...(VariablesTypes) == 0) {
  //       return cargs;
  //     } else {
  //       return std::tuple_cat(
  //           cargs,
  //           transformArguments(TypeList<VariablesTypes...>{},
  //           next_arguments...));
  //     }
  //   } // end of transformArguments
  //
  //   template <typename ResultType,
  //             typename... VariablesTypes,
  //             typename... ArgumentsTypes>
  //   auto call_enzyme_fwdiff(const TypeList<VariablesTypes...>&,
  //                           const std::tuple<ArgumentsTypes...>& args) {
  //     return std::apply(__enzyme_fwddiff<ResultType, VariablesTypes...>,
  //     args);
  //   }

  struct EnzymeConst {};
  struct EnzymeDup {};

  template <std::size_t I, class... Types>
  constexpr std::conditional_t<
      std::is_same_v<std::tuple_element_t<I, std::tuple<Types...>>,
                     EnzymeConst>,
      int&,
      std::conditional_t<
          std::is_same_v<std::tuple_element_t<I, std::tuple<Types...>>,
                         EnzymeDup>,
          int&,
          std::tuple_element_t<I, std::tuple<Types...>>&&>>
  my_get(std::tuple<Types...>&& t) noexcept {
    if constexpr (std::is_same_v<std::tuple_element_t<I, std::tuple<Types...>>,
                                 EnzymeConst>) {
      return enzyme_const;
    } else if constexpr (std::is_same_v<
                             std::tuple_element_t<I, std::tuple<Types...>>,
                             EnzymeDup>) {
      return enzyme_dup;
    } else {
      return std::get<I>(std::forward<std::tuple<Types...>>(t));
    }
  };

  template <typename Arg, typename... Args>
  void print(Arg&& arg, Args&&... args) {
    std::cout << &arg;
    ((std::cout << ", " << &args), ...);
    std::cout << std::endl;
  }

  template <typename... Args, std::size_t... I>
  void print_tuple_impl(std::tuple<Args...>&& args, std::index_sequence<I...>) {
    print(my_get<I>(std::forward<std::tuple<Args...>>(args))...);
  }

  template <typename... Args>
  void print_tuple(std::tuple<Args...>&& args) {
    print_tuple_impl(
        std::forward<std::tuple<Args...>>(args),
        std::make_index_sequence<std::tuple_size_v<std::tuple<Args...>>>{});
  }

  template <typename ResultType,
            typename... EnzymeArgumentsTypes,
            typename... TupleArgs,
            std::size_t... I>
  auto call_enzyme_fwddiff_impl(const TypeList<EnzymeArgumentsTypes...>&,
                                void* const f,
                                std::tuple<TupleArgs...>&& args,
                                std::index_sequence<I...>) {
    std::cout << "final arguments:\n";
    std::cout << "ptr: " << f << std::endl;
    std::cout << "final arguments (2):\n";
    print_tuple(std::forward<std::tuple<TupleArgs...>>(args));
    return __enzyme_fwddiff<ResultType, EnzymeArgumentsTypes...>(
        f, my_get<I>(std::forward<std::tuple<TupleArgs...>>(args))...);
  }  // end of call_enzyme_fwddiff_imp

  template <typename ResultType,
            typename... EnzymeArgumentsTypes,
            typename... TupleArgs>
  auto call_enzyme_fwddiff(const TypeList<EnzymeArgumentsTypes...>&,
                           const TypeList<>&,
                           void* const f,
                           std::tuple<TupleArgs...>&& args) {
    return call_enzyme_fwddiff_impl<ResultType, EnzymeArgumentsTypes...>(
        TypeList<EnzymeArgumentsTypes...>{}, f,
        std::forward<std::tuple<TupleArgs...>>(args),
        std::make_index_sequence<
            std::tuple_size_v<std::tuple<TupleArgs...>>>{});
  }  // end of call_enzyme_fwddiff

  template <typename ResultType,
            typename... EnzymeArgumentsTypes,
            typename... TupleArgs,
            VariableConcept CurrentVariableType,
            VariableConcept... VariablesTypes,
            typename CurrentArgumentType,
            typename... ArgumentsTypes>
  auto call_enzyme_fwddiff(
      const TypeList<EnzymeArgumentsTypes...>&,
      const TypeList<CurrentVariableType, VariablesTypes...>&,
      void* const f,
      const std::tuple<TupleArgs...>& args,
      CurrentArgumentType&& argument,
      ArgumentsTypes&&... arguments)
    requires(sizeof...(VariablesTypes) == sizeof...(ArgumentsTypes))
  {
    std::cout << "ptr: " << f << std::endl;
    if constexpr (isVariableValueAndIncrement<
                      std::decay_t<CurrentArgumentType>>()) {
      return call_enzyme_fwddiff<ResultType>(
          TypeList<EnzymeArgumentsTypes..., int, decltype(argument.value),
                   decltype(argument.increment)>{},
          TypeList<VariablesTypes...>{},
          f, std::tuple_cat(
              args, std::make_tuple(EnzymeDup{}),
              std::forward_as_tuple(argument.value, argument.increment)),
          arguments...);
    } else {
      if constexpr (std::is_same_v<std::decay_t<CurrentVariableType>,
                                   std::decay_t<CurrentArgumentType>>) {
        if constexpr (std::is_reference_v<CurrentVariableType>) {
          return call_enzyme_fwddiff<ResultType>(
              TypeList<EnzymeArgumentsTypes..., int, CurrentVariableType*>{},
              TypeList<VariablesTypes...>{},
              f, std::tuple_cat(args, std::make_tuple(EnzymeConst{}),
                             std::forward_as_tuple(&argument)),
              arguments...);
        } else {
          return call_enzyme_fwddiff<ResultType>(
              TypeList<EnzymeArgumentsTypes..., int, CurrentVariableType>{},
              TypeList<VariablesTypes...>{},
              f, std::tuple_cat(args, std::make_tuple(EnzymeConst{}),
                             std::forward_as_tuple(argument)),
              arguments...);
        }
      } else {
        return call_enzyme_fwddiff<ResultType>(
            TypeList<EnzymeArgumentsTypes..., int, CurrentVariableType>{},
            TypeList<VariablesTypes...>{},
            f, std::tuple_cat(args, std::make_tuple(EnzymeConst{}),
                           std::tuple<int&, CurrentVariableType>{argument}),
            arguments...);
      }
    }
  }  // end of call_enzyme_fwddiff

  template <typename ResultType,
            VariableConcept... VariablesTypes,
            typename... ArgumentsTypes>
  auto diff(function_ptr<ResultType, VariablesTypes...> f,
            ArgumentsTypes&&... arguments)
    requires(sizeof...(VariablesTypes) == sizeof...(ArgumentsTypes))
  {
    auto* const ptr = reinterpret_cast<void*>(f);
    std::cout << "ptr: " << ptr << std::endl;
    return call_enzyme_fwddiff<ResultType>(TypeList<>{},
                                           TypeList<VariablesTypes...>{}, ptr,
                                           std::make_tuple(), arguments...);
  }

}  // end of namespace tfel::math::enzyme

#endif /* LIB_TFEL_MATH_ENZYME_DIFF_HXX */
