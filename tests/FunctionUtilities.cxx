/*!
 * \file   tests/FunctionUtilities.cxx
 * \brief
 * \author Thomas Helfer
 * \date   09/11/2026
 */

#include <cstdlib>
#include <iostream>
#include <type_traits>
#include "TFEL/Math/stensor.hxx"
#include "TFEL/Math/qt.hxx"
#include "TFEL/Math/Enzyme/Internals/FunctionUtilities.hxx"

#include "TFEL/Tests/TestCase.hxx"
#include "TFEL/Tests/TestProxy.hxx"
#include "TFEL/Tests/TestManager.hxx"

using Stensor = tfel::math::stensor<3u, double>;
using NoUnit = tfel::math::unit::NoUnit;
using QT = tfel::math::qt<NoUnit, double>;
using Mode = tfel::math::enzyme::Mode;

namespace internals = tfel::math::enzyme::internals;

static double f0(const double x) { return x * x; }  // end of f0

static void f1(const Stensor& s, double& v) { v = s[0]; }  // end of f1

struct MyClass {
  double g(const double x) { return x + 1; }  // end of g
  double h(const double x) const { return x + 2; }  // end of h
};  // end of MyClass

struct MyFunctor {
  double operator()(const double x) const {
    return x * x;
  }  // end of operator()
};  // end of MyFunctor

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// isFunction, isFunctionPointer and the associated concepts
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static_assert(internals::isFunction<decltype(f0)>());
static_assert(internals::isFunction<double(double)>());
static_assert(!internals::isFunction<double (*)(double)>());
static_assert(!internals::isFunction<MyFunctor>());
static_assert(!internals::isFunction<double>());

static_assert(internals::isFunctionPointer<decltype(&f0)>());
static_assert(internals::isFunctionPointer<double (*)(double)>());
static_assert(!internals::isFunctionPointer<decltype(f0)>());
static_assert(!internals::isFunctionPointer<MyFunctor>());
static_assert(!internals::isFunctionPointer<double>());
static_assert(internals::IsFunctionPointerConcept<decltype(&f0)>);
static_assert(!internals::IsFunctionPointerConcept<decltype(f0)>);

static_assert(internals::hasCallOperator<MyFunctor>());
// `FunctionWrapper` is a plain tag: it has no call operator, which is why
// regular functions require dedicated entry points
static_assert(!internals::hasCallOperator<internals::FunctionWrapper<f0>>());
static_assert(!internals::hasCallOperator<decltype(f0)>());
static_assert(!internals::hasCallOperator<decltype(&f0)>());
static_assert(!internals::hasCallOperator<double>());

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// EnzymeCallableConcept: callables which are neither functions nor function
// pointers are valid Enzyme callables
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static_assert(internals::EnzymeCallableConcept<MyFunctor>);
static_assert(!internals::EnzymeCallableConcept<decltype(f0)>);
static_assert(!internals::EnzymeCallableConcept<decltype(&f0)>);
static_assert(
    !internals::EnzymeCallableConcept<internals::FunctionWrapper<f0>>);
static_assert(!internals::EnzymeCallableConcept<double>);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// isTemporary
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static_assert(!internals::isTemporary<double>());
static_assert(!internals::isTemporary<Stensor>());
static_assert(!internals::isTemporary<QT>());
static_assert(internals::isTemporary<decltype(2 * Stensor{})>());
static_assert(internals::isTemporary<tfel::math::View<Stensor>>());
static_assert(internals::isTemporary<tfel::math::qt_ref<NoUnit, double>>());
static_assert(
    internals::isTemporary<tfel::math::const_qt_ref<NoUnit, double>>());

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TypeList
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static_assert(internals::getTypeListSize(internals::TypeList<>{}) == 0u);
static_assert(internals::getTypeListSize(internals::TypeList<double>{}) == 1u);
static_assert(
    internals::getTypeListSize(internals::TypeList<double, Stensor>{}) == 2u);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FunctionTraits, getArgumentsList and getArgumentsSize
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// free function designator
static_assert(std::is_same_v<internals::FunctionTraits<decltype(f0)>::type,
                             internals::TypeList<double>>);
static_assert(internals::getArgumentsSize<decltype(f0)>() == 1u);
// function pointer
static_assert(std::is_same_v<internals::FunctionTraits<decltype(&f0)>::type,
                             internals::TypeList<double>>);
// function pointer with two arguments, a math object and a reference
static_assert(std::is_same_v<internals::FunctionTraits<decltype(&f1)>::type,
                             internals::TypeList<const Stensor&, double&>>);
static_assert(internals::getArgumentsSize<decltype(&f1)>() == 2u);
// member function pointers, const and non-const
static_assert(
    std::is_same_v<internals::FunctionTraits<double (MyClass::*)(double)>::type,
                   internals::TypeList<double>>);
static_assert(
    std::is_same_v<
        internals::FunctionTraits<double (MyClass::*)(double) const>::type,
        internals::TypeList<double>>);
// functor: the traits are deduced from the call operator, whose argument types
// are those of the corresponding function pointer type
static_assert(std::is_same_v<internals::FunctionTraits<MyFunctor>::type,
                             internals::TypeList<double>>);
static_assert(internals::getArgumentsSize<MyFunctor>() == 1u);
// `getArgumentsList` returns a default constructed `TypeList`
static_assert(
    std::is_same_v<decltype(internals::getArgumentsList<decltype(f0)>()),
                   internals::TypeList<double>>);
