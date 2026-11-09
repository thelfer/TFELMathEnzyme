/*!
 * \file   tests/computeReverseModeDerivative.cxx
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
#include "TFEL/Math/tmatrix.hxx"
#include "TFEL/Math/tvector.hxx"
#include "TFEL/Math/Stensor/StensorConceptIO.hxx"
#include "TFEL/Math/st2tost2.hxx"
#include "TFEL/Math/ST2toST2/ST2toST2ConceptIO.hxx"
#include "TFEL/Material/Lame.hxx"
#include "TFEL/Math/Enzyme/computeReverseModeDerivative.hxx"

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

static double f_trace(const tfel::math::stensor<3u, double>& s,
                      const double a) {
  return a * tfel::math::trace(s);
}  // end of f_trace

struct TFELMathEnzymeComputeReversModeDerivative final
    : public tfel::tests::TestCase {
  TFELMathEnzymeComputeReversModeDerivative()
      : tfel::tests::TestCase("TFEL/Math/Enzyme",
                              "TFELMathEnzymeComputeReversModeDerivative") {
  }  // end of TFELMathEnzymeComputeReversModeDerivative
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
    this->test8();
    this->test9();
    this->test10();
    this->test11();
    this->test12();
    return this->result;
  }  // end of execute
 private:
  void test1() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    constexpr auto a = double{2.3};
    const auto v = double{1};
    const auto c = [](const double x) { return std::cos(x); };
    TFEL_TESTS_ASSERT(
        std::abs(computeReverseModeDerivative(c, v) + std::sin(v)) < eps);
    TFEL_TESTS_ASSERT(
        std::abs(computeReverseModeDerivative<0>(c, v) + std::sin(v)) < eps);
    const auto c2 = [](const double x) { return a * std::sin(x); };
    const auto dc2_dx = computeReverseModeDerivative(c2, v);
    TFEL_TESTS_ASSERT(std::abs(dc2_dx - a * std::cos(v)) < eps);
    const auto c3 = [&c](const double x) {
      return computeReverseModeDerivative(c, x);
    };
    const auto dc3_dx = computeReverseModeDerivative(c3, v);
    TFEL_TESTS_ASSERT(std::abs(dc3_dx + std::cos(v)) < eps);
    TFEL_TESTS_ASSERT(
        std::abs(computeReverseModeDerivative(function<f>, 2) - 12) < eps);
    TFEL_TESTS_ASSERT(
        std::abs(computeReverseModeDerivative<0>(function<f>, 2) - 12) < eps);
    TFEL_TESTS_ASSERT(
        std::abs(computeReverseModeDerivative<0>(function<f2>, 2, 1) - 1) <
        eps);
    TFEL_TESTS_ASSERT(
        std::abs(computeReverseModeDerivative<1>(function<f2>, 2, 1) - 2) <
        eps);
    const auto [df_dx, df_dy] =
        computeReverseModeDerivative<0, 1>(function<f2>, 2, 1);
    TFEL_TESTS_ASSERT(std::abs(df_dx - 1) < eps);
    TFEL_TESTS_ASSERT(std::abs(df_dy - 2) < eps);
  }
  void test2() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    const auto [df_dx, df_dy] =
        computeReverseModeDerivative(function<f3>, 3, 2);
    TFEL_TESTS_ASSERT(std::abs(df_dx - 2) < eps);
    TFEL_TESTS_ASSERT(std::abs(df_dy - 2.75) < eps);
  }
  void test3() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    constexpr auto id = stensor<2u, double>::Id();
    const auto r = computeReverseModeDerivative(function<my_trace>, id);
    TFEL_TESTS_ASSERT(abs(r - id) < eps);
    const auto r2 = computeReverseModeDerivative(function<my_trace2>, id);
    TFEL_TESTS_ASSERT(abs(r2 - id) < eps);
    TFEL_TESTS_ASSERT(
        abs(computeReverseModeDerivative(function<mult_by_2<1>>, id) -
            stensor<2, double>{0, 2, 0, 0}) < eps);
  }
  void test4() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    constexpr auto id = stensor<2u, double>::Id();
    constexpr auto K = st2tost2<2u, double>::K();
    const auto r = computeReverseModeDerivative(function<::deviator>, id);
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
    const auto dvp = computeReverseModeDerivative(first_eigen_value, s);
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
      return computeReverseModeDerivative(hooke_potential, e);
    };
    const auto stiffness = [stress](const Stensor& e) {
      return computeReverseModeDerivative(stress, e);
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
    constexpr auto E = qt<unit::Stress, double>{150e9};
    constexpr auto nu = qt<unit::NoUnit, double>{0.3};
    constexpr auto lambda = computeLambda(E, nu);
    constexpr auto mu = computeMu(E, nu);
    constexpr auto eps = double{1e-14};
    using Stensor4 = st2tost2<3u, qt<unit::NoUnit, double>>;
    using Stensor = stensor<3u, qt<unit::NoUnit, double>>;
    const auto hooke_potential = [lambda, mu](const Stensor& e) {
      return (lambda / 2) * power<2>(trace(e)) + mu * (e | e);
    };
    const auto stress = [hooke_potential](const Stensor& e) {
      return computeReverseModeDerivative(hooke_potential, e);
    };
    const auto stiffness = [stress](const Stensor& e) {
      return computeReverseModeDerivative(stress, e);
    };
    const auto e = Stensor{0.01, 0, 0, 0, 0, 0};
    const auto K = stiffness(e);
    const st2tost2<3u, qt<unit::Stress, double>> Kr =
        lambda * Stensor4::IxI() + 2 * mu * Stensor4::Id();
    TFEL_TESTS_ASSERT(abs(K - Kr) < E * eps);
  }
  // differentiation with respect to a scalar argument and a math object
  // argument of the same callable (scalar first)
  void test8() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    using Stensor = stensor<3u, double>;
    const auto s = Stensor{1, 2, 3, 4, 5, 6};
    const auto f = [](const double a, const Stensor& x) {
      return a * trace(x);
    };
    const auto [df_da, df_ds] = computeReverseModeDerivative<0, 1>(f, 3.0, s);
    TFEL_TESTS_ASSERT(std::abs(df_da - trace(s)) < eps);
    TFEL_TESTS_ASSERT(abs(df_ds - 3 * Stensor::Id()) < eps);
    // differentiation with respect to the scalar argument only
    const auto df_da2 = computeReverseModeDerivative<0>(f, 3.0, s);
    TFEL_TESTS_ASSERT(std::abs(df_da2 - trace(s)) < eps);
    // differentiation with respect to the math object argument only
    const auto df_ds2 = computeReverseModeDerivative<1>(f, 3.0, s);
    TFEL_TESTS_ASSERT(abs(df_ds2 - 3 * Stensor::Id()) < eps);
  }
  // differentiation with respect to a math object argument and a scalar
  // argument of the same callable (math object first)
  void test9() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    using Stensor = stensor<3u, double>;
    const auto s = Stensor{1, 2, 3, 4, 5, 6};
    const auto g = [](const Stensor& x, const double a) {
      return a * trace(x);
    };
    const auto [dg_ds, dg_da] = computeReverseModeDerivative<0, 1>(g, s, 3.0);
    TFEL_TESTS_ASSERT(std::abs(dg_da - trace(s)) < eps);
    TFEL_TESTS_ASSERT(abs(dg_ds - 3 * Stensor::Id()) < eps);
  }
  // differentiation with respect to two math object arguments of the same
  // callable
  void test10() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    using Stensor = stensor<3u, double>;
    const auto s_prod = [](const Stensor& u, const Stensor& v) {
      auto r = double{0};
      for (unsigned short i = 0u; i != u.size(); ++i) {
        r += u[i] * v[i];
      }
      return r;
    };
    const auto a = Stensor{1, 2, 3, 4, 5, 6};
    const auto b = Stensor{6, 5, 4, 3, 2, 1};
    // the derivatives with respect to both arguments are computed during a
    // single call
    const auto d = computeReverseModeDerivative<0, 1>(s_prod, a, b);
    TFEL_TESTS_ASSERT(abs(get<0>(d) - b) < eps);
    TFEL_TESTS_ASSERT(abs(get<1>(d) - a) < eps);
    // the derivatives can also be computed separately
    const auto da = computeReverseModeDerivative<0>(s_prod, a, b);
    TFEL_TESTS_ASSERT(abs(da - b) < eps);
    const auto db = computeReverseModeDerivative<1>(s_prod, a, b);
    TFEL_TESTS_ASSERT(abs(db - a) < eps);
  }
  // differentiation with respect to a matrix like math object: the derivative
  // is a three-index object, accessed with three indices
  void test11() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    using TMatrix = tmatrix<3u, 3u, double>;
    using TVector = tvector<3u, double>;
    const auto v = TVector{1., 2., 3.};
    // r(i) = sum_k A(i, k) * v(k)
    const auto apply = [&v](const TMatrix& A) {
      auto r = TVector{};
      for (unsigned short i = 0u; i != 3u; ++i) {
        for (unsigned short k = 0u; k != 3u; ++k) {
          r(i) += A(i, k) * v(k);
        }
      }
      return r;
    };
    const auto a = []() {
      auto m = TMatrix{};
      for (unsigned short i = 0u; i != 3u; ++i) {
        for (unsigned short j = 0u; j != 3u; ++j) {
          m(i, j) = 1. + i + 2. * j;
        }
      }
      return m;
    }();
    const auto d = computeReverseModeDerivative(apply, a);
    for (unsigned short i = 0u; i != 3u; ++i) {
      for (unsigned short j = 0u; j != 3u; ++j) {
        for (unsigned short k = 0u; k != 3u; ++k) {
          const auto expected = (i == j) ? v(k) : 0.;
          TFEL_TESTS_ASSERT(std::abs(d(i, j, k) - expected) < eps);
        }
      }
    }
  }
  // differentiation of a free function with respect to a math object argument
  // and a scalar argument of the same callable
  void test12() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    using Stensor = stensor<3u, double>;
    const auto s = Stensor{1, 2, 3, 4, 5, 6};
    const auto [dg_ds, dg_da] =
        computeReverseModeDerivative<0, 1>(function<f_trace>, s, 3.);
    TFEL_TESTS_ASSERT(abs(dg_ds - 3 * Stensor::Id()) < eps);
    TFEL_TESTS_ASSERT(std::abs(dg_da - trace(s)) < eps);
  }
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzymeComputeReversModeDerivative,
                          "TFELMathEnzymeComputeReversModeDerivative");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme-computeReverseModeDerivative.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}
