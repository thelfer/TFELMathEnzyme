set(TFEL_MATH_ENZYME_HEADERS
    TFEL/Math/Enzyme/getForwardModeDerivativeFunction.ixx
    TFEL/Math/Enzyme/Variable.ixx
    TFEL/Math/Enzyme/Internals/TypeList.hxx
    TFEL/Math/Enzyme/Internals/IsTemporary.hxx
    TFEL/Math/Enzyme/Internals/Enzyme.hxx
    TFEL/Math/Enzyme/Internals/FunctionUtilities.hxx
    TFEL/Math/Enzyme/fwddiff.hxx
    TFEL/Math/Enzyme/getReverseModeDerivativeFunction.hxx
    TFEL/Math/Enzyme/Variable.hxx
    TFEL/Math/Enzyme/getForwardModeDerivativeFunction.hxx
    TFEL/Math/Enzyme/getDerivativeFunction.ixx
    TFEL/Math/Enzyme/computeReverseModeDerivative.ixx
    TFEL/Math/Enzyme/computeReverseModeDerivative.hxx
    TFEL/Math/Enzyme/computeForwardModeDerivative.ixx
    TFEL/Math/Enzyme/computeDerivative.hxx
    TFEL/Math/Enzyme/getReverseModeDerivativeFunction.ixx
    TFEL/Math/Enzyme/computeDerivative.ixx
    TFEL/Math/Enzyme/getDerivativeFunction.hxx
    TFEL/Math/Enzyme/fwddiff.ixx
    TFEL/Math/Enzyme/computeForwardModeDerivative.hxx)

foreach(file ${TFEL_MATH_ENZYME_HEADERS})
  get_filename_component(dir ${file} DIRECTORY)
  install(FILES ${file} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${dir} )
endforeach()
