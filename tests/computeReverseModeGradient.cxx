/*!
 * \file   tests/computeReverseModeGradient.cxx
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
#include "TFEL/Math/Enzyme/computeReverseModeGradient.hxx"

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

template <unsigned short N>
static double mult_by_2(const tfel::math::stensor<2u, double>& s) {
  return (2 * s)(N);
}

static tfel::math::stensor<2u, double> deviator(
    const tfel::math::stensor<2u, double>& s) {
  return ::tfel::math::deviator(s);
}  // end of deviator

struct TFELMathEnzymeComputeGradient final : public tfel::tests::TestCase {
  TFELMathEnzymeComputeGradient()
      : tfel::tests::TestCase("TFEL/Math/Enzyme",
                              "TFELMathEnzymeComputeGradient") {
  }  // end of TFELMathEnzymeComputeGradient
  tfel::tests::TestResult execute() override {
    using namespace tfel::math;
    this->test1();
    this->test2();
    this->test3();
    this->test4();
    //    this->test5<stensor_common::TFELEIGENSOLVER>();
    this->test5<stensor_common::FSESJACOBIEIGENSOLVER>();
    this->test6();
    this->test7();
    return this->result;
  }  // end of execute
 private:
  void test1() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    constexpr auto a = double{2.3};
    const auto v = double{1};
    const auto c = [](const double x) { return std::cos(x); };
    TFEL_TESTS_ASSERT(std::abs(computeReverseModeGradient(c, v) + std::sin(v)) <
                      eps);
    TFEL_TESTS_ASSERT(
        std::abs(computeReverseModeGradient<0>(c, v) + std::sin(v)) < eps);
    const auto c2 = [](const double x) { return a * std::sin(x); };
    const auto dc2_dx = computeReverseModeGradient(c2, v);
    const auto c3 = [&c](const double x) {
      return computeReverseModeGradient(c, x);
    };
    const auto dc3_dx = computeReverseModeGradient(c3, v);
    TFEL_TESTS_ASSERT(std::abs(dc3_dx + std::cos(v)) < eps);
    TFEL_TESTS_ASSERT(std::abs(dc2_dx - a * std::cos(v)) < eps);
    TFEL_TESTS_ASSERT(
        std::abs(computeReverseModeGradient(function<f>, 2) - 12) < eps);
    TFEL_TESTS_ASSERT(
        std::abs(computeReverseModeGradient<0>(function<f>, 2) - 12) < eps);
    TFEL_TESTS_ASSERT(
        std::abs(computeReverseModeGradient<0>(function<f2>, 2, 1) - 1) < eps);
    TFEL_TESTS_ASSERT(
        std::abs(computeReverseModeGradient<1>(function<f2>, 2, 1) - 2) < eps);
    const auto [df_dx, df_dy] =
        computeReverseModeGradient<0, 1>(function<f2>, 2, 1);
    TFEL_TESTS_ASSERT(std::abs(df_dx - 1) < eps);
    TFEL_TESTS_ASSERT(std::abs(df_dy - 2) < eps);
  }
  void test2() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    const auto [df_dx, df_dy] = computeReverseModeGradient(function<f3>, 3, 2);
    TFEL_TESTS_ASSERT(std::abs(df_dx - 2) < eps);
    TFEL_TESTS_ASSERT(std::abs(df_dy - 2.75) < eps);
  }
  void test3() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    constexpr auto id = stensor<2u, double>::Id();
    const auto r = computeReverseModeGradient(function<my_trace>, id);
    TFEL_TESTS_ASSERT(abs(r - id) < eps);
    const auto r2 = computeReverseModeGradient(function<my_trace2>, id);
    TFEL_TESTS_ASSERT(abs(r2 - id) < eps);
    TFEL_TESTS_ASSERT(
        abs(computeReverseModeGradient(function<mult_by_2<1>>, id) -
            stensor<2, double>{0, 2, 0, 0}) < eps);
  }
  void test4() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    constexpr auto id = stensor<2u, double>::Id();
    constexpr auto K = st2tost2<2u, double>::K();
    const auto r = computeReverseModeGradient(function<::deviator>, id);
    TFEL_TESTS_ASSERT(abs(r - K) < eps);
  }
  template <tfel::math::stensor_common::EigenSolver esolver>
  void test5() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    using Stensor = stensor<3u, double>;
    const auto s = Stensor{0, 1, 2, 0, 0, 0};
    auto first_eigen_value = [](const Stensor& v) -> double {
      const auto vp = v.computeEigenValues<esolver>();
      return vp(0);
    };
    const auto dvp = computeReverseModeGradient(first_eigen_value, s);
    const auto [vp, m] = s.computeEigenVectors<esolver>();
    const auto [n0, n1, n2] = Stensor::computeEigenTensors(m);
    TFEL_TESTS_ASSERT(abs(dvp - n0) < eps);
    static_cast<void>(vp);
    static_cast<void>(n1);
    static_cast<void>(n2);
  }
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
    const auto hooke_potential = [](const Stensor& e) {
      return (lambda / 2) * power<2>(trace(e)) + mu * (e | e);
    };
    const auto stress = [hooke_potential](const Stensor& e) {
      return computeReverseModeGradient(hooke_potential, e);
    };
    const auto stiffness = [stress](const Stensor& e) {
      return computeReverseModeGradient(stress, e);
    };
    const auto e = Stensor{0.01, 0, 0, 0, 0, 0};
    const auto K = stiffness(e);
    const Stensor4 Kr = lambda * Stensor4::IxI() + 2 * mu * Stensor4::Id();
    TFEL_TESTS_ASSERT(abs(K - Kr) < E * eps);
  }
  void test7() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    using namespace tfel::material;
    constexpr auto E = qt<Stress, double>{150e9};
    constexpr auto nu = qt<NoUnit, double>{0.3};
    constexpr auto lambda = computeLambda(E, nu);
    constexpr auto mu = computeMu(E, nu);
    constexpr auto eps = double{1e-14};
    using Stensor4 = st2tost2<3u, qt<NoUnit, double>>;
    using Stensor = stensor<3u, qt<NoUnit, double>>;
    const auto hooke_potential = [lambda, mu](const Stensor& e) {
      return (lambda / 2) * power<2>(trace(e)) + mu * (e | e);
    };
    const auto stress = [hooke_potential](const Stensor& e) {
      return computeReverseModeGradient(hooke_potential, e);
    };
    const auto stiffness = [stress](const Stensor& e) {
      return computeReverseModeGradient(stress, e);
    };
    const auto e = Stensor{0.01, 0, 0, 0, 0, 0};
    const auto K = stiffness(e);
    const st2tost2<3u, qt<Stress, double>> Kr =
        lambda * Stensor4::IxI() + 2 * mu * Stensor4::Id();
    TFEL_TESTS_ASSERT(abs(K - Kr) < E * eps);
  }
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzymeComputeGradient,
                          "TFELMathEnzymeComputeGradient");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme-computeReverseModeGradient.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}
