/*!
 * \file tests/test1.cxx
 * \author Thomas Helfer
 * \date 20/08/2024
 */

#include <array>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <type_traits>

#include <enzyme/enzyme>
#include "TFEL/Material/StiffnessTensor.hxx"
#include "TFEL/Material/IsotropicModuli.hxx"
#include "TFEL/Material/IsotropicEshelbyTensor.hxx"
#include "TFEL/Material/DerivativesOfLocalisationTensors.hxx"

#include "TFEL/Config/TFELTypes.hxx"

#include "TFEL/Math/power.hxx"
#include "TFEL/Tests/TestCase.hxx"
#include "TFEL/Tests/TestManager.hxx"
#include "TFEL/Tests/TestProxy.hxx"

template <typename T>
static constexpr T my_abs(const T& v) noexcept {
  return v < T(0) ? -v : v;
}


static double f(double x) { return x * x; }
static double f2(const double x) { return pow(x, 3); }
static double f3(const double x, const double y) { return x + y * y; }

struct DerivativesOfHillTensorComponentsTest final : public tfel::tests::TestCase {
  DerivativesOfHillTensorComponentsTest() : tfel::tests::TestCase("TFELMathEnzyme", "DerivativesOfHillTensorComponentsTest") {}

  tfel::tests::TestResult execute() override {
    this->test1<double,false>();
    return this->result;
  }

 private:
 template <typename NumericType, bool use_qt>
  void test1() {

    using stress =
        typename tfel::config::Types<1u, NumericType, use_qt>::stress;
    using real = typename tfel::config::Types<1u, NumericType, use_qt>::real;
    
    using squared_compliance =
        typename tfel::config::Types<1u, NumericType, use_qt>::squared_compliance;

    constexpr auto eps = double{1e-16};


    using namespace tfel::material;
    const auto k0=stress(2);
    const auto mu0 =stress(1);
    const auto ki=stress(200);
    const auto mui =stress(100);
    const auto KG0=KGModuli<stress>(k0,mu0);
    const auto KGi=KGModuli<stress>(ki,mui);
    const std::array<real,2> dkg0 = {1.,0.};
    const double e = 3.5;
    using namespace tfel::material::homogenization::elasticity;
    using namespace tfel::material::homogenization::elasticity::internals;


    //----------------------------- ENZYME Calculation ---------------------------------------------------

    // Derivee par rapport a k0
    KGModuli<stress> d_kg0_dk0{0.0, 0.0};
    d_kg0_dk0.kappa = 1.0;

    const auto dP_dk0 = enzyme::get<0>(
        enzyme::autodiff<enzyme::Forward,
                          enzyme::DuplicatedNoNeed<std::array<double, 6>>>(
            computeHillTensorComponents<double>,
            enzyme::Duplicated<const KGModuli<double>&>{KG0, d_kg0_dk0},
            enzyme::Const<const double&>{e}));

    const auto dP_dk0_ref =
        computeDerivativesOfHillTensorComponents<double>(KG0, e, {1.0, 0.0});

            
    // Derivee par rapport a mu0
    KGModuli<stress> d_kg0_dmu0{0.0, 0.0};
    d_kg0_dmu0.mu = 1.0;

    const auto dP_dmu0 = enzyme::get<0>(
        enzyme::autodiff<enzyme::Forward,
                          enzyme::DuplicatedNoNeed<std::array<double, 6>>>(
            computeHillTensorComponents<double>,
            enzyme::Duplicated<const KGModuli<double>&>{KG0, d_kg0_dmu0},
            enzyme::Const<const double&>{e}));

    const auto dP_dmu0_ref =
        computeDerivativesOfHillTensorComponents<double>(KG0, e, {0.0, 1.0});


    // Comparaison

    for (int i : {0, 1, 2, 3, 4, 5}) {
      TFEL_TESTS_ASSERT(my_abs(dP_dk0[i] - dP_dk0_ref[i]) < eps);
    }

    for (int i : {0, 1, 2, 3, 4, 5}) {
      TFEL_TESTS_ASSERT(my_abs(dP_dmu0[i] - dP_dmu0_ref[i]) < eps);
    }



  }



};

TFEL_TESTS_GENERATE_PROXY(DerivativesOfHillTensorComponentsTest, "DerivativesOfHillTensorComponentsTest");

/* coverity [UNCAUGHT_EXCEPT] */
int main() {
  auto &m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("DerivativesOfHillTensorComponentsTest.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}