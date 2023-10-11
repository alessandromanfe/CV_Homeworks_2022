cmake_minimum_required(VERSION 2.8)
project(Lab7)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} src/Lab7.cpp src/panoramic_utils.cpp src/panoramic_utils.h)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
