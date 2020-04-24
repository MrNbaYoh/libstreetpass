###################
## Build targets ##
###################

add_library(StreetpassIface)
add_library(streetpass::iface ALIAS StreetpassIface)

target_sources(StreetpassIface
    PRIVATE
        interface.cpp
    )

target_include_directories(StreetpassIface
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
    )

target_link_libraries(StreetpassIface PRIVATE streetpass::nl80211 streetpass::utils)
