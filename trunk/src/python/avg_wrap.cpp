//
// $Id$
//

void export_raster();
void export_event();
void export_devices();

#include "../base/Logger.h"
#include "../player/Player.h"
#include "../player/AVGNode.h"
#include "../player/DivNode.h"
#include "../player/Excl.h"
#include "../player/PanoImage.h"

#include <boost/python.hpp>

using namespace boost::python;
using namespace avg;

BOOST_PYTHON_MODULE(avg)
{
    class_<Logger>("Logger", 
            "Interface to the logger used by the avg player. Enables the setting\n"
            "of different logging categories and a destination file. Can be used\n"
            "to log application-specific messages as well.\n"
            "Each log entry contains the time the message was written, the category\n"
            "of the entry and the message itself.",
            no_init)
        .def("get", &Logger::get, 
                return_value_policy<reference_existing_object>(),
                "get() -> Logger\n\n"
                "There is only one logger. This method gives access to it.")
        .staticmethod("get")
        .def("setDestination", &Logger::setDestination,
                "setDestination(filename) -> None\n\n"
                "Sets the file that the log should be written to. Default is stderr.\n")
        .def("setCategories", &Logger::setCategories,
                "setCategories(categories) -> None\n\n"
                "Sets the types of messages that should be logged. Possible \n"
                "categories are:\n"
                "    NONE: No logging except for errors.\n"
                "    BLTS: Display subsystem logging. Useful for timing/performance\n"
                "          measurements.\n"
                "    PROFILE: Outputs performance statistics on player termination.\n"
                "    PROFILE_LATEFRAMES: Outputs performance statistics whenever a\n"
                "                        frame is displayed late.\n"
                "    EVENTS: Outputs basic event data.\n"
                "    EVENTS2: Outputs all event data available.\n"
                "    CONFIG: Outputs configuration data.\n"
                "    WARNING: Outputs warning messages. Default is on.\n"
                "    ERROR: Outputs error messages. Can't be shut off.\n"
                "    MEMORY: Currently unused.\n"
                "    APP: Reserved for application-level messages issued by python\n"
                "         code.\n"
                "Categories can be or'ed together.")
        .def("trace", &Logger::trace,
                "trace(category, message) -> None\n\n"
                "Logs message to the log if category is active. The category should\n"
                "in most cases be APP.")
        .def_readonly("NONE", &Logger::NONE)
        .def_readonly("BLTS", &Logger::BLTS)
        .def_readonly("PROFILE", &Logger::PROFILE)
        .def_readonly("PROFILE_LATEFRAMES", &Logger::PROFILE_LATEFRAMES)
        .def_readonly("EVENTS", &Logger::EVENTS)
        .def_readonly("EVENTS2", &Logger::EVENTS2)
        .def_readonly("CONFIG", &Logger::CONFIG)
        .def_readonly("WARNING", &Logger::WARNING)
        .def_readonly("ERROR", &Logger::ERROR)
        .def_readonly("MEMORY", &Logger::MEMORY)
        .def_readonly("APP", &Logger::APP)
    ;

    export_devices();
    export_event();
    
    class_<Node, boost::noncopyable>("Node",
            "Base class for all elements in the avg tree.\n"
            "Properties:\n"
            "    id: A unique identifier that can be used to reference the node (ro).\n"
            "    x: The position of the node's left edge relative to it's parent node.\n"
            "    y: The position of the node's top edge relative to it's parent node.\n"
            "    width\n"
            "    height\n"
            "    opacity: A measure of the node's transparency. 0.0 is completely\n"
            "             transparent, 1.0 is completely opaque. Opacity is relative to\n"
            "             the parent node's opacity.", 
            no_init)
        .def("getParent", &Node::getParent,
                return_internal_reference<>(),
                "getParent() -> Node\n\n"
                "Returns the container (AVGNode, DivNode or Excl) the node is in. For\n"
                "the root node, returns None.\n")
        .add_property("id", make_function(&Node::getID, 
                return_value_policy<copy_const_reference>()))
        .add_property("x", &Node::getX, &Node::setX)
        .add_property("y", &Node::getY, &Node::setY)
        .add_property("width", &Node::getWidth, &Node::setWidth)
        .add_property("height", &Node::getHeight, &Node::setHeight)
        .add_property("opacity", &Node::getOpacity, &Node::setOpacity)
    ;

    export_raster();
    
    class_<Container, bases<Node>, boost::noncopyable>("Container",
            "Base class for all nodes containing other nodes (DivNode, AVGNode, Excl)",
            no_init)
        .def("getNumChildren", &Container::getNumChildren,
                "getNumChildren() -> numChildren\n\n")
        .def("getChild", &Container::getChild, 
                return_value_policy<reference_existing_object>(),
                "getChild(i) -> Node\n\n"
                "Returns the ith child in z-order.")
        .def("addChild", &Container::addChild,
                "addChild(Node) -> None\n\n"
                "Adds a new child to the container.")
        .def("removeChild", &Container::removeChild,
                "removeChild(i) -> None\n\n"
                "Removes the child at index i.")
        .def("indexOf", &Container::indexOf,
                "indexOf(childNode) -> i\n\n"
                "Returns the index of the child given or -1 if childNode isn't a\n"
                "child of the container.")
    ;
    
    class_<DivNode, bases<Container>, boost::noncopyable>("DivNode", 
            "A div node is a node that groups other nodes logically and visually.\n"
            "Its upper left corner is used as point of origin for the coordinates\n"
            "of its child nodes. Its extents are used to clip the children. Its\n"
            "opacity is used as base opacity for the child nodes' opacities.\n",
            no_init)
    ;
    
    class_<AVGNode, bases<DivNode> >("AVGNode",
            "Root node of any avg tree. Defines the properties of the display and\n"
            "handles key press events. The AVGNode's width and height define the\n"
            "coordinate system for the display and are the default for the window\n"
            "size used (i.e. by default, the coordinate system is pixel-based.\n"
            "Properties:\n"
            "    onkeydown: The python code to execute when a key is pressed (ro).\n"
            "    onkeyup: The python code to execute when a key is released (ro).\n")
        .def("getCropSetting", &AVGNode::getCropSetting,
                "getCropSetting() -> isCropActive\n\n"
                "Returns true if cropping is active. Cropping can be turned off globally\n"
                "in the avg file. (Deprecated. This attribute is only nessesary because\n"
                "of certain buggy display drivers that don't work with cropping.)")
        .add_property("onkeydown", make_function(&AVGNode::getOnKeyDown,
                return_value_policy<copy_const_reference>()))
        .add_property("onkeyup", make_function(&AVGNode::getOnKeyUp,
                return_value_policy<copy_const_reference>()))
    ;

    class_<Excl, bases<Container> >("Excl",
            "Node that displays only one of its children.\n"
            "Properties:\n"
            "    activechild: The child that is currently displayed and reacting to\n"
            "                 events.")
        .add_property("activechild", &Excl::getActiveChild, &Excl::setActiveChild)
    ;

    class_<PanoImage, bases<Node> >("PanoImage",
            "A panorama image.\n"
            "Properties:\n"
            "    href: The source filename of the image.\n"
            "    sensorwidth: The width of the sensor used to make the image. This value\n"
            "                 is used together with sensorheight and focallength to\n"
            "                 determine the projection to use. (ro)\n"
            "    sensorheight: The height of the sensor used to make the image. (ro)\n"
            "    focallength: The focal length of the lens in millimeters. (ro)\n"
            "    hue: A hue to color the image in. (ro, deprecated)\n"
            "    saturation: The saturation the image should have. (ro, deprecated)\n"
            "    rotation: The current angle the viewer is looking at in radians.\n"
            "    maxrotation: The maximum angle the viewer can look at.\n")
        .add_property("href", make_function(&PanoImage::getFilename,
                return_value_policy<copy_const_reference>()))
        .add_property("sensorwidth", &PanoImage::getSensorWidth)
        .add_property("sensorheight", &PanoImage::getSensorHeight)
        .add_property("focallength", &PanoImage::getFocalLength)
        .add_property("hue", &PanoImage::getHue)
        .add_property("saturation", &PanoImage::getSaturation)
        .add_property("rotation", &PanoImage::getRotation, &PanoImage::setRotation)
        .add_property("maxrotation", &PanoImage::getMaxRotation)
    ;

    enum_<Player::DisplayEngineType>("DisplayEngineType")
        .value("DFB", Player::DFB)
        .value("OGL", Player::OGL)
        .export_values()
    ;

    class_<Player>("Player", 
                "The class used to load and play avg files.")
        .def("setDisplayEngine", &Player::setDisplayEngine,
                "setDisplayEngine(engine) -> None\n\n"
                "Determines which display backend to use. Parameter can be either\n"
                "avg.DFB (for DirectFB rendering) or avg.OGL (for OpenGL rendering).\n"
                "Must be called before loadFile.")
        .def("setResolution", &Player::setResolution,
                "setResolution(fullscreen, width, height, bpp) -> None\n\n",
                "Sets display engine parameters. width and height set the window size\n"
                "(if fullscreen is false) or screen resolution (if fullscreen is true).\n"
                "bpp is the number of bits per pixel to use. Must be called before\n"
                "loadFile.")
        .def("loadFile", &Player::loadFile,
                "loadFile(fileName) -> None\n\n"
                "Loads the avg file specified in fileName. Returns false if the file\n"
                "could not be opened.")
        .def("play", &Player::play,
                "play(framerate) -> None\n\n"
                "Opens a playback window or screen and starts playback. framerate is\n"
                "the number of frames per second that should be displayed. play returns\n"
                "when playback has ended.")
        .def("stop", &Player::stop,
                "stop() -> None\n\n"
                "Stops playback and resets the video mode if nessesary.")
        .def("createNode", &Player::createNodeFromXmlString,
                return_value_policy<manage_new_object>(),
                "createNode(xml) -> Node\n\n"
                "Creates a new Node from an xml string. This node can be used as\n"
                "parameter to Container::addChild().")
        .def("setInterval", &Player::setInterval,
                "setInterval(time, pyfunc) -> id\n\n"
                "Sets a python callable object that should be executed every time\n"
                "milliseconds. setInterval returns an id that can be used to\n"
                "call clearInterval() to stop the code from being called.")
        .def("setTimeout", &Player::setTimeout, 
                "setTimeout(time, pyfunc) -> id\n\n"
                "Sets a python callable object that should be executed after time\n"
                "milliseconds. setTimeout returns an id that can be used to\n"
                "call clearInterval() to stop the code from being called.")
        .def("clearInterval", &Player::clearInterval,
                "clearInterval(id) -> ok\n\n"
                "Stops a timeout or an interval from being called. Returns 1 if\n"
                "there was an interval with the given id, 0 if not.\n")
        .def("getCurEvent", &Player::getCurEvent,
                return_value_policy<reference_existing_object>(),
                "getCurEvent() -> Event\n\n"
                "Gets an interface to the current event. Only valid inside event\n"
                "handlers (onmouseup, onmousedown, etc.)")
        .def("screenshot", &Player::screenshot,
                "screenshot(filename) -> ok\n\n"
                "Saves the contents of the current screen in a png file. Returns\n"
                "1 on success, 0 if the screen couldn't be saved.\n")
        .def("showCursor", &Player::showCursor,
                "showCursor(show) -> None\n\n"
                "Shows or hides the mouse cursor. (Currently, this only works for\n"
                "OpenGL. Showing the DirectFB mouse cursor seems to expose some\n"
                "issue with DirectFB.)")
        .def("getElementByID", &Player::getElementByID,
                return_value_policy<reference_existing_object>(),
                "getElementByID(id) -> Node\n\n"
                "Returns an element in the avg tree. The id corresponds to the id\n"
                "attribute of the node.")
        .def("getRootNode", &Player::getRootNode,
                return_value_policy<reference_existing_object>(),
                "getRootNode() -> AVGNode\n\n"
                "Returns the outermost element in the avg tree.")
        .def("getFramerate", &Player::getFramerate,
                "getFramerate() -> framerate\n\n"
                "Returns the current framerate in frames per second.")
        .def("getVideoRefreshRate", &Player::getVideoRefreshRate,
                "getVideoRefreshRate() -> refreshrate\n\n"
                "Returns the current hardware video refresh rate in number of\n"
                "refreshes per second.")
    ;

}
