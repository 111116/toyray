cmake_minimum_required(VERSION 3.9)
project(toyray VERSION 1.0
                  DESCRIPTION "Very naive renderer"
                  LANGUAGES CXX)


IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()


SET(CMAKE_CXX_STANDARD 14)
message("Build type: " ${CMAKE_BUILD_TYPE})


add_executable(${PROJECT_NAME} "src/main.cpp" "src/renderer.cpp")
target_include_directories(toyray PUBLIC "src")

add_library(saveexr STATIC "src/lib/saveexr.cpp")
target_link_libraries(toyray PUBLIC saveexr)

add_library(geometry STATIC "src/geometry/geometrycollection.cpp" "src/geometry/aabox.cpp")
target_include_directories(geometry PUBLIC "src")
target_link_libraries(toyray PUBLIC geometry)

add_library(tinyla STATIC "src/math/matfloat.cpp" "src/math/vecfloat.cpp")
target_link_libraries(toyray PUBLIC tinyla)

add_library(util STATIC "src/util/jsonutil.cpp" "src/util/filepath.cpp")
target_include_directories(util PUBLIC "src")
target_link_libraries(toyray PUBLIC util)


find_package(OpenMP)

if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
    message("OpenMP installed")
endif()



