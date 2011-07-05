cmake_minimum_required (VERSION 2.6)

project(fcitx-sunpinyin)

FIND_PACKAGE(Gettext REQUIRED)
FIND_PROGRAM(INTLTOOL_EXTRACT intltool-extract)
FIND_PROGRAM(INTLTOOL_UPDATE intltool-update)
FIND_PROGRAM(INTLTOOL_MERGE intltool-merge)

if(NOT DEFINED LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib)
endif()

set(HAVE_CONFIG_H)

set(libdir ${LIB_INSTALL_DIR})

add_subdirectory(src)
add_subdirectory(data)
add_subdirectory(po)
