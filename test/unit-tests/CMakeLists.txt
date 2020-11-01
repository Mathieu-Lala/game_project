if(CONAN_CATCH2_ROOT_DEBUG)
  include(${CONAN_CATCH2_ROOT_DEBUG}/lib/cmake/Catch2/Catch.cmake)
else()
  include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)
endif()

add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main PUBLIC CONAN_PKG::Catch2 project_options project_warnings)

add_executable(unit_tests runtime.cpp)
target_link_libraries(unit_tests PRIVATE catch_main)

catch_discover_tests(unit_tests TEST_PREFIX "unittests." EXTRA_ARGS -s --reporter=xml --out=tests.xml)

add_executable(constexpr_unit_tests compile_time.cpp)
target_link_libraries(constexpr_unit_tests PRIVATE catch_main)

catch_discover_tests(constexpr_unit_tests TEST_PREFIX "constexpr." EXTRA_ARGS -s --reporter=xml --out=constexpr.xml)

add_executable(relaxed_constexpr_unit_tests compile_time.cpp)
target_link_libraries(relaxed_constexpr_unit_tests PRIVATE catch_main)
target_compile_definitions(relaxed_constexpr_unit_tests PRIVATE -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)

catch_discover_tests(relaxed_constexpr_unit_tests TEST_PREFIX "relaxed_constexpr." EXTRA_ARGS -s --reporter=xml
                     --out=relaxed_constexpr.xml)

add_test(NAME unit_tests COMMAND unit_tests)
add_test(NAME constexpr_unit_tests COMMAND constexpr_unit_tests)
add_test(NAME relaxed_constexpr_unit_tests COMMAND relaxed_constexpr_unit_tests)

if(ENABLE_COVERAGE)
  include(${PROJECT_SOURCE_DIR}/cmake/CodeCoverage.cmake)

#  set(EXCLUDE "")
#  foreach(path ${CONAN_INCLUDE_DIRS})
#      list(APPEND EXCLUDE "${path}")
#  endforeach()

  ## FIXME : not working as wanted
  setup_target_for_coverage_lcov(
    NAME generated_coverage # todo : set generated files in a generated folder to keep build dir clean
    EXECUTABLE ctest -j4
    BASE_DIRECTORY ${PROJECT_SOURCE_DIR}
    EXCLUDE /usr/include
    #    EXCLUDE ${CMAKE_BINARY_DIR}/download/
    #    BASE_DIRECTORY ${PROJECT_SOURCE_DIR}
#    EXCLUDE ${EXCLUDE} ${CMAKE_BINARY_DIR}/download/* /usr/include
    NO_DEMANGLE
    DEPENDENCIES unit_tests constexpr_unit_tests relaxed_constexpr_unit_tests
  )
endif()
