add_library(TFELMathEnzyme INTERFACE)
target_include_directories(TFELMathEnzyme
    INTERFACE 
        $<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
