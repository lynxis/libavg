# - Try to find cairo
# FFMPEG_FOUND - system has pango
# FFMPEG_LIBRARIES - path to the library
# FFMPEG_INCLUDE_DIRS - include dirs
# FFMPEG_DEFINITIONS - cflags
# 
# COPYING-CMAKE-SCRIPTS
# Alexander Couzens <lynxis@base45.de>

find_package(PkgConfig)

pkg_check_modules(PC_FFMPEG QUIET libavcodec)

set(libFFMPEG_DEFINITIONS ${PC_FFMPEG_CFLAGS_OTHER})

find_path(FFMPEG_INCLUDE_DIR cairo.h
          HINTS ${PC_FFMPEG_INCLUDEDIR} ${PC_FFMPEG_INCLUDE_DIRS}
          PATH_SUFFIXES cairo )

find_library(FFMPEG_LIBRARY NAMES cairo
             HINTS ${PC_FFMPEG_LIBDIR} ${PC_FFMPEG_LIBRARY_DIRS} )

set(FFMPEG_LIBRARIES ${FFMPEG_LIBRARY})
set(FFMPEG_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set FFMPEG_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(FFMPEG  DEFAULT_MSG
                                  FFMPEG_LIBRARY FFMPEG_INCLUDE_DIR)

mark_as_advanced(FFMPEG_INCLUDE_DIR FFMPEG_LIBRARY)

