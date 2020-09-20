###################
## Build targets ##
###################

add_library(StreetpassCec)
add_library(streetpass::cec ALIAS StreetpassCec)

target_sources(StreetpassCec
    PRIVATE
        module_filter.cpp
        send_mode.cpp
    )

target_include_directories(StreetpassCec
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
    )

target_include_directories(StreetpassCec PRIVATE ${CMAKE_SOURCE_DIR}/externals/libtins/include)
target_link_libraries(StreetpassCec PRIVATE tins)
