INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIR}
    ${LIBXML2_INCLUDE_DIR}
)

SET(video_SRCS 
    AsyncDemuxer.cpp
    AsyncVideoDecoder.cpp
    AudioDecoderThread.cpp
    FFMpegDecoder.cpp
    FFMpegDemuxer.cpp
    FrameAge.cpp
    PacketVideoMsg.cpp
    VideoDecoder.cpp
    VideoDecoderThread.cpp
    VideoDemuxerThread.cpp
    VideoInfo.cpp
    VideoMsg.cpp
    testvideo.cpp
)

IF(VDPAU)
    SET(video_SRCS ${video_SRCS} VDPAU.cpp AVCCOpaque.cpp)
ENDIF()

ADD_LIBRARY(video STATIC ${video_SRCS})

