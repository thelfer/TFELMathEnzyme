/*!
 * \file   tests/computeDerivative.cxx
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
#include "TFEL/Math/Enzyme/computeDerivative.hxx"

#include "TFEL/Tests/TestCase.hxx"
#include "TFEL/Tests/TestProxy.hxx"
#include "TFEL/Tests/TestManager.hxx"

static double f_cube(const double x) {
  return tfel::math::power<3>(x);
}  // end of f_cube

static double f_x_plus_y2(const double x, const double y) {
  return x + tfel::math::power<2>(y);
}  // end of f_x_plus_y2

struct TFELMathEnzymeComputeDerivative final : public tfel::tests::TestCase {
  TFELMathEnzymeComputeDerivative()
      : tfel::tests::TestCase("TFEL/Math/Enzyme",
                              "TFELMathEnzymeComputeDerivative") {
  }  // end of TFELMathEnzymeComputeDerivative
  tfel::tests::TestResult execute() override {
    using namespace tfel::math;
    this->test1<tfel::math::enzyme::Mode::FORWARD>();
    this->test1<tfel::math::enzyme::Mode::REVERSE>();
    this->test2<tfel::math::enzyme::Mode::FORWARD>();
    this->test2<tfel::math::enzyme::Mode::REVERSE>();
    this->test3<tfel::math::enzyme::Mode::FORWARD>();
    this->test3<tfel::math::enzyme::Mode::REVERSE>();
    this->test4();
    return this->result;
  }  // end of execute
 private:
  template <tfel::math::enzyme::Mode m>
  void test1() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    constexpr auto a = double{2.3};
    const auto v = double{1};
    const auto c = [](const double x) { return std::cos(x); };
    TFEL_TESTS_ASSERT(std::abs(computeDerivative<m, 0>(c, v) + std::sin(v)) <
                      eps);
    TFEL_TESTS_ASSERT(std::abs(computeDerivative<m, 0>(c, v) + std::sin(v)) <
                      eps);
    const auto c2 = [](const double x) { return a * std::sin(x); };
    const auto dc2_dx = computeDerivative<m, 0>(c2, v);
    TFEL_TESTS_ASSERT(std::abs(dc2_dx - a * std::cos(v)) < eps);
    const auto c3 = [&c](const double x) {
      return computeDerivative<m, 0>(c, x);
    };
    const auto dc3_dx = computeDerivative<m, 0>(c3, v);
    TFEL_TESTS_ASSERT(std::abs(dc3_dx + std::cos(v)) < eps);
  }
  // The derivative of a callable returning a math object is stored with the
  // index of the result first and the index of the variable second
  // (`d(i, j) = d\,result(i) / d\,variable(j)`), both in forward and in
  // reverse mode. This is checked here with a non-symmetric Jacobian, which
  // would not be detected with a symmetric derivative (such as the stiffness
  // tensor of a Hooke's material).
  template <tfel::math::enzyme::Mode m>
  void test2() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    using Stensor = stensor<3u, double>;
    using Stensor4 = st2tost2<3u, double>;
    const auto f = [](const Stensor& s) -> Stensor {
      Stensor r = 0.0 * s;
      r(0) = s(1);
      r(1) = 2 * s(3);
      return r;
    };
    const auto s = Stensor{1, 2, 3, 4, 5, 6};
    const auto d = computeDerivative<m, 0>(f, s);
    auto r = eval(0.0 * Stensor4::Id());
    r(0, 1) = 1;
    r(1, 3) = 2;
    TFEL_TESTS_ASSERT(abs(d - r) < eps);
  }
  // regular functions must be passed as template parameters, through the
  // `function` helper
  template <tfel::math::enzyme::Mode m>
  void test3() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    // d(x^3)/dx = 3 x^2
    TFEL_TESTS_ASSERT(
        std::abs(computeDerivative<m, 0>(function<f_cube>, 2.) - 12.) < eps);
  }
  // differentiation of a function of two variables, with respect to one of
  // them only (reverse mode)
  void test4() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    constexpr auto x = double{2};
    constexpr auto y = double{3};
    const auto df_dx = computeDerivative<tfel::math::enzyme::Mode::REVERSE, 0>(
        function<f_x_plus_y2>, x, y);
    TFEL_TESTS_ASSERT(std::abs(df_dx - 1.) < eps);
    const auto df_dy = computeDerivative<tfel::math::enzyme::Mode::REVERSE, 1>(
        function<f_x_plus_y2>, x, y);
    TFEL_TESTS_ASSERT(std::abs(df_dy - 6.) < eps);
  }
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzymeComputeDerivative,
                          "TFELMathEnzymeComputeDerivative");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme-computeDerivative.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}
