cmake_minimum_required(VERSION 3.18)

project(rice-rush-ext)

include(FetchContent)

# capo
FetchContent_Declare(
  capo
  GIT_REPOSITORY https://github.com/capo-devs/capo
  GIT_TAG 1dd0a16 # change to version tag after all bug-fixes etc
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/capo"
)

# vulkify
FetchContent_Declare(
  vulkify
  GIT_REPOSITORY https://github.com/vulkify/vulkify
  GIT_TAG de84733 # change to version tag after all bug-fixes etc
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/vulkify"
)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(VULKIFY_DEBUG_TRACE ON)
endif()

set(KTL_INSTALL ON)
set(CAPO_BUILD_EXAMPLE ON)
set(CAPO_INSTALL ON)
set(VULKIFY_BUILD_EXAMPLES ON)
set(VULKIFY_INSTALL ON)
set(VULKIFY_BUILD_TESTS ON)
FetchContent_MakeAvailable(capo vulkify)
