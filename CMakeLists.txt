cmake_minimum_required(VERSION 3.1)
project(toyray VERSION 1.0
                  DESCRIPTION "Very naive renderer"
                  LANGUAGES CXX)


IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()


SET(CMAKE_CXX_STANDARD 14)
message(${CMAKE_CXX_FLAGS_RELEASE})


# file(GLOB_RECURSE SRC "src/*.cpp")

add_executable(${PROJECT_NAME} "src/main.cpp")
add_library(saveexr STATIC src/lib/saveexr.cpp)
target_link_libraries(toyray PUBLIC saveexr)
