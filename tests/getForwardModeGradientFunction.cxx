/*!
 * \file   tests/getForwardModeGradientFunction.cxx
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
#include "TFEL/Math/Stensor/StensorConceptIO.hxx"
#include "TFEL/Math/st2tost2.hxx"
#include "TFEL/Math/ST2toST2/ST2toST2ConceptIO.hxx"
#include "TFEL/Math/Enzyme/getForwardModeGradientFunction.hxx"
#include "TFEL/Material/Lame.hxx"

#include "TFEL/Tests/TestCase.hxx"
#include "TFEL/Tests/TestProxy.hxx"
#include "TFEL/Tests/TestManager.hxx"

static double f(const double x) { return tfel::math::power<3>(x); }

struct TFELMathEnzymeGetDerivative final : public tfel::tests::TestCase {
  TFELMathEnzymeGetDerivative()
      : tfel::tests::TestCase("TFEL/Math/Enzyme",
                              "TFELMathEnzymeGetDerivative") {
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
    constexpr auto eps = 1e-14;
    auto f = [](const double x) { return tfel::math::power<3>(x); };
    // first derivative
    auto df = getForwardModeGradientFunction<0>(f);
    TFEL_TESTS_ASSERT(std::abs(df(2) - 12) < eps);
    // second derivative
    auto d2f = getForwardModeGradientFunction<0, 0>(f);
    TFEL_TESTS_ASSERT(std::abs(d2f(1) - 6) < eps);
  }

  void test2() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    using Stensor = stensor<2u, double>;
    const auto s = Stensor{150e6};
    const auto ds = getForwardModeGradientFunction<0>(
        [](const Stensor& v) { return trace(v); });
    TFEL_TESTS_ASSERT(abs(ds(s) - Stensor::Id()) < eps);
    const auto ds2 = getForwardModeGradientFunction<0>(
        [](const Stensor& v) -> double { return sigmaeq(v); });
    const auto n = eval(3 * deviator(s) / (2 * sigmaeq(s)));
    TFEL_TESTS_ASSERT(abs(ds2(s) - n) < eps);
    const auto ds3 = getForwardModeGradientFunction<0>(
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
    const auto stress = getForwardModeGradientFunction<0>(Phi);
    const auto stiffness = getForwardModeGradientFunction<0, 0>(Phi);
    const auto e = double{1e-2};
    const auto s = stress(e);
    const auto K = stiffness(e);
    TFEL_TESTS_ASSERT(abs(s - E * e) < E * eps);
    TFEL_TESTS_ASSERT(abs(K - E) < E * eps);
  }

  // works with -O2
  void test4() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = 1e-14;
    auto E = qt<Stress, float>{150e9};
    auto Phi = [E](const qt<NoUnit, float> e) { return E * e * e / 2; };
    const auto stress = getForwardModeGradientFunction<0>(Phi);
    const auto stiffness = getForwardModeGradientFunction<0>(stress);
    const auto e = qt<NoUnit, float>{1e-2};
    const auto s = stress(e);
    const auto K = stiffness(e);
    static_assert(std::is_same_v<decltype(s), const qt<Stress, float>>);
    static_assert(std::is_same_v<decltype(K), const qt<Stress, float>>);
    //     std::cout << "s: " << s << std::endl;
    //     std::cout << "K: " << K << std::endl;
    TFEL_TESTS_ASSERT(abs(s - E * e) < E * eps);
    TFEL_TESTS_ASSERT(abs(K - E) < E * eps);
  }

  void test5() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    using Stensor = stensor<3u, double>;
    const auto s = Stensor{0, 1, 2, 0, 0, 0};
    auto first_eigen_value = [](const Stensor& v) -> double {
      const auto vp = v.computeEigenValues<Stensor::FSESJACOBIEIGENSOLVER>();
      return vp(0);
    };
    const auto dvp = getForwardModeGradientFunction<0>(first_eigen_value)(s);
    const auto [vp, m] =
        s.computeEigenVectors<Stensor::FSESJACOBIEIGENSOLVER>();
    const auto [n0, n1, n2] = Stensor::computeEigenTensors(m);
    TFEL_TESTS_ASSERT(abs(dvp - n0) < eps);
    static_cast<void>(vp);
    static_cast<void>(n1);
    static_cast<void>(n2);
  }

  // fails to compile with -O2
  void test6() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    using namespace tfel::material;
    constexpr auto E = double{70e9};
    constexpr auto nu = double{0.3};
    constexpr auto lambda = computeLambda(E, nu);
    constexpr auto mu = computeMu(E, nu);
    constexpr auto eps = double{1e-14};
    using Stensor4 = st2tost2<3u, double>;
    using Stensor = stensor<3u, double>;
    const auto hooke_potential = [lambda, mu](const Stensor& e) {
      return (lambda / 2) * power<2>(trace(e)) + mu * (e | e);
    };
    const auto stress = getForwardModeGradientFunction<0>(hooke_potential);
    const auto stiffness =
        getForwardModeGradientFunction<0, 0>(hooke_potential);
    const auto e = Stensor{0.01, 0, 0, 0, 0, 0};
    const auto s = stress(e);
    const auto K = stiffness(e);
    const Stensor4 Kr = lambda * Stensor4::IxI() + 2 * mu * Stensor4::Id();
    TFEL_TESTS_ASSERT(abs(K - Kr) < E * eps);
  }
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzymeGetDerivative,
                          "TFELMathEnzymeGetDerivative");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme-get-derivative.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}
