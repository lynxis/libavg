
INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
)
SET(notworking_SRCS
    CMUCamera.cpp
    CMUCameraUtils.cpp
    DSCamera.cpp
    DSHelper.cpp
    DSSampleGrabber.cpp
)
SET(imaging_SRCS
    Blob.cpp
    Camera.cpp
    CameraInfo.cpp
    CoordTransformer.cpp
    DeDistort.cpp
    FWCamera.cpp
    FWCameraUtils.cpp
    FakeCamera.cpp
    FilterClearBorder.cpp
    FilterDistortion.cpp
    FilterWipeBorder.cpp
    Run.cpp
    TrackerConfig.cpp
    TrackerThread.cpp
    V4LCamera.cpp
    checktracking.cpp
    testimaging.cpp
    trackerconfigdtd.cpp
)

ADD_LIBRARY(imaging STATIC ${imaging_SRCS})
