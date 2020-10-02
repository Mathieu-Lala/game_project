add_executable(app src/main.cpp
                    src/level/LevelTilemapBuilder.cpp
                    src/level/TileFactory.cpp
                    src/level/MapGenerator.cpp)

target_link_libraries(app PRIVATE engine)

target_include_directories(app PRIVATE include)
