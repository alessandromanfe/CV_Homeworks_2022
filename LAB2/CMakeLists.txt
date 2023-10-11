cmake_minimum_required(VERSION 2.8)
project(Lab2)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} src/Lab2.cpp src/filters.cpp src/filters.h)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
