###################
## Build targets ##
###################

add_library(StreetpassNl80211)
add_library(streetpass::nl80211 ALIAS StreetpassNl80211)

target_sources(StreetpassNl80211
    PRIVATE
        commands.cpp
        error.cpp
        message.cpp
        socket.cpp
    )

target_include_directories(StreetpassNl80211
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
    )

target_include_directories(StreetpassNl80211 PUBLIC ${LIBNL_INCLUDE_DIRS})
target_link_libraries(StreetpassNl80211 PUBLIC ${LIBNL_LIBRARIES})
