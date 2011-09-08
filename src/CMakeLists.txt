find_package(PkgConfig REQUIRED)

include_directories (
    ${SUNPINYIN_INCLUDE_DIRS}
    ${FCITX4_FCITX_UTILS_INCLUDE_DIRS}
    ${FCITX4_FCITX_CONFIG_INCLUDE_DIRS}
    ${FCITX4_FCITX_INCLUDE_DIRS}
    ${LIBINTL_INCLUDE_DIR}
    )

link_directories (${SUNPINYIN_LIBRARY_DIRS})

set( fcitx_sunpinyin_sources
    eim.cpp
    handler.cpp
    config.cpp
    )

add_definitions( -DLOCALEDIR=\"${CMAKE_INSTALL_PREFIX}/share/locale\" )

fcitx_add_addon(fcitx-sunpinyin ${fcitx_sunpinyin_sources} )
target_link_libraries (fcitx-sunpinyin
                       ${SUNPINYIN_LIBRARIES}
                       ${LIBINTL_LIBRARIES}
                       )
