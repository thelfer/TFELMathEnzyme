add_library(TFELMathEnzyme INTERFACE)
add_library(tfel::TFELMathEnzyme ALIAS TFELMathEnzyme)
target_include_directories(TFELMathEnzyme
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(TFELMathEnzyme
                      INTERFACE ClangEnzymeFlags tfel::TFELMath)
