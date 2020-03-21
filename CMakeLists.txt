cmake_minimum_required(VERSION 3.11)
project(libstreetpass)

file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS ${PROJECT_SOURCE_DIR}/include/*.h ${PROJECT_SOURCE_DIR}/include/*.hpp)

add_executable(streetpass ${SRCS} ${HEADERS})

target_include_directories(streetpass PUBLIC ${PROJECT_SOURCE_DIR}/include/)

