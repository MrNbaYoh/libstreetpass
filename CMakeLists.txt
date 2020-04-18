cmake_minimum_required(VERSION 3.11)
project(libstreetpass)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/clangformat-cmake/cmake)

file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/include/*.h ${PROJECT_SOURCE_DIR}/include/*.hpp)

find_package(PkgConfig)
pkg_check_modules(LIBNL REQUIRED libnl-3.0 libnl-genl-3.0)

add_executable(streetpass ${SRCS} ${HEADERS})

target_include_directories(streetpass PUBLIC ${PROJECT_SOURCE_DIR}/include/ ${LIBNL_INCLUDE_DIRS})
target_link_libraries(streetpass ${LIBNL_LIBRARIES})

include(ClangFormat)
target_clangformat_setup(streetpass)
