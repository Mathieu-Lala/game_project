add_executable(app_unit_tests runtime.cpp)
target_link_libraries(app_unit_tests PRIVATE catch_main ThePURGE)

catch_discover_tests(app_unit_tests TEST_PREFIX "app_unit_tests." EXTRA_ARGS -s --reporter=xml --out=app_unit_tests.xml)

add_executable(app_unit_tests_constexpr compile_time.cpp)
target_link_libraries(app_unit_tests_constexpr PRIVATE catch_main ThePURGE)

catch_discover_tests(app_unit_tests_constexpr TEST_PREFIX "app_unit_tests_constexpr." EXTRA_ARGS -s --reporter=xml
                     --out=app_unit_tests_constexpr.xml)

add_executable(app_unit_tests_constexpr_relaxed compile_time.cpp)
target_link_libraries(app_unit_tests_constexpr_relaxed PRIVATE catch_main ThePURGE)
target_compile_definitions(app_unit_tests_constexpr_relaxed PRIVATE -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)

catch_discover_tests(app_unit_tests_constexpr_relaxed TEST_PREFIX "app_unit_tests_constexpr_relaxed." EXTRA_ARGS -s
                     --reporter=xml --out=app_unit_tests_constexpr_relaxed.xml)
