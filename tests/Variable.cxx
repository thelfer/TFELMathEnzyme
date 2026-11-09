/*!
 * \file   tests/Variable.cxx
 * \brief
 * \author Thomas Helfer
 * \date   09/11/2026
 */

#include <cstdlib>
#include <iostream>
#include <type_traits>
#include "TFEL/Math/tmatrix.hxx"
#include "TFEL/Math/tvector.hxx"
#include "TFEL/Math/vector.hxx"
#include "TFEL/Math/matrix.hxx"
#include "TFEL/Math/stensor.hxx"
#include "TFEL/Math/st2tost2.hxx"
#include "TFEL/Math/power.hxx"
#include "TFEL/Math/qt.hxx"
#include "TFEL/Math/Enzyme/Variable.hxx"

#include "TFEL/Tests/TestCase.hxx"
#include "TFEL/Tests/TestProxy.hxx"
#include "TFEL/Tests/TestManager.hxx"

using Stensor = tfel::math::stensor<3u, double>;
using Stensor4 = tfel::math::st2tost2<3u, double>;
using TVector = tfel::math::tvector<3u, double>;
using TMatrix = tfel::math::tmatrix<3u, 3u, double>;
using StrainQT = tfel::math::qt<tfel::math::unit::NoUnit, double>;

// scalars satisfy the `VariableConcept` concept
static_assert(tfel::math::enzyme::VariableConcept<double>);
static_assert(tfel::math::enzyme::VariableConcept<float>);
static_assert(tfel::math::enzyme::VariableConcept<int>);
static_assert(tfel::math::enzyme::VariableConcept<const double>);
static_assert(tfel::math::enzyme::VariableConcept<double&>);
static_assert(tfel::math::enzyme::VariableConcept<const double&>);
// unit typed scalars satisfy the `VariableConcept` concept
static_assert(tfel::math::enzyme::VariableConcept<StrainQT>);
// statically sized math objects satisfy the `VariableConcept` concept
static_assert(tfel::math::enzyme::VariableConcept<Stensor>);
static_assert(tfel::math::enzyme::VariableConcept<Stensor4>);
static_assert(tfel::math::enzyme::VariableConcept<TVector>);
static_assert(tfel::math::enzyme::VariableConcept<TMatrix>);
static_assert(
    tfel::math::enzyme::VariableConcept<tfel::math::stensor<3u, StrainQT>>);
// math objects whose size is only known at run time do not satisfy the
// `VariableConcept` concept: the type of their derivative is not defined
static_assert(!tfel::math::enzyme::VariableConcept<tfel::math::vector<double>>);
static_assert(!tfel::math::enzyme::VariableConcept<tfel::math::matrix<double>>);
static_assert(
    !tfel::math::enzyme::VariableConcept<const tfel::math::vector<double>&>);
// expression templates do not satisfy the `VariableConcept` concept
static_assert(!tfel::math::enzyme::VariableConcept<decltype(2 * Stensor{})>);

// `FixedSizeConcept` only accepts statically sized math objects
static_assert(tfel::math::enzyme::FixedSizeConcept<Stensor>);
static_assert(tfel::math::enzyme::FixedSizeConcept<Stensor4>);
static_assert(tfel::math::enzyme::FixedSizeConcept<TMatrix>);
static_assert(!tfel::math::enzyme::FixedSizeConcept<double>);
static_assert(
    !tfel::math::enzyme::FixedSizeConcept<tfel::math::vector<double>>);
static_assert(
    !tfel::math::enzyme::FixedSizeConcept<tfel::math::matrix<double>>);

// `getArity`
static_assert(tfel::math::enzyme::internals::getArity<double>() == 0u);
static_assert(tfel::math::enzyme::internals::getArity<StrainQT>() == 0u);
static_assert(tfel::math::enzyme::internals::getArity<const Stensor&>() == 1u);
static_assert(tfel::math::enzyme::internals::getArity<TVector>() == 1u);
static_assert(tfel::math::enzyme::internals::getArity<const TMatrix&>() == 2u);
static_assert(tfel::math::enzyme::internals::getArity<Stensor4>() == 2u);

