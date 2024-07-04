/*!
 * \file   test.cxx
 * \brief
 * \author th202608
 * \date   28/06/2024
 */

#include <cmath>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <type_traits>

#include "TFEL/Math/qt.hxx"
#include "TFEL/Math/Quantity/qtIO.hxx"
#include "TFEL/Math/stensor.hxx"
#include "TFEL/Math/st2tost2.hxx"
#include "TFEL/Math/Vector/tvectorIO.hxx"
#include "TFEL/Math/Stensor/StensorConceptIO.hxx"
#include "TFEL/Math/ST2toST2/ST2toST2ConceptIO.hxx"
#include "TFEL/Math/General/DerivativeType.hxx"
#include "TFEL/Math/General/MathObjectTraits.hxx"
#include "TFEL/Math/Enzyme/diff.hxx"
#include "TFEL/Material/Lame.hxx"

#include "TFEL/Tests/TestCase.hxx"
#include "TFEL/Tests/TestProxy.hxx"
#include "TFEL/Tests/TestManager.hxx"

struct TFELMathEnzyme final : public tfel::tests::TestCase {
  TFELMathEnzyme()
      : tfel::tests::TestCase("TFEL/Math/Enzyme", "TFELMathEnzyme2") {
  }  // end of TFELMathEnzyme
  tfel::tests::TestResult execute() override {
    this->test1();
    this->test2();
    return this->result;
  }  // end of execute
private:
 //! \brief test based on the uniaxial Hooke law without quantities
 void test1() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = 1e-14;
    auto E = double{150e9};
    auto Phi = [E](const auto e) { return E * e * e / 2; };
    const auto stress = getCallableDerivative<double>(Phi);
    const auto stiffness = getCallableDerivative<double>(stress);
    const auto e = double{1e-2};
    const auto s = stress(e);
    const auto K = stiffness(e);
    TFEL_TESTS_ASSERT(abs(s - E * e) < E * eps);
    TFEL_TESTS_ASSERT(abs(K - E) < E * eps);
  }
 //! \brief test based on the uniaxial Hooke law with quantities
  void test2() {
    //     using namespace tfel::math;
    //     using namespace tfel::math::enzyme;
    //     constexpr auto eps = 1e-14;
    //     auto E = qt<Stress, double>{150e9};
    //     auto Phi = [E](const auto e) { return E * e * e / 2; };
    //     const auto stress = getCallableDerivative<qt<NoUnit, double>>(Phi);
    //     const auto stiffness = getCallableDerivative<qt<NoUnit,
    //     double>>(stress); const auto e = qt<NoUnit, double>{1e-2}; const auto
    //     s = stress(e); const auto K = stiffness(e);
    //     static_assert(std::is_same_v<decltype(s), const qt<Stress, double>>);
    //     static_assert(std::is_same_v<decltype(K), const qt<Stress, double>>);
    //     std::cout << "s: " << s << " " << E * e << std::endl;
    //     std::cout << "K: " << K << " " << E  << std::endl;
  }
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzyme, "TFELMathEnzyme2");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme-2.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}
