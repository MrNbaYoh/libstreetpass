#############
## Helpers ##
#############

find_package(PkgConfig)

# A macro to ensure all git submodules are present.
macro(verify_submodules)
    # Read and extract all the modules from .gitmodules.
    file(READ "${CMAKE_SOURCE_DIR}/.gitmodules" gitmodules)
    string(REGEX MATCHALL "path *= *[^\t\r\n]*" modules ${gitmodules})

    # Verify that every module has a .git directory in its respective path,
    # i.e. make sure that the submodule is initialized and present.
    foreach(module ${modules})
        string(REGEX REPLACE "path *= *" "" module ${module})
        if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${module}/.git")
            message(FATAL_ERROR "Dependency ${module} not found. Please run `git submodule update --init --recursive`.")
        endif()
    endforeach()
endmacro()

verify_submodules()

##########################
## Project dependencies ##
##########################

# libnl
if(NOT PkgConfig_FOUND)
    message(FATAL_ERROR "pkg-config is required to find libnl on the system!")
else()
    # Find libnl on the system.
    pkg_check_modules(LIBNL REQUIRED libnl-3.0 libnl-genl-3.0)
endif()
