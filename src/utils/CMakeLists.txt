###################
## Build targets ##
###################

add_library(StreetpassUtils)
add_library(streetpass::utils ALIAS StreetpassUtils)

target_sources(StreetpassUtils
    PRIVATE
        ifioctl.cpp
    )

target_include_directories(StreetpassUtils
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
    )
