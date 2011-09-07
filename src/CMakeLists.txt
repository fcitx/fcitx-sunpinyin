find_package(PkgConfig REQUIRED)

include_directories (${SUNPINYIN_INCLUDE_DIRS})
include_directories (${FCITX4_FCITX_UTILS_INCLUDE_DIRS})
include_directories (${FCITX4_FCITX_CONFIG_INCLUDE_DIRS})
include_directories (${FCITX4_FCITX_INCLUDE_DIRS})

link_directories (${SUNPINYIN_LIBRARY_DIRS}
                  ${FCITX4_FCITX_LIBRARY_DIRS}
                  ${FCITX4_FCITX_CONFIG_LIBRARY_DIRS}
                  ${FCITX4_FCITX_UTILS_LIBRARY_DIRS}
                  )

set( fcitx_sunpinyin_sources
    eim.cpp
    handler.cpp
    config.cpp
    )

add_definitions( -DLOCALEDIR=\"${CMAKE_INSTALL_PREFIX}/share/locale\" )

fcitx_add_addon(fcitx-sunpinyin ${fcitx_sunpinyin_sources} )
target_link_libraries (fcitx-sunpinyin
                       ${SUNPINYIN_LIBRARIES}
                       ${FCITX4_FCITX_CONFIG_LIBRARIES}
                       ${FCITX4_FCITX_UTILS_LIBRARIES}
                       ${FCITX4_FCITX_LIBRARIES}
                       )
