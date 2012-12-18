# - Try to find the Sunpinyin libraries
# Once done this will define
#
#  SUNPINYIN_FOUND - system has SUNPINYIN
#  SUNPINYIN_INCLUDE_DIR - the SUNPINYIN include directory
#  SUNPINYIN_LIBRARIES - SUNPINYIN library
#
# Copyright (c) 2012 CSSlayer <wengxt@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(SUNPINYIN_INCLUDE_DIR AND SUNPINYIN_LIBRARIES)
  # Already in cache, be silent
  set(SUNPINYIN_FIND_QUIETLY TRUE)
endif(SUNPINYIN_INCLUDE_DIR AND SUNPINYIN_LIBRARIES)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PC_LIBSUNPINYIN "sunpinyin-2.0>=2.0.4")

find_path(SUNPINYIN_MAIN_INCLUDE_DIR
  NAMES sunpinyin.h
  HINTS ${PC_LIBSUNPINYIN_INCLUDEDIR})

find_library(SUNPINYIN_LIBRARIES
  NAMES sunpinyin
  HINTS ${PC_LIBSUNPINYIN_LIBDIR})

set(SUNPINYIN_INCLUDE_DIR "${SUNPINYIN_MAIN_INCLUDE_DIR}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sunpinyin  DEFAULT_MSG
  SUNPINYIN_LIBRARIES
  SUNPINYIN_MAIN_INCLUDE_DIR)

mark_as_advanced(SUNPINYIN_MAIN_INCLUDE_DIR SUNPINYIN_INCLUDE_DIR
  SUNPINYIN_LIBRARIES)
