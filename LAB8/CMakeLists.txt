cmake_minimum_required(VERSION 2.8)
project(Lab8)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} src/Lab8.cpp )
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
