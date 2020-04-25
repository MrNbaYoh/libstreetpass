include(ClangFormat)

####################
## Subdirectories ##
####################

add_subdirectory(nl80211)
add_subdirectory(crypto)
add_subdirectory(iface)

###################
## Build targets ##
###################

add_executable(Streetpass)

target_sources(Streetpass
    PRIVATE
        main.cpp
    )

target_link_libraries(Streetpass PRIVATE streetpass::nl80211 streetpass::crypto streetpass::iface)

target_clangformat_setup(Streetpass)
