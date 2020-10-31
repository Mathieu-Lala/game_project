add_executable(
  app
  src/main.cpp
  src/level/LevelTilemapBuilder.cpp
  src/entity/TileFactory.cpp
  src/level/MapGenerator.cpp
  src/entity/EnemyFactory.cpp
  src/ThePURGE.cpp
  src/GameLogic.cpp
  src/classes/ClassFactory.cpp
  src/DataConfigLoader.cpp
  "src/console/DebugConsole.cpp"
   "include/console/ImGuiConsole.hpp" "src/console/ImGuiConsole.cpp")

target_link_libraries(app PRIVATE engine)

target_include_directories(app PRIVATE include)

# note : maybe not the best solution because the assets might be heavy
add_custom_command(TARGET app POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/out/data)
