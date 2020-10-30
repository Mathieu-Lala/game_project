if(NOT ${CMAKE_BUILD_TYPE} MATCHES Release)
  message(AUTHOR_WARNING "Benchmarking should be done with a release build")
endif()

add_executable(test_engine_benchmark basic.cpp)

target_link_libraries(test_engine_benchmark PUBLIC CONAN_PKG::benchmark)
