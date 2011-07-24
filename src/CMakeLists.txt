include(FindPkgConfig)

PKG_CHECK_MODULES(SUNPINYIN REQUIRED "sunpinyin-2.0 >= 2.0" )

include_directories (${SUNPINYIN_INCLUDE_DIRS})
include_directories (${FCITX_CONFIG_INCLUDE_DIRS})
include_directories (${FCITX_INCLUDE_DIRS})

set( fcitx_sunpinyin_sources
    eim.cpp
    handler.cpp
    config.cpp
    )

add_definitions( -DLOCALEDIR=\"${CMAKE_INSTALL_PREFIX}/share/locale\" )

fcitx_add_addon(fcitx-sunpinyin ${fcitx_sunpinyin_sources} )
target_link_libraries (fcitx-sunpinyin ${SUNPINYIN_LIBRARIES})
target_link_libraries (fcitx-sunpinyin ${FCITX4_FCITX_CONFIG_LIBRARIES})
target_link_libraries (fcitx-sunpinyin ${FCITX4_FCITX_LIBRARIES})
