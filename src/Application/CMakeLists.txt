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
  )

target_link_libraries(app PRIVATE engine)

target_include_directories(app PRIVATE include)
