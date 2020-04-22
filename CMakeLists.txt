cmake_minimum_required(VERSION 3.11)

######################
## Project settings ##
######################

# Inject relevant directories into CMAKE_MODULE_PATH.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/externals/clangformat-cmake/cmake")

# Declare the project.
project(libstreetpass
    VERSION "0.1.0"
    LANGUAGES CXX
    )

#########################
## CMake configuration ##
#########################

# Set the CMake build type, if this wasn't already done.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose build type." FORCE)
endif()

# Set the C++ standard, if this wasn't already done.
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# Relent on using C++ extensions, except within Cygwin environments.
if(CMAKE_SYSTEM_NAME MATCHES "CYGWIN")
    set(CMAKE_CXX_EXTENSIONS ON)
else()
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

####################
## Subdirectories ##
####################

add_subdirectory(externals)
add_subdirectory(src)
