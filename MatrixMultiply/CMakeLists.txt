cmake_minimum_required(VERSION 3.26)
project(untitled5 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx -mavx2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx -mavx2")

# Your main.c executable
add_executable(untitled5 main.c)

# Your test.cpp executable
add_executable(untitled5_test tes.cpp)

# Link the OpenCV library to your main executable
target_link_libraries(untitled5 ${OpenCV_LIBS})

# Link the OpenCV library to your test executable
target_link_libraries(untitled5_test ${OpenCV_LIBS})

# Include the directories for OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})
