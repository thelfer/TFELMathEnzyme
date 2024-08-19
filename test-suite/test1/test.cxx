/*!
 * \file   test-suite/test1/test.cxx
 * \brief
 * \author Thomas Helfer
 * \date   28/06/2024
 */

#include <cmath>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <type_traits>
#include "TFEL/Math/power.hxx"
#include "TFEL/Math/stensor.hxx"
#include "TFEL/Math/Stensor/StensorConceptIO.hxx"
#include "TFEL/Math/st2tost2.hxx"
#include "TFEL/Math/ST2toST2/ST2toST2ConceptIO.hxx"
#include "TFEL/Math/Enzyme/fwddiff.hxx"
#include "TFEL/Math/Enzyme/autodiff.hxx"

#include "TFEL/Tests/TestCase.hxx"
#include "TFEL/Tests/TestProxy.hxx"
#include "TFEL/Tests/TestManager.hxx"

static double f(const double x) { return tfel::math::power<3>(x); }
static double f2(const double x, const double y) {
  return x + tfel::math::power<2>(y);
}
static double f3(const double x, const double y) { return x * y + 1.0 / y; }

static double my_trace(const tfel::math::stensor<2u, double>& s) {
  return tfel::math::trace(s);
}

static double my_trace2(const tfel::math::stensor<2u, double> s) {
  return tfel::math::trace(s);
}

template<unsigned short N>
static double mult_by_2(const tfel::math::stensor<2u, double>& s) {
  return (2 * s)(N);
}

static tfel::math::stensor<2u, double> deviator(
    const tfel::math::stensor<2u, double>& s) {
  return ::tfel::math::deviator(s);
} // end of deviator

struct TFELMathEnzyme final : public tfel::tests::TestCase {
  TFELMathEnzyme()
      : tfel::tests::TestCase("TFEL/Math/Enzyme", "TFELMathEnzyme") {
  }  // end of TFELMathEnzyme
  tfel::tests::TestResult execute() override {
    this->test1();
    this->test2();
    this->test3();
    this->test4();
    this->test5();
    this->test6();
    return this->result;
  }  // end of execute
 private:
  void test1() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    //
    TFEL_TESTS_STATIC_ASSERT(internals::IsFunctionConcept<decltype(f)>);
    // functions of one variable
    TFEL_TESTS_ASSERT(std::abs(fwddiff([](const double x) { return f(x); },
                                    VariableValueAndIncrement<double>{2, 1}) -
                               12) < eps);
    TFEL_TESTS_ASSERT(std::abs(fwddiff([](const double x) { return f(x); },
                                    make_vdv<double>(2, 1)) -
                               12) < eps);
    TFEL_TESTS_ASSERT(std::abs(fwddiff(function<f>, make_vdv<double>(2, 1)) - 12) <
                      eps);
    // functions of two variables
    TFEL_TESTS_ASSERT(
        std::abs(fwddiff([](const double x, const double y) { return f2(x, y); },
                      0, VariableValueAndIncrement<double>{3, 1}) -
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
  void test3() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    constexpr auto a = double{2.3};
    const auto v = double{1};
    const auto c = [](const double x) { return std::cos(x); };
    TFEL_TESTS_ASSERT(std::abs(computeGradient(c, v) + std::sin(v)) < eps);
    TFEL_TESTS_ASSERT(std::abs(computeGradient<0>(c, v) + std::sin(v)) < eps);
    const auto c2 = [a](const double x) { return a * std::sin(x); };
    const auto dc2_dx = computeGradient(c2, v);
    const auto c3 = [&c](const double x) {
      return computeGradient(c, x);
    };
    const auto dc3_dx = computeGradient(c3, v);
    TFEL_TESTS_ASSERT(std::abs(dc3_dx + std::cos(v)) < eps);
    TFEL_TESTS_ASSERT(std::abs(dc2_dx - a * std::cos(v)) < eps);
    TFEL_TESTS_ASSERT(std::abs(computeGradient(function<f>, 2) - 12) < eps);
    TFEL_TESTS_ASSERT(std::abs(computeGradient<0>(function<f>, 2) - 12) < eps);
    TFEL_TESTS_ASSERT(std::abs(computeGradient<0>(function<f2>, 2, 1) - 1) < eps);
    TFEL_TESTS_ASSERT(std::abs(computeGradient<1>(function<f2>, 2, 1) - 2) < eps);
    const auto [df_dx, df_dy] = computeGradient<0, 1>(function<f2>, 2, 1);
    TFEL_TESTS_ASSERT(std::abs(df_dx - 1) < eps);
    TFEL_TESTS_ASSERT(std::abs(df_dy - 2) < eps);
  }
  void test4() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    const auto [df_dx, df_dy] = computeGradient(function<f3>, 3, 2);
    TFEL_TESTS_ASSERT(std::abs(df_dx - 2) < eps);
    TFEL_TESTS_ASSERT(std::abs(df_dy - 2.75) < eps);
  }
  void test5() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    constexpr auto id = stensor<2u, double>::Id();
    const auto r = computeGradient(function<my_trace>, id);
    TFEL_TESTS_ASSERT(abs(r - id) < eps);
    const auto r2 = computeGradient(function<my_trace2>, id);
    TFEL_TESTS_ASSERT(abs(r2 - id) < eps);
    TFEL_TESTS_ASSERT(abs(computeGradient(function<mult_by_2<1>>, id) -
                          stensor<2, double>{0, 2, 0, 0}) < eps);
  }
  void test6() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    constexpr auto id = stensor<2u, double>::Id();
    constexpr auto K = st2tost2<2u, double>::K();
    const auto r = computeGradient(function<::deviator>, id);
    TFEL_TESTS_ASSERT(abs(r - K) < eps);
  }
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzyme, "TFELMathEnzyme");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}
