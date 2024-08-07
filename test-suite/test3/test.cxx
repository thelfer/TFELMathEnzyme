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
#include "TFEL/Math/Enzyme/diff2.hxx"

#include "TFEL/Tests/TestCase.hxx"
#include "TFEL/Tests/TestProxy.hxx"
#include "TFEL/Tests/TestManager.hxx"

int enzyme_dup = -2;
int enzyme_const = -1;

double f2(const double x, const double y) { return 2 * x * x + y * y; };

struct TFELMathEnzyme final : public tfel::tests::TestCase {
  TFELMathEnzyme()
      : tfel::tests::TestCase("TFEL/Math/Enzyme", "TFELMathEnzyme3") {
  }  // end of TFELMathEnzyme
  tfel::tests::TestResult execute() override {
    using namespace tfel::math::enzyme;
    //     auto f = [](const double) {};
    //     diff<0>(+f, 12);
    //     diff<0>(f, 12);
    int i = 0;
    auto v = VariableValueAndIncrement<double>{2, 1};
    std::cout << __enzyme_fwddiff<double>(reinterpret_cast<void*>(f2),
                                          enzyme_const, 2., enzyme_dup, v.value, v.increment)
              << std::endl;
    std::cout << "ptr: " << reinterpret_cast<void*>(f2) << std::endl;
    std::cout << "arguments: " << &(v.value) << " " << &(v.increment)
              << std::endl;
    diff(f2, 2., v);
    //    std::cout << diff(f2, 2., v) << std::endl;
    //    print_tuple(transformArguments(TypeList<double, double>{}, 12., v));
    return this->result;
  }  // end of execute
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzyme, "TFELMathEnzyme3");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme-3.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}
