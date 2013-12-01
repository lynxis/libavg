FILE(GLOB audio_SRCS *.cpp)

SET(audio_SRCS
    AudioBuffer.cpp
    AudioEngine.cpp
    AudioMsg.cpp
    AudioParams.cpp
    AudioSource.cpp
    testlimiter.cpp
)

INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    ${PYTHON_INCLUDE_DIRS}
)

ADD_LIBRARY(audio STATIC ${audio_SRCS})
