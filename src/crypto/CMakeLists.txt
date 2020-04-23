###################
## Build targets ##
###################

add_library(StreetpassCrypto)
add_library(streetpass::crypto ALIAS StreetpassCrypto)

target_sources(StreetpassCrypto
    PRIVATE
        crypto.cpp
    )

target_include_directories(StreetpassCrypto
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>
    )

target_include_directories(StreetpassCrypto PUBLIC ${LIBCRYPTOPP_INCLUDE_DIRS})
target_link_libraries(StreetpassCrypto PUBLIC ${LIBCRYPTOPP_LIBRARIES})
