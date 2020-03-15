cmake_minimum_required(VERSION 3.9)
project(toyray VERSION 1.0
                  DESCRIPTION "Very naive renderer"
                  LANGUAGES CXX)


IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()


SET(CMAKE_CXX_STANDARD 14)
message("Build type: " ${CMAKE_BUILD_TYPE})


add_executable(${PROJECT_NAME} "src/main.cpp")
add_library(saveexr STATIC src/lib/saveexr.cpp)
target_link_libraries(toyray PUBLIC saveexr)





if(APPLE)
    set(CMAKE_C_COMPILER clang)
    set(CMAKE_CXX_COMPILER clang++)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang\$")
        set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp")
        set(OpenMP_C_LIB_NAMES "omp")
        set(OpenMP_omp_LIBRARY omp)
    endif()
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang\$")
        set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
        set(OpenMP_CXX_LIB_NAMES "omp")
        set(OpenMP_omp_LIBRARY omp)
    endif()
endif()

find_package(OpenMP)

if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
    message("OpenMP installed")
endif()







# link OpenMP if available
# OPTION (USE_OpenMP "Use OpenMP to enamble <omp.h>" ON)


# if(CMAKE_BUILD_TYPE STREQUAL Release)
#     find_package(OpenMP)
#     if(NOT TARGET OpenMP::OpenMP_CXX)
#         add_library(OpenMP_TARGET INTERFACE)
#         add_library(OpenMP::OpenMP_CXX ALIAS OpenMP_TARGET)
#         target_compile_options(OpenMP_TARGET INTERFACE ${OpenMP_CXX_FLAGS})
#         find_package(Threads REQUIRED)
#         target_link_libraries(OpenMP_TARGET INTERFACE Threads::Threads)
#         target_link_libraries(OpenMP_TARGET INTERFACE ${OpenMP_CXX_FLAGS})
#     endif()
#     if(OpenMP_CXX_FOUND)
#         target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
#         message("OpenMP installed")
#     endif()
# endif()




# Find OpenMP
# if(APPLE AND USE_OpenMP)
#     if(CMAKE_C_COMPILER_ID MATCHES "Clang")
#         set(OpenMP_C "${CMAKE_C_COMPILER}")
#         set(OpenMP_C_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
#         set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
#         set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES})
#         set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES})
#         set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES})
#     endif()
#     if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#       set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
#       set(OpenMP_CXX_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
#       set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
#       set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
#       set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
#       set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
#     endif()
# endif()

# if(USE_OpenMP)
#   find_package(OpenMP REQUIRED)
# endif(USE_OpenMP)

# if (OPENMP_FOUND)
#     include_directories("${OPENMP_INCLUDES}")
#     link_directories("${OPENMP_LIBRARIES}")
#     set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#     set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#     # set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# endif(OPENMP_FOUND)




