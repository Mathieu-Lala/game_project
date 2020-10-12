add_executable(app src/main.cpp src/level/LevelTilemapBuilder.cpp
                   src/level/TileFactory.cpp src/level/MapGenerator.cpp include/Competences/FarmerCompetences.hpp)

target_link_libraries(app PRIVATE engine)

target_include_directories(app PRIVATE include)
