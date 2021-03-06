
SET(graphics_SRCS
    Bitmap.cpp
    BitmapLoader.cpp
    BmpTextureMover.cpp
    Display.cpp
    FBO.cpp
    Filter.cpp
    Filter3x3.cpp
    FilterBandpass.cpp
    FilterBlur.cpp
    FilterDilation.cpp
    FilterErosion.cpp
    FilterFastBandpass.cpp
    FilterFastDownscale.cpp
    FilterGauss.cpp
    FilterGetAlpha.cpp
    FilterHighpass.cpp
    FilterIntensity.cpp
    FilterMask.cpp
    FilterNormalize.cpp
    FilterResizeBilinear.cpp
    FilterResizeGaussian.cpp
    FilterThreshold.cpp
    FilterUnmultiplyAlpha.cpp
    Filtercolorize.cpp
    Filterflip.cpp
    FilterflipX.cpp
    Filterfliprgb.cpp
    Filterfliprgba.cpp
    Filterflipuv.cpp
    Filtergrayscale.cpp
    GLBufferCache.cpp
    GLConfig.cpp
    GLContext.cpp
    GLContextAttribs.cpp
    GLShaderParam.cpp
    GLTexture.cpp
    GLXContext.cpp
    GPUBandpassFilter.cpp
    GPUBlurFilter.cpp
    GPUBrightnessFilter.cpp
    GPUChromaKeyFilter.cpp
    GPUFilter.cpp
    GPUHueSatFilter.cpp
    GPUInvertFilter.cpp
    GPUNullFilter.cpp
    GPURGB2YUVFilter.cpp
    GPUShadowFilter.cpp
    GraphicsTest.cpp
    HistoryPreProcessor.cpp
    ImagingProjection.cpp
    OGLHelper.cpp
    OGLShader.cpp
    PBO.cpp
    Pixel32.cpp
    PixelFormat.cpp
    ShaderRegistry.cpp
    StandardShader.cpp
    SubVertexArray.cpp
    TextureMover.cpp
    VertexArray.cpp
    VertexData.cpp
)

IF(ENABLE_EGL_SUPPORT)
    SET(graphics_SRCS ${graphics_SRCS} CGLContext.cpp EGLContext.cpp WGLContext.cpp)
ENDIF()

## platform specific 
IF(APPLE)
    SET(graphics_SRCS ${graphics_SRCS} AppleDisplay.cpp)
ENDIF()

IF(WINDOWS)
    SET(graphics_SRCS ${graphics_SRCS} WinDisplay.cpp)
ENDIF()

IF(UNIX)
    SET(graphics_SRCS ${graphics_SRCS} X11Display.cpp)
ENDIF()

IF(RASPBERRY)
    SET(graphics_SRCS ${graphics_SRCS} BCMDisplay.cpp)
ENDIF()

##  testing
# benchmarkgraphics.cpp
# testgpu.cpp
# testgraphics.cpp

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
