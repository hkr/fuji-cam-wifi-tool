cmake_minimum_required(VERSION 2.8.11)

project(fuji_cam_wifi_tool)
file(GLOB_RECURSE fuji_cam_wifi_tool_sources ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file(GLOB_RECURSE fuji_cam_wifi_tool_headers ${CMAKE_CURRENT_SOURCE_DIR} src/*.hpp)

add_executable(fuji_cam_wifi_tool ${fuji_cam_wifi_tool_sources} ${fuji_cam_wifi_tool_headers})
target_link_libraries(fuji_cam_wifi_tool fuji_cam_wifi)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")