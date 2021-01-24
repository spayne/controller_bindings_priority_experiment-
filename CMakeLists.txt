cmake_minimum_required (VERSION 3.5.1)

set (CMAKE_CXX_STANDARD 14)


IF (WIN32)
  # set stuff for windows
  add_definitions(-D_WIN32_WINNT=0x0601)
ENDIF()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CMake_FindOpenVR.cmake)
include_directories(${OPENVR_INCLUDE_DIR})

message(STATUS "openvrlibrary is" ${OPENVR_LIBRARY})

add_executable(controller_bindings_test_app
  controller_bindings_test_app.cpp
)

target_link_libraries(controller_bindings_test_app
${OPENVR_LIBRARY}
)

add_library(controller_bindings_test_driver SHARED
 dprintf.h
 dprintf.cpp
  controller_bindings_test_driver.cpp
  skeleton_constants.h
  skeleton_constants.cpp
)
target_link_libraries(controller_bindings_test_driver
${OPENVR_LIBRARY}
)
