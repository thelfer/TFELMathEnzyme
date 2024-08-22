add_library(TFELMathEnzyme INTERFACE)
target_include_directories(TFELMathEnzyme
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(TFELMathEnzyme INTERFACE
                      ClangEnzymeFlags tfel::TFELMath)
