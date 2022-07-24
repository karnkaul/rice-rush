target_sources(${PROJECT_NAME} PRIVATE
  engine/audio.hpp
  engine/basis.hpp
  engine/context.hpp
  engine/delta_time.hpp
  engine/env.cpp
  engine/env.hpp
  engine/framerate.hpp
  engine/instanced.hpp
  engine/keyboard.hpp
  engine/quad.cpp
  engine/quad.hpp
  engine/sprite.cpp
  engine/sprite.hpp
  engine/trigger_renderer.cpp
  engine/trigger_renderer.hpp
  engine/trigger.hpp

  game/consumable.cpp
  game/consumable.hpp
  game/controller.cpp
  game/controller.hpp
  game/cooker_pool.cpp
  game/cooker_pool.hpp
  game/explode.cpp
  game/explode.hpp
  game/game_object.cpp
  game/game_object.hpp
  game/game.cpp
  game/game.hpp
  game/layout.hpp
  game/pawn.hpp
  game/player.cpp
  game/player.hpp
  game/resources.cpp
  game/resources.hpp

  util/collection.hpp
  util/logger.cpp
  util/logger.hpp
  util/ptr.hpp
  util/util.cpp
  util/util.hpp

  main.cpp
)
