install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
  DESTINATION share/doc/tfel-math-enzyme
  COMPONENT doxygen)
install(FILES
  ${PROJECT_SOURCE_DIR}/docs/doxygen/img/background.svg
  DESTINATION share/doc/tfel-math-enzyme/html/img/
  COMPONENT doxygen)
