add_library(TFELMathEnzyme INTERFACE)
target_include_directories(TFELMathEnzyme
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
if(NOT TFEL_MATH_ENZYME_ENZYME_FLAGS)
  target_link_libraries(TFELMathEnzyme INTERFACE
                        ClangEnzymeFlags tfel::TFELMath)
else(NOT TFEL_MATH_ENZYME_ENZYME_FLAGS)
  # an awful trick to circumvent the fact that
  # ClangEnzymeFlags is not correctly imported
  # on the runner of the github action
  target_link_libraries(TFELMathEnzyme INTERFACE
                        tfel::TFELMath)
  target_compile_options(TFELMathEnzyme PUBLIC
                        ${TFEL_MATH_ENZYME_ENZYME_FLAGS})
endif(NOT TFEL_MATH_ENZYME_ENZYME_FLAGS)
