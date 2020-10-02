add_executable(app src/main.cpp src/Floor.cpp src/Tile.cpp)
target_link_libraries(app PRIVATE engine)

target_include_directories(app PRIVATE include)
