cmake_minimum_required(VERSION 3.18)

if(UNIX)
  set(build_args -j4)
endif()

function(build_install_ext CONFIG DEST)
  execute_process(COMMAND ${CMAKE_COMMAND} -S ext -B out/ext/${CONFIG} -DCMAKE_BUILD_TYPE=${CONFIG})
  execute_process(COMMAND ${CMAKE_COMMAND} --build out/ext/${CONFIG} -- ${build_args})
  execute_process(COMMAND ${CMAKE_COMMAND} --install out/ext/${CONFIG} --prefix=${DEST})
endfunction()

function(install_ext DEST)
  build_install_ext(Debug "${DEST}")
  build_install_ext(Release "${DEST}")
endfunction()

if("${DEST}" STREQUAL "")
  message(FATAL_ERROR "Missing required arg: DEST")
endif()

install_ext("${DEST}")
message(STATUS "Dependencies installed to ${DEST}")
