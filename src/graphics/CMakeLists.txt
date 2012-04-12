FILE(GLOB graphics_SRCS *.cpp)

INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    ${PYTHON_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
    ${GTK2_INCLUDE_DIRS}
)
#    /usr/include/glib-2.0/

#${GTK2_LIBRARIES}
# todo create a good find gdkpxibu
ADD_LIBRARY(graphics STATIC ${graphics_SRCS})
