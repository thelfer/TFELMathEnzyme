# TFELMathEnzyme

## Overview

`TFELMathEnzyme` is a C++ library that extends the
[`TFEL/Math`](https://thelfer.github.io/tfel/web/tfel-math.html)
library with automatic differentiation capabilities powered by the
[Enzyme project](https://enzyme.mit.edu/). This integration enables
users to compute derivatives of mathematical functions and callables
automatically, leveraging both forward and reverse mode automatic
differentiation.

## Key Features

- **Automatic Differentiation**: Compute derivatives of callables and
    free functions using Enzyme's forward and reverse mode AD.
- **TFEL/Math Integration**: Seamlessly works with TFEL's mathematical
    objects and utilities.
- **Header-Only Library**: Lightweight and easy to integrate into
    existing projects.
- **Modern C++**: Written in C++20 with extensive use of templates,
    concepts, and type traits.
- **Comprehensive Testing**: Includes unit tests for all major
    functionality.

## Core Components

### Main Functionality

1. **`computeDerivative`**: Computes derivatives of callables or free
  functions with respect to specified variables.
   - Supports both forward and reverse mode differentiation.
   - Works with single-argument callables (forward mode) and multi-argument functions.

2. **`fwddiff`**: Computes the increment of a callable given variable values and their increments.
   - Uses Enzyme's forward mode automatic differentiation.
   - Requires at least one `VariableValueAndIncrement` argument.

3. **`Variable`**: Provides utilities for handling variables in differentiation.
   - `VariableConcept`: A concept to constrain variable types.
   - `VariableValueAndIncrement`: A structure to hold variable values and their increments.
   - `make_vdv`: Helper function to create `VariableValueAndIncrement` objects.

4. **Derivative Functions**: Higher-level abstractions for derivative computation.
   - `getDerivativeFunction`: Creates a callable that computes derivatives.
   - `getForwardModeDerivativeFunction`: Forward mode specific derivative function.
   - `getReverseModeDerivativeFunction`: Reverse mode specific derivative function.

### Internal Utilities

- **Type Traits**: Custom type traits for working with Enzyme and TFEL types.
- **Function Utilities**: Utilities for handling function pointers and callables.
- **Enzyme Integration**: Low-level integration with Enzyme's AD capabilities.

## Use Cases

- **Scientific Computing**: Automatic differentiation for numerical simulations.
- **Optimization**: Compute gradients for optimization algorithms.
- **Machine Learning**: Automatic differentiation for training neural networks.
- **Engineering Applications**: Derivative computations in material modeling and finite element analysis.

## Dependencies

- **C++20 Compiler**: Required for modern C++ features.
- **LLVM 18+**: Enzyme requires LLVM 18 or later.
- **Enzyme**: Automatic differentiation library.
- **TFEL/Math**: Mathematical library from the TFEL project.

### Incremental builds

The Enzyme clang plugin provides the `enzyme` headers from an in-memory file
system mounted at `/enzymeroot` and the compiler records these phantom paths
in the dependency files of every compiled translation unit. As those files
never exist on disk, build systems consider the objects always out of date and
recompile everything on every run. `cmake/ScrubEnzymeDeps.cmake`, run after
each test is built, replaces the phantom dependencies by `/dev/null` so that
incremental builds work as expected. Alternatively, exposing Enzyme's include
directory as `/enzymeroot` (for instance with
`sudo ln -s <enzyme-install>/include /enzymeroot`) achieves the same result
for every build tree, system wide.

## Project Structure

```
TFELMathEnzyme/
├── include/                    # Public headers
│   └── TFEL/
│       └── Math/
│           └── Enzyme/        # Main library headers
│               ├── computeDerivative.hxx
│               ├── fwddiff.hxx
│               ├── Variable.hxx
│               ├── getDerivativeFunction.hxx
│               ├── getForwardModeDerivativeFunction.hxx
│               ├── getReverseModeDerivativeFunction.hxx
│               └── Internals/  # Internal utilities
│                   ├── Enzyme.hxx
│                   ├── FunctionUtilities.hxx
│                   ├── IsTemporary.hxx
│                   └── TypeList.hxx
├── src/                       # Library definition (INTERFACE target)
├── tests/                     # Unit tests
│   ├── fwddiff.cxx
│   ├── computeDerivative.cxx
│   ├── computeReverseModeDerivative.cxx
│   ├── getForwardModeDerivativeFunction.cxx
│   └── getDerivativeFunction.cxx
├── CMakeLists.txt             # Main CMake configuration
├── README.md                  # This file
├── INSTALL.md                 # Installation instructions
└── CONTRIBUTING.md            # Contribution guidelines
```

## Quick Start

See [INSTALL.md](INSTALL.md) for detailed installation instructions.

### Basic Example

```cpp
#include <TFEL/Math/Enzyme/computeDerivative.hxx>
#include <TFEL/Math/Enzyme/Variable.hxx>

using namespace tfel::math::enzyme;

// Define a simple function
auto square = [](double x) { return x * x; };

// Compute its derivative
auto derivative = computeDerivative<Mode::FORWARD, 0>(square, 2.0);
// derivative now contains 4.0 (2 * 2.0)
```

## Supported Types and Limitations

- Only statically sized math objects (such as `tfel::math::stensor`,
  `tfel::math::tvector`, `tfel::math::tmatrix`, `tfel::math::st2tost2`, …)
  can be differentiated with respect to, or returned by a differentiated
  callable: the derivative type (`tfel::math::derivative_type`) is only
  defined for objects whose size is known at compile time. Math objects
  whose size is only known at run time (`tfel::math::vector`,
  `tfel::math::matrix`, views, …) and expression templates are rejected by
  the `VariableConcept` concept.
- Reverse mode supports callables with at most two arguments. The result of
  the differentiated callable must be a scalar or a vector like math object
  (an object whose `indexing_policy::arity` is at most 1); the variable with
  respect to which derivatives are computed can be a scalar, a vector like
  (`arity == 1`) or a matrix like (`arity == 2`) math object.
- Forward mode only supports callables of a single scalar or vector like
  variable returning a scalar or a vector like math object: the derivative
  implementation accesses the result with at most two indices. Matrix like
  variables (such as `tfel::math::tmatrix` or `tfel::math::st2tost2`) are
  only supported in reverse mode.
- The `compute*` entry points (`computeDerivative`,
  `computeForwardModeDerivative`, `computeReverseModeDerivative`) compute a
  single derivative per call: in forward mode, the only valid index is 0 and
  it cannot be repeated. Higher order derivatives require the getter
  functions (`getForwardModeDerivativeFunction`, `getDerivativeFunction` in
  forward mode), which accept repeated indices (like `0, 0`) and build
  nested differentiation.
- Differentiating with respect to a unit typed scalar (`tfel::math::qt`) is
  not supported: the generated code crashes the compiler. Unit typed values
  can be used in the body of the differentiated callables and unit typed
  math objects (such as `tfel::math::stensor<3, tfel::math::qt<…>>`) are
  supported, as illustrated by the Hooke's law examples.
- Derivatives of a callable returning a math object are stored with the index
  of the result first and the index of the variable second
  (`d(i, j) = d\,result(i) / d\,variable(j)`), following TFEL's convention
  (a derivative maps increments to increments: `increment_result = derivative
  * increment_variable`).
- The ability of Enzyme to differentiate a callable depends on the code it
  contains: some patterns (such as some uses of `memset` in the generated
  code) may fail to compile, and results may depend on the optimization
  level.

## Documentation

- **API Reference**: Generated using Doxygen (see `docs/` directory).
- **Installation Guide**: See [INSTALL.md](INSTALL.md).
- **Contribution Guidelines**: See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

This project is publicly released under either the **GNU GPL License**
or the **CECILL-A License**. A copy of these licenses is delivered
with the sources of TFEL. CEA or EDF may also distribute this project
under specific licensing conditions.

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines on how to contribute to the project.

## Acknowledgments

- **Enzyme Project**: [https://enzyme.mit.edu/](https://enzyme.mit.edu/)
- **TFEL Project**: [https://thelfer.github.io/tfel/web/](https://thelfer.github.io/tfel/web/tfel-math.html)
- **LLVM Project**: [https://llvm.org/](https://llvm.org/)

## Contact

For questions or support, please contact: [tfel-contact@cea.fr](mailto:tfel-contact@cea.fr)