
INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    ${PYTHON_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIRS}
    ${RSVG_INCLUDE_DIRS}
    ${GTK2_INCLUDE_DIRS}
)


SET(player_SRCS
    AVGNode.cpp
    AreaNode.cpp
    Arg.cpp
    ArgBase.cpp
    ArgList.cpp
    BlurFXNode.cpp
    CameraNode.cpp
    Canvas.cpp
    CanvasNode.cpp
    ChromaKeyFXNode.cpp
    CircleNode.cpp
    Contact.cpp
    CursorEvent.cpp
    CursorState.cpp
    CurveNode.cpp
    DisplayEngine.cpp
    DisplayParams.cpp
    DivNode.cpp
    Event.cpp
    EventDispatcher.cpp
    FXNode.cpp
    FilledVectorNode.cpp
    HueSatFXNode.cpp
    Image.cpp
    ImageNode.cpp
    InvertFXNode.cpp
    KeyEvent.cpp
    LineNode.cpp
    MainCanvas.cpp
    MaterialInfo.cpp
    MeshNode.cpp
    MouseEvent.cpp
    MultitouchInputDevice.cpp
    Node.cpp
    NodeDefinition.cpp
    NodeRegistry.cpp
    NullFXNode.cpp
    OGLSurface.cpp
    OffscreenCanvas.cpp
    OffscreenCanvasNode.cpp
    Player.cpp
    PluginManager.cpp
    PolyLineNode.cpp
    PolygonNode.cpp
    RasterNode.cpp
    RectNode.cpp
    SDLDisplayEngine.cpp
    SVG.cpp
    SVGElement.cpp
    ShadowFXNode.cpp
    Shape.cpp
    SoundNode.cpp
    TUIOInputDevice.cpp
    TestHelper.cpp
    TextEngine.cpp
    Timeout.cpp
    TouchEvent.cpp
    TouchStatus.cpp
    TrackerCalibrator.cpp
    TrackerInputDevice.cpp
    TrackerTouchStatus.cpp
    VectorNode.cpp
    VideoNode.cpp
    VideoWriter.cpp
    VideoWriterThread.cpp
    WordsNode.cpp
    testcalibrator.cpp
    testplayer.cpp
)

IF(MTDEV)
    SET(player_SRCS ${player_SRCS} LibMTDevInputDevice.cpp)
ENDIF()

IF(MACTRACKPAD)
    SET(player_SRCS ${player_SRCS} AppleTrackpadInputDevice.cpp)
ENDIF()

IF(WIN7TOUCH)
    SET(player_SRCS ${player_SRCS} Win7TouchInputDevice.cpp)
ENDIF()

IF(XINPUTMT)
    SET(player_SRCS ${player_SRCS} XInputMTInputDevice.cpp)
ENDIF()

ADD_LIBRARY(player STATIC ${player_SRCS}) 

