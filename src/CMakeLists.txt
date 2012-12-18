include_directories(
  ${PROJECT_BINARY_DIR}
  ${SUNPINYIN_INCLUDE_DIR}
  ${FCITX4_FCITX_UTILS_INCLUDE_DIRS}
  ${FCITX4_FCITX_CONFIG_INCLUDE_DIRS}
  ${FCITX4_FCITX_INCLUDE_DIRS}
  ${LIBINTL_INCLUDE_DIR}
  )

link_directories(${SUNPINYIN_LIBRARY_DIRS})

set(fcitx_sunpinyin_sources
  eim.cpp
  handler.cpp
  config.cpp
  )

add_definitions(-DLOCALEDIR=\"${CMAKE_INSTALL_PREFIX}/share/locale\")

fcitx_add_addon_full(sunpinyin
DESC
SOURCES ${fcitx_sunpinyin_sources}
IM_CONFIG sunpinyin.conf
LINK_LIBS ${SUNPINYIN_LIBRARIES} ${LIBINTL_LIBRARIES}
)
