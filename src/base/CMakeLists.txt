FILE(GLOB base_SRCS *.cpp)

INCLUDE_DIRECTORIES(
    ${PYTHON_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
)

SET(base_SRCS 
Backtrace.cpp
BezierCurve.cpp
CmdLine.cpp
ConfigMgr.cpp
CubicSpline.cpp
DirEntry.cpp
Directory.cpp
Exception.cpp
FileHelper.cpp
GLMHelper.cpp
GeomHelper.cpp
Logger.cpp
MathHelper.cpp
OSHelper.cpp
ObjectCounter.cpp
ProfilingZone.cpp
ProfilingZoneID.cpp
ScopeTimer.cpp
StringHelper.cpp
Test.cpp
TestSuite.cpp
ThreadProfiler.cpp
TimeSource.cpp
Triangle.cpp
UTF8String.cpp
VersionInfo.cpp
WideLine.cpp
XMLHelper.cpp
testbase.cpp
)

ADD_SUBDIRECTORY(triangulate)

ADD_LIBRARY(base STATIC ${base_SRCS})
TARGET_LINK_LIBRARIES(base triangulate)
