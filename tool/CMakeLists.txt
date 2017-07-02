cmake_minimum_required(VERSION 2.8.11)

add_subdirectory(linenoise-ng)

project(fuji_cam_wifi_tool)
file(GLOB fuji_cam_wifi_tool_sources ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)
file(GLOB fuji_cam_wifi_tool_headers ${CMAKE_CURRENT_SOURCE_DIR} src/*.hpp)

include_directories(${PROJECT_SOURCE_DIR}/linenoise-ng/include)

add_executable(fuji_cam_wifi_tool ${fuji_cam_wifi_tool_sources} ${fuji_cam_wifi_tool_headers})
target_link_libraries(fuji_cam_wifi_tool fuji_cam_wifi linenoise)
set_property(TARGET fuji_cam_wifi_tool PROPERTY CXX_STANDARD 11)

find_package(Threads REQUIRED)
target_link_libraries(fuji_cam_wifi_tool ${CMAKE_THREAD_LIBS_INIT})

find_package(OpenCV REQUIRED)
target_link_libraries(fuji_cam_wifi_tool ${OpenCV_LIBS})

if(WIN32)
  target_link_libraries(fuji_cam_wifi_tool wsock32 ws2_32)
endif()
