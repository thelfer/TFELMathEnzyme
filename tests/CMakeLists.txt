function(add_tfel_math_enzyme_test name)
  add_executable(${name}-test ${name}.cxx)
  target_link_libraries(${name}-test PUBLIC TFELMathEnzyme tfel::TFELTests)
  add_test(NAME ${name}-TEST COMMAND ${name}-test)
  # The Enzyme clang plugin serves the `enzyme` headers from an in-memory
  # file system mounted at `/enzymeroot` and the compiler records these
  # phantom paths in the dependency files of the object files it compiles.
  # As those files never exist on disk, the build system would consider the
  # objects always out of date and recompile everything on every run. The
  # following command replaces the phantom dependencies by `/dev/null` so
  # that targets are only recompiled when a real dependency changes.
  add_custom_command(
    TARGET ${name}-test
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND}
      -D
      DIR=${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${name}-test.dir
      -P
      ${PROJECT_SOURCE_DIR}/cmake/ScrubEnzymeDeps.cmake
    VERBATIM)
endfunction()

add_tfel_math_enzyme_test(fwddiff)
add_tfel_math_enzyme_test(Variable)
add_tfel_math_enzyme_test(FunctionUtilities)
add_tfel_math_enzyme_test(computeDerivative)
add_tfel_math_enzyme_test(computeForwardModeDerivative)
add_tfel_math_enzyme_test(computeReverseModeDerivative)
add_tfel_math_enzyme_test(getForwardModeDerivativeFunction)
add_tfel_math_enzyme_test(getReverseModeDerivativeFunction)
add_tfel_math_enzyme_test(getDerivativeFunction)
