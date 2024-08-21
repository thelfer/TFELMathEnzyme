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

struct TFELMathEnzymeComputeDerivative final : public tfel::tests::TestCase {
  TFELMathEnzymeComputeDerivative()
      : tfel::tests::TestCase("TFEL/Math/Enzyme",
                              "TFELMathEnzymeComputeDerivative") {
  }  // end of TFELMathEnzymeComputeDerivative
  tfel::tests::TestResult execute() override {
    using namespace tfel::math;
    this->test1<tfel::math::enzyme::Mode::FORWARD>();
    this->test1<tfel::math::enzyme::Mode::REVERSE>();
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
