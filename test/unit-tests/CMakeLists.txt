if(CONAN_CATCH2_ROOT_DEBUG)
  include(${CONAN_CATCH2_ROOT_DEBUG}/lib/cmake/Catch2/Catch.cmake)
else()
  include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)
endif()

add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main PUBLIC CONAN_PKG::Catch2 project_options project_warnings)

add_subdirectory(Engine)
add_subdirectory(Application)

add_test(NAME app_unit_tests COMMAND app_unit_tests)
add_test(NAME app_unit_tests_constexpr COMMAND app_unit_tests_constexpr)
add_test(NAME app_unit_tests_constexpr_relaxed COMMAND app_unit_tests_constexpr_relaxed)

add_test(NAME engine_unit_tests COMMAND engine_unit_tests)
add_test(NAME engine_unit_tests_constexpr COMMAND engine_unit_tests_constexpr)
add_test(NAME engine_unit_tests_constexpr_relaxed COMMAND engine_unit_tests_constexpr_relaxed)

if(ENABLE_COVERAGE)
  include(${PROJECT_SOURCE_DIR}/cmake/CodeCoverage.cmake)

  set(INCLUDE_FOLDER_CONAN)
  foreach(path ${CONAN_INCLUDE_DIRS})
    list(APPEND INCLUDE_FOLDER_CONAN ${path}/*)
  endforeach()

  # cmake-format: off
  setup_target_for_coverage_lcov(
    NAME coverage
    EXECUTABLE ctest -j4
    BASE_DIRECTORY ${PROJECT_SOURCE_DIR}/src/
    EXCLUDE
      "/usr/include/*"
      "/usr/lib/*"
      "${CMAKE_BINARY_DIR}/download/*"
      "${INCLUDE_FOLDER_CONAN}"
      "test/*" # should we exclude the tests from the coverage ?
    NO_DEMANGLE
    DEPENDENCIES
      app_unit_tests
      app_unit_tests_constexpr
      app_unit_tests_constexpr_relaxed
      engine_unit_tests
      engine_unit_tests_constexpr
      engine_unit_tests_constexpr_relaxed)
  # cmake-format: on
endif()
