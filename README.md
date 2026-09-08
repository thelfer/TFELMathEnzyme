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