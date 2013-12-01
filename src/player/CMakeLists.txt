
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
    BitmapManager.cpp
    BitmapManagerMsg.cpp
    BitmapManagerThread.cpp
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
    ExportedObject.cpp
    FXNode.cpp
    FilledVectorNode.cpp
    FontStyle.cpp
    HueSatFXNode.cpp
    Image.cpp
    ImageNode.cpp
    InvertFXNode.cpp
    KeyEvent.cpp
    LineNode.cpp
    MainCanvas.cpp
    MaterialInfo.cpp
    MeshNode.cpp
    MessageID.cpp
    MouseEvent.cpp
    MultitouchInputDevice.cpp
    Node.cpp
    NullFXNode.cpp
    OGLSurface.cpp
    OffscreenCanvas.cpp
    OffscreenCanvasNode.cpp
    Player.cpp
    PluginManager.cpp
    PolyLineNode.cpp
    PolygonNode.cpp
    Publisher.cpp
    PublisherDefinition.cpp
    PublisherDefinitionRegistry.cpp
    PythonLogSink.cpp
    RasterNode.cpp
    RectNode.cpp
    SDLDisplayEngine.cpp
    SVG.cpp
    SVGElement.cpp
    ShadowFXNode.cpp
    Shape.cpp
    SoundNode.cpp
    SubscriberInfo.cpp
    TUIOInputDevice.cpp
    TestHelper.cpp
    TextEngine.cpp
    Timeout.cpp
    TouchEvent.cpp
    TouchStatus.cpp
    TrackerCalibrator.cpp
    TrackerInputDevice.cpp
    TrackerTouchStatus.cpp
    TypeDefinition.cpp
    TypeRegistry.cpp
    VectorNode.cpp
    VersionInfo.cpp
    VideoNode.cpp
    VideoWriter.cpp
    VideoWriterThread.cpp
    WordsNode.cpp
    WrapPython.cpp
)

## testing
#    testcalibrator.cpp
#    testplayer.cpp

IF(AVG_ENABLE_MTDEV)
    SET(player_SRCS ${player_SRCS} LibMTDevInputDevice.cpp)
ENDIF()

IF(AVG_ENABLE_MACTRACKPAD)
    SET(player_SRCS ${player_SRCS} AppleTrackpadInputDevice.cpp)
ENDIF()

IF(AVG_ENABLE_WIN7TOUCH)
    SET(player_SRCS ${player_SRCS} Win7TouchInputDevice.cpp)
ENDIF()

IF(AVG_ENABLE_XINPUTMT)
    SET(player_SRCS ${player_SRCS} XInputMTInputDevice.cpp)
ENDIF()

ADD_LIBRARY(player STATIC ${player_SRCS})