// `isVariableValueAndIncrement`
using VDV = tfel::math::enzyme::VariableValueAndIncrement<double>;
static_assert(
    tfel::math::enzyme::internals::isVariableValueAndIncrement<VDV>());
static_assert(
    tfel::math::enzyme::internals::isVariableValueAndIncrement<const VDV&>());
static_assert(
    !tfel::math::enzyme::internals::isVariableValueAndIncrement<double>());
static_assert(
    !tfel::math::enzyme::internals::isVariableValueAndIncrement<Stensor>());

// `countNumberOfVariableValueAndIncrement`
static_assert(
    tfel::math::enzyme::internals::countNumberOfVariableValueAndIncrement<
        VDV>() == 1u);
static_assert(tfel::math::enzyme::internals::
                  countNumberOfVariableValueAndIncrement<double, VDV>() == 1u);
static_assert(tfel::math::enzyme::internals::
                  countNumberOfVariableValueAndIncrement<VDV, VDV>() == 2u);
static_assert(tfel::math::enzyme::internals::
                  countNumberOfVariableValueAndIncrement<double, int>() == 0u);

struct TFELMathEnzymeVariable final : public tfel::tests::TestCase {
  TFELMathEnzymeVariable()
      : tfel::tests::TestCase("TFEL/Math/Enzyme", "TFELMathEnzymeVariable") {
  }  // end of TFELMathEnzymeVariable
  tfel::tests::TestResult execute() override {
    this->test1();
    this->test2();
    return this->result;
  }  // end of execute
 private:
  // `VariableValueAndIncrement` of a scalar
  void test1() {
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    const auto a = VariableValueAndIncrement<double>{2., 3.};
    TFEL_TESTS_ASSERT(std::abs(a.value - 2.) < eps);
    TFEL_TESTS_ASSERT(std::abs(a.increment - 3.) < eps);
    const auto b = make_vdv<double>(1., 2.);
    TFEL_TESTS_ASSERT((std::is_same_v<std::remove_cvref_t<decltype(b)>,
                                      VariableValueAndIncrement<double>>));
    TFEL_TESTS_ASSERT(std::abs(b.value - 1.) < eps);
    TFEL_TESTS_ASSERT(std::abs(b.increment - 2.) < eps);
    // conversion from a compatible type is allowed
    const auto c = make_vdv<double>(1.f, 2.f);
    TFEL_TESTS_ASSERT(std::abs(c.value - 1.) < eps);
  }
  // `VariableValueAndIncrement` of a math object
  void test2() {
    using namespace tfel::math;
    using namespace tfel::math::enzyme;
    constexpr auto eps = double{1e-14};
    const auto s = Stensor{1, 2, 3, 4, 5, 6};
    // copy construction from lvalues
    const auto a = make_vdv<Stensor>(s, s);
    TFEL_TESTS_ASSERT(abs(a.value - s) < eps);
    TFEL_TESTS_ASSERT(abs(a.increment - s) < eps);
    // construction from rvalues
    const auto b = make_vdv<Stensor>(Stensor{1, 2, 3, 4, 5, 6}, Stensor{});
    TFEL_TESTS_ASSERT(abs(b.increment) < eps);
    // the increment of a math object can be initialized with an expression
    const auto c = VariableValueAndIncrement<Stensor>{s, 2 * s};
    TFEL_TESTS_ASSERT(abs(c.increment - 2 * s) < eps);
    // unit typed math objects
    using QS = stensor<3u, StrainQT>;
    const auto q = QS{StrainQT{1}};
    const auto d = make_vdv<QS>(q, QS{StrainQT{2}});
    TFEL_TESTS_ASSERT(abs(d.value(0) - StrainQT{1}) < StrainQT{eps});
    TFEL_TESTS_ASSERT(abs(d.increment(0) - StrainQT{2}) < StrainQT{eps});
  }
};

TFEL_TESTS_GENERATE_PROXY(TFELMathEnzymeVariable, "TFELMathEnzymeVariable");

/* coverity [UNCAUGHT_EXCEPT]*/
int main() {
  auto& m = tfel::tests::TestManager::getTestManager();
  m.addTestOutput(std::cout);
  m.addXMLTestOutput("tfel-math-enzyme-variable.xml");
  return m.execute().success() ? EXIT_SUCCESS : EXIT_FAILURE;
}
