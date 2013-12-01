cmake_minimum_required(VERSION 2.8)

PROJECT(libavg)

OPTION(AVG_ENABLE_MTDEV "enable multitouch library" OFF)
OPTION(AVG_ENABLE_MACTRACKPAD "enable support for mac trackpads" OFF)
OPTION(AVG_ENABLE_WIN7TOUCH "enable support for Win7 input devices" OFF)
OPTION(AVG_ENABLE_XINPUTMT "enable support for Xinput MT devices" OFF)
OPTION(AVG_ENABLE_VDPAU "enable support for VDPAU" OFF)
OPTION(AVG_ENABLE_V4L2 "enable support for V4L2 devices" OFF)
OPTION(AVG_ENABLE_1394_2 "enable support for firewire / 1394 devices" OFF)
OPTION(RASPBERRY "enable support for raspberry specific graphic driver (broadcom)" OFF)
OPTION(HAVE_APPLE_OPENGL_FRAMEWORK "enable support for VDPAU devices TODO DESCR" OFF)

SET(CMAKE_INSTALL_PREFIX /usr/local)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_HOME_DIRECTORY}/cmake
                      )


ADD_DEFINITIONS(-O3 -ffast-math -Wall -pipe -Wno-invalid-offsetof -Woverloaded-virtual -Wswitch -Wuninitialized -Wempty-body)
SET(CMAKE_FIND_FRAMEWORK LAST)

SET(Boost_USE_STATIC_LIBS OFF)
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS
                thread
                python
             REQUIRED)

# Use Python_ADDITIONAL_VERSIONS
SET(Python_ADDITIONAL_VERSIONS 2.7)
FIND_PACKAGE(PythonLibs REQUIRED)
FIND_PACKAGE(LibXml2 REQUIRED)
FIND_PACKAGE(GTK2 REQUIRED gtk)
FIND_PACKAGE(Freetype REQUIRED)
FIND_PACKAGE(RSVG REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(FFmpeg COMPONENTS AVCODEC AVFORMAT SWSCALE REQUIRED)

# Name of package
SET(PACKAGE "libavg")
# Define to the address where bug reports for this package should be sent.
SET(PACKAGE_BUGREPORT "uzadow@libavg.de")
# Define to the full name of this package.
SET(PACKAGE_NAME "libavg")
# Define to the full name and version of this package.
SET(PACKAGE_STRING "libavg 1.7.1")
# Define to the one symbol short name of this package.
SET(PACKAGE_TARNAME "libavg")
# Define to the home page for this package.
SET(PACKAGE_URL "")
# Define to the version of this package.
SET(PACKAGE_VERSION "1.7.1")
# Define VERSION
SET(VERSION "1.7.1")


## TODO!!!
SET(STDC_HEADERS 1)
UNSET(HAVE_XI2_1)
UNSET(HAVE_XI2_2)
UNSET(PTHREAD_CREATE_JOINABLE)
SET(HAVE_NAMESPACES)
SET(HAVE_BOOST_THREAD)
UNSET(HAVE_APPLE_OPENGL_FRAMEWORK)

IF(CYGWIN)
    SET(WINDOWS)
ENDIF()

IF(MSVC)
    SET(WINDOWS)
ENDIF()

ADD_SUBDIRECTORY(src)
