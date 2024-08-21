---
title: The TFELMathEnzyme project
author: Thomas Helfer
date: 01/08/2024
lang: en-EN
link-citations: true
colorlinks: true
geometry:
  - margin=2cm
papersize: a4
figPrefixTemplate: "$$i$$"
tblPrefixTemplate: "$$i$$"
secPrefixTemplate: "$$i$$"
eqnPrefixTemplate: "($$i$$)"
---

The `TFELMathEnzyme` project aims at enhancing the [`TFEL/Math`
library](https://thelfer.github.io/tfel/web/tfel-math.html) by adding
support for automatic differentation using the [`Enzyme
AD`](https://enzyme.mit.edu/) project.

> **Disclaimer**
>
> The `TFELMathEnzyme` project is experimental. As it relies on the
> [`enzyme`](https://enzyme.mit.edu/) project, the developpers
> can't provide the same level of guarantees as for the `TFEL` project.

# Overview

The project provides two main functions:

- The `getDerivativeFunction` returns a function computing the first
  and/or higher derivatives of a function with respect to one or more
  variables of a free function or a callable object (like a \(\lambda\)
  function).
- The `computeDerivative` evaluates the derivative of a free function or
  a callable object (like a \(\lambda\) function).

Thanks to `Enzyme AD`, forward mode differentiation and reverse mode
differentiation are avaiable.

# Example of usage

The following snipet shows how to compute the stress and the stiffness
from the free energy associated with the Hooke law:

~~~~{.cxx}
const auto hooke_potential = [](const Stensor& e) {
  return (lambda / 2) * power<2>(trace(e)) + mu * (e | e);
};
const auto stress = getDerivativeFunction<Mode::REVERSE, 0>(hooke_potential);
const auto stiffness = getDerivativeFunction<Mode::REVERSE, 0, 0>(hooke_potential);
~~~~

Index `0` means derivation with respect to the first variable. This
index is repeated to get the second derivative when computing the
stiffness.