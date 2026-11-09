add_library(TFELMathEnzyme INTERFACE)
add_library(tfel::TFELMathEnzyme ALIAS TFELMathEnzyme)
target_include_directories(TFELMathEnzyme
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(TFELMathEnzyme
                      INTERFACE ClangEnzymeFlags tfel::TFELMath)
# \note The headers require the C++20 standard (concepts, constraints, …)
target_compile_features(TFELMathEnzyme INTERFACE cxx_std_20)

include(CMakePackageConfigHelpers)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/TFELMathEnzymeConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/TFELMathEnzymeConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TFELMathEnzyme)

install(TARGETS TFELMathEnzyme EXPORT TFELMathEnzymeTargets)

install(EXPORT TFELMathEnzymeTargets
  FILE TFELMathEnzymeTargets.cmake
  NAMESPACE tfel::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TFELMathEnzyme)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/TFELMathEnzymeConfig.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/TFELMathEnzyme)
