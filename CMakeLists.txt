cmake_minimum_required (VERSION 2.6)

project(fcitx-sunpinyin)

FIND_PACKAGE(Gettext REQUIRED)
FIND_PROGRAM(GETTEXT_XGETTEXT_EXECUTABLE xgettext)

add_subdirectory(src)
add_subdirectory(data)
add_subdirectory(po)
