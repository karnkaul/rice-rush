include(FetchContent)

# capo
FetchContent_Declare(
  capo
  GIT_REPOSITORY https://github.com/capo-devs/capo
  GIT_TAG 40a6c97 # change to version tag after all bug-fixes etc
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/capo"
)
set(CAPO_BUILD_EXAMPLE ON)
FetchContent_MakeAvailable(capo)

# vulkify
FetchContent_Declare(
  vulkify
  GIT_REPOSITORY https://github.com/vulkify/vulkify
  GIT_TAG 747814e # change to version tag after all bug-fixes etc
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/vulkify"
)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(VULKIFY_DEBUG_TRACE ON)
endif()

FetchContent_MakeAvailable(vulkify)
