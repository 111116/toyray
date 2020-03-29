cmake_minimum_required(VERSION 3.9)
project(toyray VERSION 1.0
                  DESCRIPTION "Very naive renderer"
                  LANGUAGES CXX)


IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()


SET(CMAKE_CXX_STANDARD 14)
message("Build type: " ${CMAKE_BUILD_TYPE})
set(CMAKE_CXX_FLAGS_DEBUG "-DSINGLE_THREADED")


add_executable(${PROJECT_NAME} "src/main.cpp" "src/renderer.cpp")
target_include_directories(toyray PUBLIC "src")

add_library(imgfmt STATIC "src/lib/stb_image.cpp" "src/lib/stb_image_write.cpp" "src/lib/exr_io.cpp" "src/lib/writebmp.cpp")
target_link_libraries(toyray PUBLIC imgfmt)

add_library(geometry STATIC "src/geometry/geometrycollection.cpp" "src/geometry/aabox.cpp")
target_include_directories(geometry PUBLIC "src")
target_link_libraries(toyray PUBLIC geometry)

add_library(tinyla STATIC "src/math/matfloat.cpp" "src/math/vecfloat.cpp")
target_link_libraries(toyray PUBLIC tinyla)

add_library(util STATIC "src/util/jsonutil.cpp" "src/util/filepath.cpp" "src/util/transformparser.cpp" "src/util/taskscheduler.cpp")
target_include_directories(util PUBLIC "src")
target_link_libraries(toyray PUBLIC util)

add_library(lights STATIC "src/lights/lightcollection.cpp")
target_include_directories(lights PUBLIC "src")
target_link_libraries(toyray PUBLIC lights)

add_library(material STATIC "src/bsdfs/bsdf.cpp" "src/bsdfs/materials.cpp")
target_include_directories(material PUBLIC "src")
target_link_libraries(toyray PUBLIC material)

add_library(camera STATIC "src/cameras/cameracollection.cpp")
target_include_directories(camera PUBLIC "src")
target_link_libraries(toyray PUBLIC camera)


find_package(Threads REQUIRED)
target_link_libraries(toyray PUBLIC pthread)


