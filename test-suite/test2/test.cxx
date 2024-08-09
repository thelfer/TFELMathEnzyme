/*!
 * \file   test-suite/test2/test.cxx
 * \brief
 * \author Thomas Helfer
 * \date   28/06/2024
 */

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <type_traits>
#include "TFEL/Math/qt.hxx"
#include "TFEL/Math/power.hxx"
#include "TFEL/Math/stensor.hxx"
#include "TFEL/Math/st2tost2.hxx"
#include "TFEL/Math/Enzyme/getDerivative.hxx"

#include "TFEL/Tests/TestCase.hxx"
#include "TFEL/Tests/TestProxy.hxx"
#include "TFEL/Tests/TestManager.hxx"

static double f(const double x) { return tfel::math::power<3>(x); }

struct TFELMathEnzyme2 final : public tfel::tests::TestCase {
  TFELMathEnzyme2()
      : tfel::tests::TestCase("TFEL/Math/Enzyme", "TFELMathEnzyme2") {
  }  // end of TFELMathEnzyme
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
   auto df = getDerivative<0>(f);
   TFEL_TESTS_ASSERT(std::abs(df(2) - 12) < eps);
   // second derivative
   auto d2f = getDerivative<0, 0>(f);
   TFEL_TESTS_ASSERT(std::abs(d2f(1) - 6) < eps);
 }

 void test2() {
   using namespace tfel::math;
   using namespace tfel::math::enzyme;
   constexpr auto eps = double{1e-14};
   using Stensor = stensor<2u, double>;
   const auto s = Stensor{150e6};
   const auto ds = getDerivative<0>([](const Stensor& v) {
     return trace(v);
   });
   TFEL_TESTS_ASSERT(abs(ds(s) - Stensor::Id()) < eps);
   const auto ds2 =
       getDerivative<0>([](const Stensor& v) -> double { return sigmaeq(v); });
   const auto n = eval(3 * deviator(s) / (2 * sigmaeq(s)));
   TFEL_TESTS_ASSERT(abs(ds2(s) - n) < eps);
   const auto ds3 = getDerivative<0>(
       [](const Stensor& v) -> Stensor { return deviator(v); });
   constexpr auto M = st2tost2<2u, double>::K();
   TFEL_TESTS_ASSERT(abs(ds3(s) - M) < eps);
 }

 void test3() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = 1e-14;
    auto E = double{150e9};
    auto Phi = [E](const double e) { return E * e * e / 2; };
    const auto stress = getDerivative<0>(Phi);
    const auto stiffness = getDerivative<0, 0>(Phi);
    const auto e = double{1e-2};
    const auto s = stress(e);
    const auto K = stiffness(e);
    TFEL_TESTS_ASSERT(abs(s - E * e) < E * eps);
    TFEL_TESTS_ASSERT(abs(K - E) < E * eps);
 }

 void test4() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = 1e-14;
    auto E = qt<Stress, float>{150e9};
    auto Phi = [E](const qt<NoUnit, float> e) { return E * e * e / 2; };
    const auto stress = getDerivative<0>(Phi);
    const auto stiffness = getDerivative<0>(stress);
    const auto e = qt<NoUnit, float>{1e-2};
    const auto s = stress(e);
    const auto K = stiffness(e);
    static_assert(std::is_same_v<decltype(s), const qt<Stress, float>>);
    static_assert(std::is_same_v<decltype(K), const qt<Stress, float>>);
    TFEL_TESTS_ASSERT(abs(s - E * e) < E * eps);
    TFEL_TESTS_ASSERT(abs(K - E) < E * eps);
 }
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzyme2, "TFELMathEnzyme2");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme-2.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}
