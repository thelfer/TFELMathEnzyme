/*!
 * \file   tests/computeForwardModeDerivative.cxx
 * \brief
 * \author Thomas Helfer
 * \date   09/11/2026
 */

#include <cstdlib>
#include <iostream>
#include "TFEL/Math/power.hxx"
#include "TFEL/Math/stensor.hxx"
#include "TFEL/Math/tmatrix.hxx"
#include "TFEL/Math/tvector.hxx"
#include "TFEL/Math/Enzyme/computeForwardModeDerivative.hxx"

#include "TFEL/Tests/TestCase.hxx"
#include "TFEL/Tests/TestProxy.hxx"
#include "TFEL/Tests/TestManager.hxx"

using Stensor = tfel::math::stensor<3u, double>;
using TVector2 = tfel::math::tvector<2u, double>;
using TVector3 = tfel::math::tvector<3u, double>;
using TMatrix = tfel::math::tmatrix<3u, 2u, double>;

static double f_cube(const double x) {
  return tfel::math::power<3>(x);
}  // end of f_cube

static double f_linear(const Stensor& s) {
  return s[0] + 2 * s[1];
}  // end of f_linear

struct TFELMathEnzymeComputeForwardModeDerivative final
    : public tfel::tests::TestCase {
  TFELMathEnzymeComputeForwardModeDerivative()
      : tfel::tests::TestCase("TFEL/Math/Enzyme",
                              "TFELMathEnzymeComputeForwardModeDerivative") {
  }  // end of TFELMathEnzymeComputeForwardModeDerivative
  tfel::tests::TestResult execute() override {
    this->test1();
    this->test2();
    this->test3();
    this->test4();
    this->test5();
    return this->result;
  }  // end of execute
 private:
  // differentiation of a callable of a scalar with respect to its argument
  void test1() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    const auto c = [](const double x) { return tfel::math::power<3>(x); };
    TFEL_TESTS_ASSERT(std::abs(computeForwardModeDerivative(c, 2.) - 12.) <
                      eps);
  }  // end of test1
  // differentiation of a free function of a scalar with respect to its argument
  void test2() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    TFEL_TESTS_ASSERT(
        std::abs(computeForwardModeDerivative(function<f_cube>, 3.) - 27.) <
        eps);
  }  // end of test2
  // gradient of a callable of a math object returning a scalar
  void test3() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    const auto c = [](const Stensor& s) { return s[0] + 2 * s[1]; };
    const auto s = Stensor{1, 2, 3, 4, 5, 6};
    const auto g = computeForwardModeDerivative(c, s);
    TFEL_TESTS_ASSERT(std::abs(g[0] - 1.) < eps);
    TFEL_TESTS_ASSERT(std::abs(g[1] - 2.) < eps);
    TFEL_TESTS_ASSERT(std::abs(g[2]) < eps);
    TFEL_TESTS_ASSERT(std::abs(g[5]) < eps);
  }  // end of test3
  // Jacobian of a callable of a math object returning a math object: the
  // derivative is stored as `d(i, j) = d\,result(i) / d\,variable(j)`
  void test4() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    const auto a = []() {
      auto m = TMatrix{};
      for (unsigned short i = 0u; i != 3u; ++i) {
        for (unsigned short j = 0u; j != 2u; ++j) {
          m(i, j) = 1. + 2. * i + j;
        }
      }
      return m;
    }();
    const auto c = [&a](const TVector2& v) {
      auto r = TVector3{};
      for (unsigned short i = 0u; i != 3u; ++i) {
        for (unsigned short j = 0u; j != 2u; ++j) {
          r(i) += a(i, j) * v(j);
        }
      }
      return r;
    };
    const auto v = TVector2{1.5, -2.5};
    const auto d = computeForwardModeDerivative(c, v);
    for (unsigned short i = 0u; i != 3u; ++i) {
      for (unsigned short j = 0u; j != 2u; ++j) {
        TFEL_TESTS_ASSERT(std::abs(d(i, j) - a(i, j)) < eps);
      }
    }
  }  // end of test4
  // differentiation of a free function of a math object
  void test5() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    const auto s = Stensor{1, 2, 3, 4, 5, 6};
    const auto g = computeForwardModeDerivative(function<f_linear>, s);
    TFEL_TESTS_ASSERT(std::abs(g[0] - 1.) < eps);
    TFEL_TESTS_ASSERT(std::abs(g[1] - 2.) < eps);
    TFEL_TESTS_ASSERT(std::abs(g[3]) < eps);
  }  // end of test5
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzymeComputeForwardModeDerivative,
                          "TFELMathEnzymeComputeForwardModeDerivative");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme-compute-forward-mode-derivative.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}