static_assert(
    std::is_same_v<decltype(internals::getArgumentsList<decltype(&f1)>()),
                   internals::TypeList<const Stensor&, double&>>);
static_assert(internals::getTypeListSize(
                  internals::getArgumentsList<decltype(&f1)>()) == 2u);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// checkIndices
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static_assert(internals::checkIndices<Mode::REVERSE>());
static_assert(internals::checkIndices<Mode::REVERSE, 0u, 1u>());
static_assert(internals::checkIndices<Mode::REVERSE, 1u, 1u, 5u>());
static_assert(!internals::checkIndices<Mode::FORWARD>());
static_assert(internals::checkIndices<Mode::FORWARD, 0u>());
static_assert(!internals::checkIndices<Mode::FORWARD, 1u>());
static_assert(!internals::checkIndices<Mode::FORWARD, 0u, 0u>());

struct TFELMathEnzymeFunctionUtilities final : public tfel::tests::TestCase {
  TFELMathEnzymeFunctionUtilities()
      : tfel::tests::TestCase("TFEL/Math/Enzyme",
                              "TFELMathEnzymeFunctionUtilities") {
  }  // end of TFELMathEnzymeFunctionUtilities
  tfel::tests::TestResult execute() override {
    this->test1();
    this->test2();
    this->test3();
    return this->result;
  }  // end of execute
 private:
  // `function` and `callable_wrapper`
  void test1() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    // the `function` inline variable generates function wrappers, which are
    // used as tags: regular functions must be passed as template parameters so
    // that the symbol associated with the function is seen by Enzyme
    static_assert(std::is_same_v<std::remove_cvref_t<decltype(function<f0>)>,
                                 internals::FunctionWrapper<f0>>);
    // `callable_wrapper` turns the call operator of a callable into a plain
    // function which can be differentiated by Enzyme
    const auto c = MyFunctor{};
    constexpr auto wc =
        internals::callable_wrapper<decltype(c), internals::TypeList<double>>;
    TFEL_TESTS_ASSERT(std::abs(wc(&c, 2.) - 4.) < eps);
  }  // end of test1
  // `convertToEnzymeArgument`
  void test2() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    // plain values are converted to `enzyme::Const`
    const auto a = internals::convertToEnzymeArgument<double>(2.);
    static_assert(std::is_same_v<std::remove_cvref_t<decltype(a)>,
                                 ::enzyme::Const<double>>);
    TFEL_TESTS_ASSERT(std::abs(a.value - 2.) < eps);
    // `enzyme::Const` objects are forwarded
    const auto b =
        internals::convertToEnzymeArgument<double>(::enzyme::Const<double>{3.});
    TFEL_TESTS_ASSERT(std::abs(b.value - 3.) < eps);
    // `VariableValueAndIncrement` objects are converted to `enzyme::Duplicated`
    const auto v = VariableValueAndIncrement<double>{2., 3.};
    const auto d = internals::convertToEnzymeArgument<double>(v);
    static_assert(std::is_same_v<std::remove_cvref_t<decltype(d)>,
                                 ::enzyme::Duplicated<double>>);
    TFEL_TESTS_ASSERT(std::abs(d.value - 2.) < eps);
    TFEL_TESTS_ASSERT(std::abs(d.shadow - 3.) < eps);
  }  // end of test2
  // `convertToEnzymeReverseArgument`
  void test3() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    auto x = double{2.};
    // scalar arguments are converted to `enzyme::Const`, both when the
    // differentiated function expects them by value and by reference
    const auto a = internals::convertToEnzymeReverseArgument<double>(x);
    static_assert(std::is_same_v<std::remove_cvref_t<decltype(a)>,
                                 ::enzyme::Const<double>>);
    TFEL_TESTS_ASSERT(std::abs(a.value - 2.) < eps);
    const auto r = internals::convertToEnzymeReverseArgument<const double&>(x);
    static_assert(std::is_same_v<std::remove_cvref_t<decltype(r)>,
                                 ::enzyme::Const<const double&>>);
    TFEL_TESTS_ASSERT(std::abs(*r.value - 2.) < eps);
    // math objects passed by reference are converted to pointers
    const auto s = Stensor{1, 2, 3, 4, 5, 6};
    const auto p = internals::convertToEnzymeReverseArgument<const Stensor&>(s);
    static_assert(
        std::is_same_v<std::remove_cvref_t<decltype(p)>, const Stensor*>);
    TFEL_TESTS_ASSERT(p == std::addressof(s));
    // math objects passed by value are converted to `enzyme::Const`
    const auto b =
        internals::convertToEnzymeReverseArgument<Stensor>(Stensor{});
    static_assert(std::is_same_v<std::remove_cvref_t<decltype(b)>,
                                 ::enzyme::Const<Stensor>>);
    // `VariableValueAndIncrement` objects are converted to
    // `enzyme::Duplicated`
    const auto v = VariableValueAndIncrement<double>{4., 5.};
    const auto d = internals::convertToEnzymeReverseArgument<double>(v);
    TFEL_TESTS_ASSERT(std::abs(d.value - 4.) < eps);
    TFEL_TESTS_ASSERT(std::abs(d.shadow - 5.) < eps);
  }  // end of test3
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzymeFunctionUtilities,
                          "TFELMathEnzymeFunctionUtilities");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme-function-utilities.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}
