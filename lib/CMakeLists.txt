cmake_minimum_required(VERSION 2.8.11)

project(fuji_cam_wifi)
file(GLOB_RECURSE fuji_cam_wifi_lib_sources ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file(GLOB_RECURSE fuji_cam_wifi_lib_private_headers ${CMAKE_CURRENT_SOURCE_DIR} src/*.hpp)
file(GLOB_RECURSE fuji_cam_wifi_lib_public_headers ${CMAKE_CURRENT_SOURCE_DIR} include/*.hpp)

add_library(fuji_cam_wifi ${fuji_cam_wifi_lib_sources} ${fuji_cam_wifi_lib_private_headers} ${fuji_cam_wifi_lib_public_headers})
target_include_directories(fuji_cam_wifi PUBLIC include PRIVATE src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")