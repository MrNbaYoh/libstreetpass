###################
## Build targets ##
###################

add_library(StreetpassIface)
add_library(streetpass::iface ALIAS StreetpassIface)

target_sources(StreetpassIface
    PRIVATE
        ioctl.cpp
        physical.cpp
        streetpass.cpp
        virtual.cpp
    )

target_include_directories(StreetpassIface
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
    )

target_include_directories(StreetpassIface PRIVATE ${CMAKE_SOURCE_DIR}/externals/libtins/include)
target_link_libraries(StreetpassIface PRIVATE tins streetpass::nl80211)
