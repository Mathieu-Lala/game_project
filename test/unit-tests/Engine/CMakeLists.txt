add_executable(engine_unit_tests runtime.cpp)
target_link_libraries(engine_unit_tests PRIVATE catch_main engine_core)

catch_discover_tests(engine_unit_tests TEST_PREFIX "engine_unit_tests." EXTRA_ARGS -s --reporter=xml
                     --out=engine_unit_tests.xml)

add_executable(engine_unit_tests_constexpr compile_time.cpp)
target_link_libraries(engine_unit_tests_constexpr PRIVATE catch_main engine_core)

catch_discover_tests(engine_unit_tests_constexpr TEST_PREFIX "engine_unit_tests_constexpr." EXTRA_ARGS -s
                     --reporter=xml --out=engine_unit_tests_constexpr.xml)

add_executable(engine_unit_tests_constexpr_relaxed compile_time.cpp)
target_link_libraries(engine_unit_tests_constexpr_relaxed PRIVATE catch_main engine_core)
target_compile_definitions(engine_unit_tests_constexpr_relaxed PRIVATE -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)

catch_discover_tests(engine_unit_tests_constexpr_relaxed TEST_PREFIX "engine_unit_tests_constexpr_relaxed." EXTRA_ARGS
                     -s --reporter=xml --out=engine_unit_tests_constexpr_relaxed.xml)
