/*!
 * \file   test-suite/test1/test.cxx
 * \brief
 * \author Thomas Helfer
 * \date   28/06/2024
 */

#include <cmath>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <type_traits>
#include "TFEL/Math/power.hxx"
#include "TFEL/Math/Enzyme/diff.hxx"

#include "TFEL/Tests/TestCase.hxx"
#include "TFEL/Tests/TestProxy.hxx"
#include "TFEL/Tests/TestManager.hxx"

static double f(const double x) { return tfel::math::power<3>(x); }
static double f2(const double x, const double y) {
  return x + tfel::math::power<2>(y);
}

struct TFELMathEnzyme final : public tfel::tests::TestCase {
  TFELMathEnzyme()
      : tfel::tests::TestCase("TFEL/Math/Enzyme", "TFELMathEnzyme") {
  }  // end of TFELMathEnzyme
  tfel::tests::TestResult execute() override {
    this->test1();
    this->test2();
    //     this->test3();
    //     this->test4();
    //     this->test5();
    return this->result;
  }  // end of execute
 private:
  void test1() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    //
    TFEL_TESTS_STATIC_ASSERT(internals::IsFunctionConcept<decltype(f)>);
    // functions of one variable
    TFEL_TESTS_ASSERT(std::abs(diff([](const double x) { return f(x); },
                                    VariableValueAndIncrement<double>{2, 1}) -
                               12) < eps);
    TFEL_TESTS_ASSERT(std::abs(diff([](const double x) { return f(x); },
                                    make_vdv<double>(2, 1)) -
                               12) < eps);
    TFEL_TESTS_ASSERT(std::abs(diff(function<f>, make_vdv<double>(2, 1)) - 12) <
                      eps);
    // functions of two variables
    TFEL_TESTS_ASSERT(
        std::abs(diff([](const double x, const double y) { return f2(x, y); },
                      0, VariableValueAndIncrement<double>{3, 1}) -
                 6) < eps);
    TFEL_TESTS_ASSERT(
        std::abs(diff(function<f2>, 0, make_vdv<double>(3, 1)) - 6) < eps);
  }
  void test2() {
    using namespace tfel::math::enzyme;
    constexpr auto a = double{2.3};
    constexpr auto eps = double{1e-14};
    auto v = VariableValueAndIncrement<double>{1, 1};
    static_assert(internals::isVariableValueAndIncrement<decltype(v)>());
    const auto c = [](const double x) { return std::cos(x); };
    const auto dc_dx = diff(c, v);
    TFEL_TESTS_ASSERT(std::abs(dc_dx + std::sin(v.value)) < eps);
    const auto c2 = [a](const double x) { return a * std::sin(x); };
    const auto dc2_dx = diff(c2, v);
    TFEL_TESTS_ASSERT(std::abs(dc2_dx - a * std::cos(v.value)) < eps);
    const auto c3 = [&c](const double x) {
      return diff(c, VariableValueAndIncrement<double>{x, 1});
    };
    const auto dc3_dx = diff(c3, v);
    TFEL_TESTS_ASSERT(std::abs(dc3_dx + std::cos(v.value)) < eps);
  }
  void test3() {
    //     using namespace tfel::math;
    //     using namespace tfel::math::enzyme;
    //     constexpr auto eps = double{1e-14};
    //     using Stensor = stensor<2u, double>;
    //     const auto s = Stensor{150e6};
    //     const auto ds = diff([](const Stensor& v) { return trace(v); }, s);
    //     TFEL_TESTS_ASSERT(abs(ds - Stensor::Id()) < eps);
    //     const auto ds2 =
    //         diff([](const Stensor& v) -> double { return sigmaeq(v); }, s);
    //     const auto n = eval(3 * deviator(s) / (2 * sigmaeq(s)));
    //     TFEL_TESTS_ASSERT(abs(ds2 - n) < eps);
    //     const auto ds3 =
    //         diff([](const Stensor& v) -> Stensor { return deviator(v); }, s);
    //     constexpr auto M = st2tost2<2u, double>::K();
    //     TFEL_TESTS_ASSERT(abs(ds3 - M) < eps);
  }
  //   void test4() {
  //     using namespace tfel::math;
  //     using namespace tfel::math::enzyme;
  //     constexpr auto eps = double{1e-14};
  //     using Stensor = stensor<3u, double>;
  //     const auto s = Stensor{0, 1, 2, 0, 0, 0};
  //     auto first_eigen_value = [](const Stensor &v) -> double {
  //       const auto vp =
  //       v.computeEigenValues<Stensor::FSESJACOBIEIGENSOLVER>(); return vp(0);
  //     };
  //     const auto dvp = diff(first_eigen_value, s);
  //     const auto [vp, m] =
  //         s.computeEigenVectors<Stensor::FSESJACOBIEIGENSOLVER>();
  //     const auto [n0, n1, n2] = Stensor::computeEigenTensors(m);
  //     TFEL_TESTS_ASSERT(abs(dvp - n0) < eps);
  //     static_cast<void>(vp);
  //     static_cast<void>(n1);
  //     static_cast<void>(n2);
  //   }
  //   void test5() {
  //     using namespace tfel::math;
  //     using namespace tfel::math::enzyme;
  //     using namespace tfel::material;
  //     constexpr auto E = double{70e9};
  //     constexpr auto nu = double{0.3};
  //     constexpr auto lambda = computeLambda(E, nu);
  //     constexpr auto mu = computeMu(E, nu);
  //     constexpr auto eps = double{1e-14};
  //     using Stensor = stensor<3u, double>;
  //     const auto hooke_potential = [lambda, mu](const Stensor &e) {
  //       return (lambda / 2) * power<2>(trace(e)) + mu * (e | e);
  //     };
  //     const auto stress = getCallableDerivative<Stensor>(hooke_potential);
  //     const auto stiffness = getCallableDerivative<Stensor>(stress);
  //     const auto e = Stensor{0.01, 0, 0, 0, 0, 0};
  //     const auto s = stress(e);
  //     const auto K = stiffness(e);
  //   }
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzyme, "TFELMathEnzyme");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}

// double f(const double x, const double y) { return y * y; };

// /* coverity [UNCAUGHT_EXCEPT]*/
// int main() {
//   using namespace tfel::math::enzyme;
//   // test(reinterpret_cast<void*>(f));
//   //   std::cout << __enzyme_fwddiff<double>(reinterpret_cast<void*>(f),
//   //                                         enzyme_const, 0.,
//   //                                         enzyme_dup, 2., 1.) <<
//   std::endl; auto c = [](const double x, const double y) { return f(x, y); };
//   //   double a = 16;
//   //   auto c = [&a](const double x, const double y) { return a * f(x, y); };
//   //
//   std::cout << diff(c, 12, VariableValueAndIncrement<double>(3, 1)) << '\n';
//   //   std::cout << diff(f, 12., VariableValueAndIncrement<double>(3, 1))
//   //             << '\n';
//   //   std::cout << diff(function<f>, 12.,
//   //                            VariableValueAndIncrement<double>(3, 1));
//
//   static_assert(internals::IsFunctionConcept<decltype(f)>);
//   static_assert(!EnzymeCallableConcept<decltype(f)>);
//   static_assert(!EnzymeCallableConcept<decltype(&f)>);
//
//   static_assert(std::same_as<internals::FunctionTraits<decltype(f)>::type,
//                              internals::TypeList<double, double>>);
//
//   return 0;
// }
