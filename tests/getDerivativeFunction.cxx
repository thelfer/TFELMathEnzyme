/*!
 * \file   tests/getDerivativeFunction.cxx
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
#include "TFEL/Math/stensor.hxx"
#include "TFEL/Math/st2tost2.hxx"
#include "TFEL/Math/Enzyme/getDerivativeFunction.hxx"
#include "TFEL/Material/Lame.hxx"

#include "TFEL/Tests/TestCase.hxx"
#include "TFEL/Tests/TestProxy.hxx"
#include "TFEL/Tests/TestManager.hxx"

struct TFELMathEnzymeGetDerivativeFunction final : public tfel::tests::TestCase {
  TFELMathEnzymeGetDerivativeFunction()
      : tfel::tests::TestCase("TFEL/Math/Enzyme",
                              "TFELMathEnzymeGetDerivativeFunction") {
  }  // end of TFELMathEnzyme
  tfel::tests::TestResult execute() override {
    this->test1<tfel::math::enzyme::Mode::REVERSE>();
    this->test1<tfel::math::enzyme::Mode::FORWARD>();
    this->test2<tfel::math::enzyme::Mode::REVERSE>();
    this->test2<tfel::math::enzyme::Mode::FORWARD>();
    return this->result;
  }  // end of execute
 private:
  template <tfel::math::enzyme::Mode m>
  void test1() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = 1e-14;
    auto f = [](const double x) { return tfel::math::power<3>(x); };
    // first derivative
    auto df = getDerivativeFunction<m, 0>(f);
    TFEL_TESTS_ASSERT(std::abs(df(2) - 12) < eps);
    // second derivative
    auto d2f = getDerivativeFunction<m, 0, 0>(f);
    TFEL_TESTS_ASSERT(std::abs(d2f(1) - 6) < eps);
  }
  template <tfel::math::enzyme::Mode m>
  void test2() {
    using namespace tfel::math;
    using namespace tfel::material;
    using namespace tfel::math::enzyme;
    using Stensor = stensor<2u, double>;
    using Stensor4 = st2tost2<2u, double>;
    constexpr auto eps = double{1e-14};
    constexpr auto E = double{70e9};
    constexpr auto nu = double{0.3};
    constexpr auto lambda = computeLambda(E, nu);
    constexpr auto mu = computeMu(E, nu);
    const auto hooke_potential = [](const Stensor& e) {
      return (lambda / 2) * power<2>(trace(e)) + mu * (e | e);
    };
    const auto stress = getDerivativeFunction<m, 0>(hooke_potential);
    const auto stiffness = getDerivativeFunction<m, 0, 0>(hooke_potential);
    const auto e = Stensor{0.01, 0, 0, 0};
    const auto s = stress(e);
    const auto s_ref = eval(2 * mu * e + lambda * trace(e) * Stensor::Id());
    TFEL_TESTS_ASSERT(abs(s - s_ref) < eps * E);
    const auto K = stiffness(e);
    const auto K_ref = eval(2 * mu * Stensor4::Id() + lambda * Stensor4::IxI());
    TFEL_TESTS_ASSERT(abs(K - K_ref) < eps * E);
  }
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzymeGetDerivativeFunction,
                          "TFELMathEnzymeGetDerivativeFunction");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme-getDerivativeFunction.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}
