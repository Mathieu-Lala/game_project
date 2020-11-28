add_library(
  ThePURGE STATIC # MODULE
  src/ThePURGE.cpp
  src/GameLogic.cpp
  src/stage/LevelTilemapBuilder.cpp
  src/models/Stage.cpp
  src/models/Spell.cpp
  src/models/Class.cpp
  src/factory/EntityFactory.cpp
  src/factory/SpellFactory.cpp
  src/factory/ParticuleFactory.cpp
  src/widgets/debug/console/DebugConsole.cpp
  src/widgets/debug/console/ConsoleCommands.cpp
  src/widgets/GameHUD.cpp
  src/menu/AMenu.cpp
  src/menu/MainMenu.cpp
  src/menu/UpgradePanel.cpp
  src/menu/GameOver.cpp 
  src/menu/Credits.cpp
  src/menu/HowToPlay.cpp)

target_link_libraries(ThePURGE PRIVATE engine_core) # note : should be engine_api or something ..

target_include_directories(ThePURGE PUBLIC include) # note : should be private

# note : maybe not the best solution because the assets might be heavy
add_custom_command(
  TARGET ThePURGE
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/out/data)
