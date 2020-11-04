add_executable(
  app
  src/main.cpp
  src/ThePURGE.cpp
  src/GameLogic.cpp
  src/level/LevelTilemapBuilder.cpp
  src/level/MapGenerator.cpp
  src/factory/EntityFactory.cpp
  src/factory/SpellFactory.cpp
  src/DataConfigLoader.cpp
  src/console/DebugConsole.cpp
  src/console/ConsoleCommands.cpp
  src/models/SpellDatabase.cpp)

target_link_libraries(app PRIVATE engine)

target_include_directories(app PRIVATE include)

# note : maybe not the best solution because the assets might be heavy
add_custom_command(
  TARGET app
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/out/data)
