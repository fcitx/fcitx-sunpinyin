include(FindPkgConfig)

PKG_CHECK_MODULES(SUNPINYIN REQUIRED "sunpinyin-2.0 >= 2.0" )
PKG_CHECK_MODULES(FCITX_CONFIG REQUIRED "fcitx-config >= 0.95" )
PKG_CHECK_MODULES(FCITX REQUIRED "fcitx" )

include_directories (${SUNPINYIN_INCLUDE_DIRS})
include_directories (${FCITX_CONFIG_INCLUDE_DIRS})
include_directories (${FCITX_INCLUDE_DIRS})

set( fcitx_sunpinyin_sources
    eim.cpp
    handler.cpp
    config.cpp
    )

add_definitions( ${KDE4_DEFINITIONS} )

add_library( fcitx-sunpinyin MODULE ${fcitx_sunpinyin_sources} )
set_target_properties( fcitx-sunpinyin PROPERTIES PREFIX "")

install(TARGETS fcitx-sunpinyin DESTINATION ${libdir}/fcitx/)

target_link_libraries (fcitx-sunpinyin ${SUNPINYIN_LIBRARIES})
target_link_libraries (fcitx-sunpinyin ${FCITX_CONFIG_LIBRARIES})
target_link_libraries (fcitx-sunpinyin ${FCITX_LIBRARIES})
