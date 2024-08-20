/*!
 * \file   tests/getGradientFunction.cxx
 * \brief    
 * \author Thomas Helfer
 * \date   20/08/2024
 */
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <type_traits>
#include "TFEL/Math/qt.hxx"
#include "TFEL/Math/power.hxx"
#include "TFEL/Math/Enzyme/getGradientFunction.hxx"

#include "TFEL/Tests/TestCase.hxx"
#include "TFEL/Tests/TestProxy.hxx"
#include "TFEL/Tests/TestManager.hxx"

struct TFELMathEnzymeGetGradientFunction final : public tfel::tests::TestCase {
  TFELMathEnzymeGetGradientFunction()
      : tfel::tests::TestCase("TFEL/Math/Enzyme",
                              "TFELMathEnzymeGetGradientFunction") {
  }  // end of TFELMathEnzyme
  tfel::tests::TestResult execute() override {
    this->test1<tfel::math::enzyme::Mode::REVERSE>();
    this->test1<tfel::math::enzyme::Mode::FORWARD>();
    return this->result;
  }  // end of execute
 private:
  template <tfel::math::enzyme::Mode m>
  void test1() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = 1e-14;
    auto f = [](const double x) { return tfel::math::power<3>(x); };
    // first derivative
    auto df = getGradientFunction<m, 0>(f);
    TFEL_TESTS_ASSERT(std::abs(df(2) - 12) < eps);
    // second derivative
    auto d2f = getGradientFunction<m, 0, 0>(f);
    TFEL_TESTS_ASSERT(std::abs(d2f(1) - 6) < eps);
  }
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzymeGetGradientFunction,
                          "TFELMathEnzymeGetGradientFunction");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme-getGradientFunction.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}
