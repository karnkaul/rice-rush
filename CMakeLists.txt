cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

set(CMAKE_POSITION_INDEPENDENT_CODE ON) # enable PIC
set(CMAKE_DEBUG_POSTFIX "-d")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug) # ensure CMAKE_BUILD_TYPE is not empty
endif()

# project parameters
set(project_name rice-rush)
set(project_version "1.2.0")
set(project_version_name build_version) # name of variable in generated header

project(${project_name} VERSION "${project_version}")
set(is_root_project OFF) # indicate if this is the top-level project

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(is_root_project ON)
  message(STATUS "${PROJECT_NAME} is top level")
endif()

# header text
if(is_root_project)
  set(build_config ${CMAKE_BUILD_TYPE})

  if(CMAKE_CONFIGURATION_TYPES)
    set(build_config "Multi-config")
  endif()

  message(STATUS "[${CMAKE_SYSTEM_PROCESSOR}] [${CMAKE_SYSTEM_NAME}] [${CMAKE_GENERATOR}] [${CMAKE_CXX_COMPILER_ID}] [${build_config}]")
endif()

# version
function(configure_version)
  set(in "${CMAKE_CURRENT_SOURCE_DIR}/src/${project_version_name}.hpp.in")

  if(EXISTS "${in}")
    set(out "${CMAKE_CURRENT_BINARY_DIR}/${project_version_name}.hpp")
    message(STATUS "Configuring ${out}")
    configure_file("${in}" "${out}")
  endif()
endfunction()

configure_version()

if(RR_PREINSTALLED)
  find_package(vulkify CONFIG REQUIRED)
  find_package(capo CONFIG REQUIRED)
else()
  add_subdirectory(ext)
endif()

# exe
add_executable(${PROJECT_NAME})
add_subdirectory(src)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:RR_DEBUG>)
target_include_directories(${PROJECT_NAME} PRIVATE src "${CMAKE_CURRENT_BINARY_DIR}")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} PRIVATE capo::capo vulkify::vulkify)

if(CMAKE_CXX_COMPILER_ID MATCHES Clang OR CMAKE_CXX_COMPILER_ID MATCHES GNU)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror=return-type -Wconversion)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL GNU)
  if(MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE
      /ENTRY:mainCRTStartup # Link to main() and not WinMain()
      /SUBSYSTEM:$<IF:$<CONFIG:Debug>,CONSOLE,WINDOWS> # Spawn a console in Debug
    )
  else()
    target_link_options(${PROJECT_NAME} PRIVATE -Wl,/SUBSYSTEM:$<IF:$<CONFIG:Debug>,CONSOLE,WINDOWS>,/ENTRY:mainCRTStartup)
  endif()
endif()

if(RR_ASAN)
  target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
  target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
endif()
