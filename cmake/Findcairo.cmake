# - Try to find cairo
# CAIRO_FOUND - system has pango
# CAIRO_LIBRARIES - path to the library
# CAIRO_INCLUDE_DIRS - include dirs
# CAIRO_DEFINITIONS - cflags
# 
# COPYING-CMAKE-SCRIPTS
# Alexander Couzens <lynxis@base45.de>

find_package(PkgConfig)

pkg_check_modules(PC_CAIRO QUIET cairo)

set(libCAIRO_DEFINITIONS ${PC_CAIRO_CFLAGS_OTHER})

find_path(CAIRO_INCLUDE_DIR cairo.h
          HINTS ${PC_CAIRO_INCLUDEDIR} ${PC_CAIRO_INCLUDE_DIRS}
          PATH_SUFFIXES cairo )

find_library(CAIRO_LIBRARY NAMES cairo
             HINTS ${PC_CAIRO_LIBDIR} ${PC_CAIRO_LIBRARY_DIRS} )

set(CAIRO_LIBRARIES ${CAIRO_LIBRARY})
set(CAIRO_INCLUDE_DIRS ${CAIRO_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CAIRO_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(CAIRO  DEFAULT_MSG
                                  CAIRO_LIBRARY CAIRO_INCLUDE_DIR)

mark_as_advanced(CAIRO_INCLUDE_DIR CAIRO_LIBRARY)

