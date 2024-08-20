/*!
 * \file   tests/test1.cxx
 * \brief
 * \author Thomas Helfer
 * \date   20/08/2024
 */

#include <cmath>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <type_traits>
#include "TFEL/Math/qt.hxx"
#include "TFEL/Math/power.hxx"
#include "TFEL/Math/stensor.hxx"
#include "TFEL/Math/Stensor/StensorConceptIO.hxx"
#include "TFEL/Math/st2tost2.hxx"
#include "TFEL/Math/ST2toST2/ST2toST2ConceptIO.hxx"
#include "TFEL/Material/Lame.hxx"
#include "TFEL/Math/Enzyme/fwddiff.hxx"
#include "TFEL/Math/Enzyme/autodiff.hxx"

#include "TFEL/Tests/TestCase.hxx"
#include "TFEL/Tests/TestProxy.hxx"
#include "TFEL/Tests/TestManager.hxx"

static double f(const double x) { return tfel::math::power<3>(x); }
static double f2(const double x, const double y) {
  return x + tfel::math::power<2>(y);
}

struct TFELMathEnzymeFwdDiff final : public tfel::tests::TestCase {
  TFELMathEnzymeFwdDiff()
      : tfel::tests::TestCase("TFEL/Math/Enzyme", "TFELMathEnzymeFwdDiff") {
  }  // end of TFELMathEnzymeFwdDiff
  tfel::tests::TestResult execute() override {
    this->test1();
    this->test2();
    return this->result;
  }  // end of execute
 private:
  void test1() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    //
    TFEL_TESTS_STATIC_ASSERT(internals::IsFunctionConcept<decltype(f)>);
    // functions of one variable
    TFEL_TESTS_ASSERT(
        std::abs(fwddiff([](const double x) { return f(x); },
                         VariableValueAndIncrement<double>{2, 1}) -
                 12) < eps);
    TFEL_TESTS_ASSERT(std::abs(fwddiff([](const double x) { return f(x); },
                                       make_vdv<double>(2, 1)) -
                               12) < eps);
    TFEL_TESTS_ASSERT(
        std::abs(fwddiff(function<f>, make_vdv<double>(2, 1)) - 12) < eps);
    // functions of two variables
    TFEL_TESTS_ASSERT(
        std::abs(
            fwddiff([](const double x, const double y) { return f2(x, y); }, 0,
                    VariableValueAndIncrement<double>{3, 1}) -
            6) < eps);
    TFEL_TESTS_ASSERT(
        std::abs(fwddiff(function<f2>, 0, make_vdv<double>(3, 1)) - 6) < eps);
  }
  void test2() {
    using namespace tfel::math::enzyme;
    constexpr auto a = double{2.3};
    constexpr auto eps = double{1e-14};
    auto v = VariableValueAndIncrement<double>{1, 1};
    static_assert(internals::isVariableValueAndIncrement<decltype(v)>());
    const auto c = [](const double x) { return std::cos(x); };
    const auto dc_dx = fwddiff(c, v);
    TFEL_TESTS_ASSERT(std::abs(dc_dx + std::sin(v.value)) < eps);
    const auto c2 = [a](const double x) { return a * std::sin(x); };
    const auto dc2_dx = fwddiff(c2, v);
    TFEL_TESTS_ASSERT(std::abs(dc2_dx - a * std::cos(v.value)) < eps);
    const auto c3 = [&c](const double x) {
      return fwddiff(c, VariableValueAndIncrement<double>{x, 1});
    };
    const auto dc3_dx = fwddiff(c3, v);
    TFEL_TESTS_ASSERT(std::abs(dc3_dx + std::cos(v.value)) < eps);
  }
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzymeFwdDiff, "TFELMathEnzymeFwdDiff");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme-fwddiff.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}
