/*!
 * \file   tests/getReverseModeDerivativeFunction.cxx
 * \brief
 * \author Thomas Helfer
 * \date   09/11/2026
 */

#include <cmath>
#include <cstdlib>
#include <iostream>
#include "TFEL/Math/power.hxx"
#include "TFEL/Math/stensor.hxx"
#include "TFEL/Math/Stensor/StensorConceptIO.hxx"
#include "TFEL/Math/Enzyme/getReverseModeDerivativeFunction.hxx"

#include "TFEL/Tests/TestCase.hxx"
#include "TFEL/Tests/TestProxy.hxx"
#include "TFEL/Tests/TestManager.hxx"

static double f_trace(const tfel::math::stensor<2u, double>& s) {
  return tfel::math::trace(s);
}  // end of f_trace

static double f2(const double x, const double y) {
  return x + tfel::math::power<2>(y);
}  // end of f2

static double f3(const double x, const double y) {
  return x * y + 1.0 / y;
}  // end of f3

struct TFELMathEnzymeGetReverseModeDerivativeFunction final
    : public tfel::tests::TestCase {
  TFELMathEnzymeGetReverseModeDerivativeFunction()
      : tfel::tests::TestCase(
            "TFEL/Math/Enzyme",
            "TFELMathEnzymeGetReverseModeDerivativeFunction") {
  }  // end of TFELMathEnzymeGetReverseModeDerivativeFunction
  tfel::tests::TestResult execute() override {
    this->test1();
    this->test2();
    this->test3();
    this->test4();
    return this->result;
  }  // end of execute
 private:
  void test1() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = 1e-14;
    auto f = [](const double x) { return tfel::math::power<3>(x); };
    // first derivative
    auto df = getReverseModeDerivativeFunction<0>(f);
    TFEL_TESTS_ASSERT(std::abs(df(2) - 12) < eps);
    // second derivative
    auto d2f = getReverseModeDerivativeFunction<0, 0>(f);
    TFEL_TESTS_ASSERT(std::abs(d2f(1) - 6) < eps);
  }
  void test2() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    using Stensor = stensor<2u, double>;
    const auto s = Stensor{150e6};
    // derivative with respect to a math object
    const auto ds = getReverseModeDerivativeFunction<0>(
        [](const Stensor& v) { return trace(v); });
    TFEL_TESTS_ASSERT(abs(ds(s) - Stensor::Id()) < eps);
    // same test with a regular function
    const auto ds2 = getReverseModeDerivativeFunction<0>(function<f_trace>);
    TFEL_TESTS_ASSERT(abs(ds2(s) - Stensor::Id()) < eps);
  }
  void test3() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    // differentiation with respect to a single argument of a function of two
    // variables
    const auto dx = getReverseModeDerivativeFunction<0>(function<f2>);
    TFEL_TESTS_ASSERT(std::abs(dx(2, 1) - 1) < eps);
    const auto dy = getReverseModeDerivativeFunction<1>(function<f2>);
    TFEL_TESTS_ASSERT(std::abs(dy(2, 1) - 2) < eps);
    // repeated indices give higher order derivatives, a sequence of distinct
    // indices gives mixed derivatives, computed by nested differentiation:
    // here \(d^2 f3 / dx dy = 1\), as \(df3 / dx = y\)
    const auto d2f3 = getReverseModeDerivativeFunction<0, 1>(function<f3>);
    TFEL_TESTS_ASSERT(std::abs(d2f3(3.0, 2.0) - 1) < eps);
  }
  void test4() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    // mixed derivatives of a callable of a scalar argument and a math object
    // argument, computed by nested differentiation: here
    // \(d^2 f / dx da = \mathit{Id}\), as \(df / dx = \mathit{trace}(a)\)
    using tfel::math::stensor;
    using Stensor = stensor<3u, double>;
    const auto s = Stensor{1, 2, 3, 4, 5, 6};
    const auto f = [](const double a, const Stensor& x) {
      return a * trace(x);
    };
    const auto df = getReverseModeDerivativeFunction<1, 0>(f);
    const auto d2f = df(3.0, s);
    TFEL_TESTS_ASSERT(abs(d2f - Stensor::Id()) < eps);
  }
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzymeGetReverseModeDerivativeFunction,
                          "TFELMathEnzymeGetReverseModeDerivativeFunction");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme-getReverseModeDerivativeFunction.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}
