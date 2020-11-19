include(CMakeDependentOption)

cmake_dependent_option(ENABLE_TEST_UNIT_TESTS "Enable unit testings" ON "ENABLE_TESTING" OFF)
if(ENABLE_TEST_UNIT_TESTS)
  add_subdirectory(unit-tests)
endif()

cmake_dependent_option(ENABLE_TEST_BENCHMARK "Enable benchmark testings" OFF "ENABLE_TESTING" OFF)
if(ENABLE_TEST_BENCHMARK)
  add_subdirectory(benchmark)
endif()

cmake_dependent_option(ENABLE_TEST_GOLDEN_TESTS "Enable golden testings" OFF "ENABLE_TESTING" OFF)
if(ENABLE_TEST_GOLDEN_TESTS)
  add_subdirectory(golden-tests)
endif()

cmake_dependent_option(ENABLE_TEST_FUZZING "Enable Fuzzing Builds" OFF "ENABLE_TESTING" OFF)
if(ENABLE_TEST_FUZZING)
  add_subdirectory(fuzz-tests)
endif()
