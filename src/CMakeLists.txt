include(ClangFormat)

####################
## Subdirectories ##
####################

add_subdirectory(utils)
add_subdirectory(nl80211)

###################
## Build targets ##
###################

add_executable(Streetpass)

target_sources(Streetpass
    PRIVATE
        main.cpp
    )

target_link_libraries(Streetpass PRIVATE streetpass::nl80211)

target_clangformat_setup(Streetpass)
