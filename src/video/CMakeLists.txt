INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIR}
    ${LIBXML2_INCLUDE_DIR}
)

SET(video_SRCS 
    AsyncVideoDecoder.cpp
    AudioDecoderThread.cpp
    FFMpegDemuxer.cpp
    FFMpegFrameDecoder.cpp
    SyncVideoDecoder.cpp
    VDPAUDecoder.cpp
    VDPAUHelper.cpp
    VideoDecoder.cpp
    VideoDecoderThread.cpp
    VideoDemuxerThread.cpp
    VideoInfo.cpp
    VideoMsg.cpp
)
## testing
# testvideo.cpp

IF(VDPAU)
    SET(video_SRCS ${video_SRCS} VDPAU.cpp AVCCOpaque.cpp)
ENDIF()

ADD_LIBRARY(video STATIC ${video_SRCS})

