target_sources(${PROJECT_NAME} PRIVATE
  engine/animated_sprite.cpp
  engine/animated_sprite.hpp
  engine/audio.hpp
  engine/basis.hpp
  engine/config.cpp
  engine/config.hpp
  engine/context.hpp
  engine/delta_time.hpp
  engine/framerate.hpp
  engine/instanced.hpp
  engine/keyboard.hpp
  engine/trigger_renderer.cpp
  engine/trigger_renderer.hpp
  engine/trigger.hpp

  game/background.cpp
  game/background.hpp
  game/controller.cpp
  game/controller.hpp
  game/cooker_pool.cpp
  game/cooker_pool.hpp
  game/director.cpp
  game/director.hpp
  game/explode_pool.cpp
  game/explode_pool.hpp
  game/game_object.cpp
  game/game_object.hpp
  game/game.cpp
  game/game.hpp
  game/hud.cpp
  game/hud.hpp
  game/layout.hpp
  game/player.cpp
  game/player.hpp
  game/powerup.cpp
  game/powerup.hpp
  game/resources.cpp
  game/resources.hpp

  util/collection.hpp
  util/index_timeline.cpp
  util/index_timeline.hpp
  util/logger.cpp
  util/logger.hpp
  util/property.cpp
  util/property.hpp
  util/ptr.hpp
  util/random.hpp
  util/util.cpp
  util/util.hpp

  util/io.cpp
  util/io.hpp

  main.cpp
)
