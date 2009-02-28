# -*- coding: utf-8 -*-
import unittest

import sys, time, platform

# Import the correct version of libavg. Since it should be possible to
# run the tests without installing libavg, we add the location of the 
# uninstalled libavg to the path.
# TODO: This is a mess. 
sys.path += ['../wrapper/.libs', '../python']
if platform.system() == 'Darwin':
    sys.path += ['../..']     # Location of libavg in a mac installation. 

if platform.system() == 'Windows':
    from libavg import avg    # Under windows, there is no uninstalled version.
else:    
    import avg

from testcase import *

def keyUp(Event):
    print "keyUp"

def keyDown(Event):
    print "keyDown"
    print Event
    print "  Type: "+str(Event.type)
    print "  keystring: "+Event.keystring
    print "  scancode: "+str(Event.scancode)
    print "  keycode: "+str(Event.keycode)
    print "  modifiers: "+str(Event.modifiers)

def dumpMouseEvent(Event):
    print Event
    print "  type: "+str(Event.type)
    print "  leftbuttonstate: "+str(Event.leftbuttonstate)
    print "  middlebuttonstate: "+str(Event.middlebuttonstate)
    print "  rightbuttonstate: "+str(Event.rightbuttonstate)
    print "  position: "+str(Event.x)+","+str(Event.y)
    print "  node: "+Event.node.id

mainMouseUpCalled = False
mainMouseDownCalled = False

def mainMouseUp(Event):
    global mainMouseUpCalled
    assert (Event.type == avg.CURSORUP)
    mainMouseUpCalled = True

def mainMouseDown(Event):
    global mainMouseDownCalled
    assert (Event.type == avg.CURSORDOWN)
    mainMouseDownCalled = True

def onErrMouseOver(Event):
    undefinedFunction()

mainCaptureMouseDownCalled = False
captureMouseDownCalled = False

def onMainCaptureMouseDown(Event):
    global mainCaptureMouseDownCalled
    mainCaptureMouseDownCalled = True

def onCaptureMouseDown(Event):
    global captureMouseDownCalled
    captureMouseDownCalled = True
    
class PlayerTestCase(AVGTestCase):
    def __init__(self, testFuncName, bpp):
        AVGTestCase.__init__(self, testFuncName, bpp)

    def testPoint(self):
        pt = avg.Point2D(10, 10)
        self.assert_(pt[0]==pt.x)
        self.assert_(pt[1]==pt.y)
        self.assert_(pt == avg.Point2D(10, 10))
        pt.x = 21
        pt.y = 23
        self.assert_(pt == avg.Point2D(21, 23))
        pt.x -= 11
        pt.y -= 13
        pt += avg.Point2D(10, 10)
        self.assert_(pt == avg.Point2D(20, 20))
        pt -= avg.Point2D(6, 6)
        self.assert_(pt == avg.Point2D(14, 14))
        self.assert_(pt != avg.Point2D(13, 13))
        pt = pt/2.
        self.assert_(pt == avg.Point2D(7, 7))
        pt = avg.Point2D((10, 10))
        self.assert_(pt == (10, 10))
        self.assert_(len(pt) == 2)
        self.assert_(pt[0]==pt.x)
        self.assert_(pt[1]==pt.y)
        self.assertException(lambda: pt[2])

    def testImage(self):
        def loadNewFile():
            self.assert_(Player.getElementByID("test").getMediaSize() == (65,65))
            Player.getElementByID("test").href = "rgb24alpha-64x64.png"
            Player.getElementByID("test1").href = "rgb24alpha-64x64.png"
            self.assert_(Player.getElementByID("test").getMediaSize() == (64,64))
        def getFramerate():
            framerate = Player.getEffectiveFramerate()
            self.assert_(framerate > 0)
        Player.showCursor(0)
        Player.showCursor(1)
        Player.loadFile("image.avg")
        node = Player.getElementByID("test")
        self.assert_(node.width == 65)
        self.assert_(node.height == 65)
        self.assert_(node.pos == (64, 30))
        self.assert_(node.size == (65, 65))
        self.start(None,
                (lambda: self.compareImage("testimg", False), 
                 getFramerate,
                 loadNewFile, 
                 lambda: self.compareImage("testimgload", False),
                 lambda: Player.setGamma(0.7, 0.7, 0.7),
                 lambda: Player.setGamma(1.0, 1.0, 1.0),
                 lambda: Player.showCursor(0),
                 lambda: Player.showCursor(1)
                ))
    def testDivResize(self):
        def checkSize (w, h):
            self.assert_(node.width == w)
            self.assert_(node.height == h)
            self.assert_(node.size == (w,h))
        def setSize (size):
            node.size = size
        def setWidth (w):
            node.width = w
        def setHeight (h):
            node.height = h

        Player.loadFile("avg.avg")
        node = Player.getElementByID('nestedavg')

        self.start(None,
                (lambda: checkSize(128, 32),
                    lambda: setSize((14,15)),
                    lambda: checkSize(14,15),
                    lambda: setWidth(23),
                    lambda: checkSize(23,15),
                    lambda: setHeight(22),
                    lambda: checkSize(23,22),
                    ))

    def testBitmap(self):
        def getBitmap(node):
            Bmp = node.getBitmap()
            self.assert_(Bmp.getSize() == (65,65))
            self.assert_(Bmp.getFormat() == avg.R8G8B8X8 or 
                    Bmp.getFormat() == avg.B8G8R8X8)
            node.setBitmap(Bmp)
        def loadBitmap():
            Bmp = avg.Bitmap("greyscale.png")
            self.assert_(Bmp.getSize() == (64,64))
            self.assert_(Bmp.getFormat() == avg.I8)
        Player.loadFile("image.avg")
        node = Player.getElementByID("test")
        getBitmap(node)
#        loadBitmap()
        self.start(None,
                (lambda: getBitmap(Player.getElementByID("test")),
                ))

    def testRotate(self):
        def onOuterDown(Event):
            self.onOuterDownCalled = True
        def fakeRotate():
            Player.getElementByID("outer").angle += 0.1
            Player.getElementByID("inner").angle -= 0.1
        def testCoordConversions():
            relPos = Player.getElementByID("inner").getRelPos((90, 80))
            self.assert_(almostEqual(relPos, (10, 10)))
            relPos = Player.getElementByID("outer").getRelPos((90, 80))
            self.assert_(almostEqual(relPos[0], 12.332806394528092) and
                    almostEqual(relPos[1], 6.9211188716194592))
            absPos = Player.getElementByID("outer").getAbsPos(relPos)
            self.assert_(almostEqual(absPos, (90, 80)))
        def sendEvent(x, y):
            Helper = Player.getTestHelper()
            Helper.fakeMouseEvent(avg.CURSORDOWN, True, False, False,
                        x, y, 1)
        def disableCrop():
            Player.getElementByID("outer").crop = False
            Player.getElementByID("inner").crop = False
            
        Player.loadFile("rotate.avg")
        Player.getElementByID("outer").setEventHandler(
                avg.CURSORDOWN, avg.MOUSE, onOuterDown) 
        self.onOuterDownCalled = False
        self.start(None,
                (lambda: self.compareImage("testRotate1", False),
                 testCoordConversions,
                 fakeRotate,
                 lambda: self.compareImage("testRotate1a", False),
                 lambda: sendEvent(85, 70),
                 lambda: self.assert_(not(self.onOuterDownCalled)),
                 lambda: sendEvent(85, 75),
                 lambda: self.assert_(self.onOuterDownCalled),
                 disableCrop,
                 lambda: self.compareImage("testRotate1b", False),
                ))

    def testRotate2(self):
        self.start("rotate2.avg",
                [lambda: self.compareImage("testRotate2", False)])
        
    def testRotate3(self):
        self.start("rotate3.avg",
                [lambda: self.compareImage("testRotate3", False)])

    def testRotatePivot(self):
        def setPivot (pos):
            node.pivot = pos
        def addPivotX (x):
            node.pivotx += x
        def setPivotY (y):
            node.pivoty = y
        Player.loadFile("rotate3.avg")
        node = Player.getElementByID('div1')
        self.start(None, (
            lambda: setPivot((10, 10)),
            lambda: self.compareImage("testRotatePivot1", False),
            lambda: addPivotX(-8),
            lambda: self.compareImage("testRotatePivot2", False),
            lambda: setPivotY(1),
            lambda: self.compareImage("testRotatePivot3", False),

                    ))

    def testError(self):
        Player.loadFile("image.avg")
        Player.setTimeout(1, lambda: undefinedFunction)
        Player.setTimeout(50, Player.stop)
        try:
            Player.play()
        except NameError:
            self.assert_(1)
        else:
            self.assert_(0)

    def testExceptionInTimeout(self):
        def throwException():
            raise ZeroDivisionError
        try:
            self.start("image.avg",
                    [throwException])
        except ZeroDivisionError:
            self.assert_(1)
        else:
            self.assert_(0)

    def testInvalidImageFilename(self):
        def activateNode():
            Player.getElementByID("enclosingdiv").active = 1
        self.start("invalidfilename.avg",
                [activateNode])

    def testInvalidVideoFilename(self):
        def tryplay():
            assertException(lambda: Player.getElementByID("brokenvideo").play())
        self.start("invalidvideofilename.avg",
                (lambda: tryplay,
                 lambda: Player.getElementByID("brokenvideo").stop()
                ))

    def testEvents(self):
        def getMouseState():
            Event = Player.getMouseState()
            self.assert_(Event.pos == avg.Point2D(10,10))
            self.assert_(Event.lastdownpos == avg.Point2D(10,10))
            # Make sure we're getting a Point2D as return value.
            self.assert_(Event.lastdownpos/2 == avg.Point2D(5, 5))
        def testInactiveDiv():
            Player.getElementByID("div1").active = False
            Helper.fakeMouseEvent(avg.CURSORDOWN, True, False, False,
                70, 70, 1)
        def disableHandler():
            self.mouseDown1Called = False
            Player.getElementByID("img1").setEventHandler(avg.CURSORDOWN, avg.MOUSE, None)
        def onMouseMove1(Event):
            self.assert_ (Event.type == avg.CURSORMOTION)
            print "onMouseMove1"
        def onMouseUp1(Event):
            self.assert_ (Event.type == avg.CURSORUP)
            self.mouseUp1Called = True
        def onMouseDown1(Event):
            self.assert_ (Event.type == avg.CURSORDOWN)
            self.mouseDown1Called = True
        def onMouseOver1(Event):
            self.assert_ (Event.type == avg.CURSOROVER)
            self.mouseOver1Called = True
        def onMouseOut1(Event):
            self.assert_ (Event.type == avg.CURSOROUT)
            self.mouseOut1Called = True
        def onTouchDown(Event):
            self.touchDownCalled = True
        def onDivMouseDown(Event):
            self.assert_ (Event.type == avg.CURSORDOWN)
            self.divMouseDownCalled = True
        def onMouseDown2(Event):
            self.assert_ (Event.type == avg.CURSORDOWN)
            self.mouseDown2Called = True
        def onObscuredMouseDown(Event):
            self.obscuredMouseDownCalled = True
        def onDeactMouseDown(Event):
            self.deactMouseDownCalled = True
        def onDeactMouseOver(Event):
            self.deactMouseOverLate = self.deactMouseDownCalled
            self.deactMouseOverCalled = True
        def onDeactMouseMove(Event):
            print("move")
        def onDeactMouseOut(Event):
            pass
        def onTiltedMouseDown(Event):
            self.tiltedMouseDownCalled = True
        def onKeyDown(Event):
            if Event.keystring == 'A' and Event.keycode == 65 and Event.unicode == 65:
                self.keyDownCalled = True
        def onKeyUp(Event):
            if Event.keystring == 'A' and Event.keycode == 65 and Event.unicode == 65:
                self.keyUpCalled = True
        def neverCalled(Event):
            self.neverCalledCalled = True

        Helper = Player.getTestHelper()
        global mainMouseUpCalled
        global mainMouseDownCalled
        Player.loadFile("events.avg")
        
        self.mouseMove1Called=False
        self.mouseUp1Called=False
        self.mouseDown1Called=False
        self.mouseOver1Called=False
        self.mouseOut1Called=False
        self.touchDownCalled = False
        self.keyDownCalled = False
        self.keyUpCalled = False
        img1 = Player.getElementByID("img1")
        img1.setEventHandler(avg.CURSORMOTION, avg.MOUSE, onMouseMove1) 
        img1.setEventHandler(avg.CURSORUP, avg.MOUSE, onMouseUp1) 
        img1.setEventHandler(avg.CURSORDOWN, avg.MOUSE, onMouseDown1) 
        img1.setEventHandler(avg.CURSOROVER, avg.MOUSE, onMouseOver1) 
        img1.setEventHandler(avg.CURSOROUT, avg.MOUSE, onMouseOut1) 
        img1.setEventHandler(avg.CURSORDOWN, avg.TOUCH, onTouchDown) 
        Player.getRootNode().setEventHandler(avg.KEYDOWN, avg.NONE, onKeyDown)
        Player.getRootNode().setEventHandler(avg.KEYUP, avg.NONE, onKeyUp)

        self.neverCalledCalled=False
        hidden = Player.getElementByID("hidden")
        hidden.setEventHandler(avg.CURSORUP, avg.MOUSE, neverCalled)

        self.obscuredMouseDownCalled=False
        obscured = Player.getElementByID("obscured")
        obscured.setEventHandler(avg.CURSORDOWN, avg.MOUSE, onObscuredMouseDown)

        self.divMouseDownCalled=False
        div1 = Player.getElementByID("div1")
        div1.setEventHandler(avg.CURSORDOWN, avg.MOUSE, onDivMouseDown)

        self.mouseDown2Called=False
        img2 = Player.getElementByID("img2")
        img2.setEventHandler(avg.CURSORDOWN, avg.MOUSE, onMouseDown2)

        self.deactMouseOverCalled=False
        self.deactMouseOverLate=False
        self.deactMouseDownCalled=False
        deact = Player.getElementByID("deact")
        deact.setEventHandler(avg.CURSORDOWN, avg.MOUSE, onDeactMouseDown)
        deact.setEventHandler(avg.CURSOROVER, avg.MOUSE, onDeactMouseOver)
        deact.setEventHandler(avg.CURSOROUT, avg.MOUSE, onDeactMouseOut)
        deact.setEventHandler(avg.CURSORMOTION, avg.MOUSE, onDeactMouseMove)

        self.tiltedMouseDownCalled=False
        Player.getElementByID("tilted").setEventHandler(
                avg.CURSORDOWN, avg.MOUSE, onTiltedMouseDown)

        self.start(None, 
                (lambda: self.compareImage("testEvents", False),
                 lambda: Helper.fakeMouseEvent(avg.CURSORDOWN, True, False, False,
                        10, 10, 1),
                 lambda: self.assert_(self.mouseDown1Called and self.mouseOver1Called 
                        and mainMouseDownCalled and not(self.touchDownCalled)),
                 getMouseState,
                 lambda: Helper.fakeMouseEvent(avg.CURSORUP, True, False, False,
                        12, 12, 1),
                 lambda: Helper.fakeMouseEvent(avg.CURSORMOTION, False, False, False,
                        70, 70, 1),
                 lambda: self.assert_(self.mouseUp1Called and mainMouseUpCalled),
                 lambda: Helper.fakeMouseEvent(avg.CURSORDOWN, True, False, False,
                        70, 70, 1),
                 lambda: self.assert_(self.mouseDown2Called and self.divMouseDownCalled and 
                        self.mouseOut1Called and not(self.obscuredMouseDownCalled)),
                 testInactiveDiv,
                 lambda: self.assert_(self.obscuredMouseDownCalled),
                 # Test if deactivation between mouse click and mouse out works.
                 lambda: Helper.fakeMouseEvent(avg.CURSORDOWN, True, False, False,
                        70, 10, 1),
                 lambda: self.assert_(self.deactMouseOverCalled and not(self.deactMouseOverLate)
                        and not(self.neverCalledCalled)),
                 disableHandler,
                 lambda: Helper.fakeMouseEvent(avg.CURSORDOWN, True, False, False,
                        10, 10, 1),
                 lambda: self.assert_(not(self.mouseDown1Called)),
                 lambda: Helper.fakeMouseEvent(avg.CURSORDOWN, True, False, False,
                        0, 64, 1),
                 lambda: self.assert_(not(self.tiltedMouseDownCalled)),
                 lambda: Helper.fakeMouseEvent(avg.CURSORDOWN, True, False, False,
                        0, 80, 1),
                 lambda: self.assert_(self.tiltedMouseDownCalled),
                 lambda: Helper.fakeKeyEvent(avg.KEYDOWN, 65, 65, "A", 65, avg.KEYMOD_NONE),
                 lambda: self.assert_(self.keyDownCalled),
                 lambda: Helper.fakeKeyEvent(avg.KEYUP, 65, 65, "A", 65, avg.KEYMOD_NONE),
                 lambda: self.assert_(self.keyUpCalled)
                 # XXX
                 # - errMouseOver
                ))

    def testEventCapture(self):
        def captureEvent():
            global captureMouseDownCalled
            Helper = Player.getTestHelper()
            captureMouseDownCalled = False
            mainCaptureMouseDownCalled = False
            Player.getElementByID("img1").setEventCapture()
            Helper.fakeMouseEvent(avg.CURSORDOWN, True, False, False,
                    100, 10, 1)
        def noCaptureEvent():
            global captureMouseDownCalled
            Helper = Player.getTestHelper()
            captureMouseDownCalled = False
            mainCaptureMouseDownCalled = False
            Player.getElementByID("img1").releaseEventCapture()
            Helper.fakeMouseEvent(avg.CURSORDOWN, True, False, False,
                    100, 10, 1)
        global captureMouseDownCalled
        global mainCaptureMouseDownCalled
        Helper = Player.getTestHelper()
        self.start("eventcapture.avg",
                (lambda: Helper.fakeMouseEvent(avg.CURSORDOWN, True, False, False,
                        10, 10, 1),
                 lambda: self.assert_(captureMouseDownCalled),
                 captureEvent,
                 lambda: self.assert_(captureMouseDownCalled and 
                        mainCaptureMouseDownCalled),
                 noCaptureEvent,
                 lambda: self.assert_(not(captureMouseDownCalled) and 
                        mainCaptureMouseDownCalled)
                ))

    def testMouseOver(self):
        def onImg2MouseOver(Event):
            self.img2MouseOverCalled = True
        def onImg2MouseOut(Event):
            self.img2MouseOutCalled = True
        def onDivMouseOver(Event):
            self.divMouseOverCalled = True
        def onDivMouseOut(Event):
            self.divMouseOutCalled = True
        def onAVGMouseOver(Event):
            self.avgMouseOverCalled = True
        def onImg1MouseOver(Event):
            self.img1MouseOverCalled = True
        def printState():
            print "----"
            print "img2MouseOverCalled=", self.img2MouseOverCalled
            print "img2MouseOutCalled=", self.img2MouseOutCalled
            print "divMouseOverCalled=", self.divMouseOverCalled
            print "divMouseOutCalled=", self.divMouseOutCalled
            print "avgMouseOverCalled=", self.avgMouseOverCalled
            print "img1MouseOverCalled=", self.img1MouseOverCalled
        def resetState():
            self.img2MouseOverCalled = False
            self.img2MouseOutCalled = False
            self.divMouseOverCalled = False
            self.divMouseOutCalled = False
            self.avgMouseOverCalled = False
            self.img1MouseOverCalled = False
        def killNodeUnderCursor():
            Parent = img1.getParent()
            Parent.removeChild(Parent.indexOf(img1))
        Helper = Player.getTestHelper()
        Player.loadFile("mouseover.avg")
        img2 = Player.getElementByID("img2")
        img2.setEventHandler(avg.CURSOROVER, avg.MOUSE, onImg2MouseOver)
        img2.setEventHandler(avg.CURSOROUT, avg.MOUSE, onImg2MouseOut)
        div = Player.getElementByID("div1")
        div.setEventHandler(avg.CURSOROVER, avg.MOUSE, onDivMouseOver)
        div.setEventHandler(avg.CURSOROUT, avg.MOUSE, onDivMouseOut)
        avgNode = Player.getRootNode()
        avgNode.setEventHandler(avg.CURSOROVER, avg.MOUSE, onAVGMouseOver)
        img1 = Player.getElementByID("img1")
        img1.setEventHandler(avg.CURSOROVER, avg.MOUSE, onImg1MouseOver)
        self.start(None, 
                (resetState,
                 lambda: Helper.fakeMouseEvent(avg.CURSORDOWN, True, False, False,
                        70, 70, 1),
                 lambda: self.assert_(
                        self.img2MouseOverCalled and 
                        self.divMouseOverCalled and
                        self.avgMouseOverCalled and 
                        not(self.img2MouseOutCalled) and 
                        not(self.divMouseOutCalled) and 
                        not(self.img1MouseOverCalled)),

                 resetState,
                 lambda: Helper.fakeMouseEvent(avg.CURSORDOWN, True, False, False,
                        70, 10, 1),
                 lambda: self.assert_(
                        not(self.img2MouseOverCalled) and 
                        not(self.divMouseOverCalled) and 
                        not(self.avgMouseOverCalled) and 
                        self.img2MouseOutCalled and 
                        not(self.divMouseOutCalled) and 
                        not(self.img1MouseOverCalled)),
                 
                 resetState,
                 lambda: Helper.fakeMouseEvent(avg.CURSORDOWN, True, False, False,
                        10, 10, 1),
                 lambda: self.assert_(
                        not(self.img2MouseOverCalled) and 
                        not(self.divMouseOverCalled) and 
                        not(self.avgMouseOverCalled) and 
                        not(self.img2MouseOutCalled) and 
                        self.divMouseOutCalled and 
                        self.img1MouseOverCalled),

                 resetState,
                 killNodeUnderCursor,
                 lambda: Helper.fakeMouseEvent(avg.CURSORUP, True, False, False,
                        10, 10, 1),
                 lambda: self.assert_(
                        not(self.img2MouseOverCalled) and 
                        not(self.divMouseOverCalled) and 
                        not(self.avgMouseOverCalled) and 
                        not(self.img2MouseOutCalled) and 
                        not(self.divMouseOutCalled) and 
                        not(self.img1MouseOverCalled)),

                 resetState,
                 lambda: Player.getElementByID("img2").setEventCapture(),
                 lambda: Helper.fakeMouseEvent(avg.CURSORUP, True, False, False,
                        70, 70, 1),
                 lambda: self.assert_(
                        self.img2MouseOverCalled and 
                        not(self.divMouseOverCalled) and 
                        not(self.avgMouseOverCalled) and 
                        not(self.img2MouseOutCalled) and 
                        not(self.divMouseOutCalled) and 
                        not(self.img1MouseOverCalled)),

                 resetState,
                 lambda: Helper.fakeMouseEvent(avg.CURSORUP, True, False, False,
                        10, 10, 1),
                 lambda: self.assert_(
                        not(self.img2MouseOverCalled) and 
                        not(self.divMouseOverCalled) and 
                        not(self.avgMouseOverCalled) and 
                        self.img2MouseOutCalled and 
                        not(self.divMouseOutCalled) and 
                        not(self.img1MouseOverCalled))
                ))

    def testTimeouts(self):
        self.timeout1called = False
        self.timeout2called = False
        def timeout1():
            Player.clearInterval(self.timeout1ID)
            Player.clearInterval(self.timeout2ID)
            self.timeout1called = True
        def timeout2():
            self.timeout2called = True
        def wait():
            pass
        def setupTimeouts():
            self.timeout1ID = Player.setTimeout(0, timeout1)
            self.timeout2ID = Player.setTimeout(1, timeout2)
        self.start("image.avg",
                (setupTimeouts,
                 wait,
                 lambda: self.assert_(self.timeout1called),
                 lambda: self.assert_(not(self.timeout2called))
                ))

    def testEventErr(self):
        Player.loadFile("errevent.avg")
        Player.setTimeout(10, Player.stop)
        try:
            Player.play()
        except NameError:
            print("(Intentional) NameError caught")
            self.assert_(1)

    def testNOPShader(self):
        def addShader():
            shaderProg="""void main() {
                gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
                gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
                gl_FrontColor = gl_Color;
                gl_TexCoord[0] = gl_MultiTexCoord0;
            }"""
            Player.getElementByID("pano").setVertexShader(shaderProg)
        def changeProperties():
            node = Player.getElementByID("pano")
            node.sensorheight=10
            node.sensorwidth=15
            node.focallength=25
        def loadImage():
            node = Player.getElementByID("pano")
            node.href = "rgb24-65x65.png"
        self.start("panoimage.avg",
                (addShader,
                 lambda: self.compareImage("testPanoImagewNOP", False),
                 lambda: time.sleep,
                 changeProperties,
                 loadImage
                ))
    def testHugeImage(self):
        def moveImage():
            Player.getElementByID("mainimg").x -= 2500
        self.start("hugeimage.avg",
                (lambda: self.compareImage("testHugeImage0", False),
                 moveImage,
                 lambda: self.compareImage("testHugeImage1", False)
                ))

    def testPanoImage(self):
        def changeProperties():
            node = Player.getElementByID("pano")
            node.sensorheight=10
            node.sensorwidth=15
            node.focallength=25
        def loadImage():
            node = Player.getElementByID("pano")
            node.href = "rgb24-65x65.png"
        self.start("panoimage.avg",
                (lambda: self.compareImage("testPanoImage", False),
                 lambda: time.sleep,
                 changeProperties,
                 loadImage
                ))

    def testBroken(self):
        def testBrokenFile(filename):
            self.assertException(lambda: Player.loadFile(filename))
        testBrokenFile("filedoesntexist.avg")
        testBrokenFile("noxml.avg")
        testBrokenFile("noavg.avg")
        testBrokenFile("noavg2.avg")

    def testMove(self):
        def moveit():
            node = Player.getElementByID("nestedimg1")
            node.x += 50
            node.opacity -= 0.7
            node = Player.getElementByID("nestedavg")
            node.x += 50
        def checkRelPos():
            RelPos = Player.getElementByID("obscured").getRelPos((50,52))
            self.assert_(RelPos == (0, 0))
        self.start("avg.avg",
                (lambda: self.compareImage("testMove1", False),
                 moveit,
                 checkRelPos,
                 lambda: self.compareImage("testMove2", False)
                ))

    def testSetBitmap(self):
        def setBitmap():
            node = Player.createNode('image',{'y':65})
            bitmap = avg.Bitmap('rgb24-65x65.png')
            node.setBitmap(bitmap)
            parent = Player.getElementByID("mainavgnode")
            parent.appendChild(node)
        def setBitmapLinked(nodeID):
            node = Player.getElementByID(nodeID)
            bitmap = avg.Bitmap('rgb24-65x65.png')
            node.setBitmap(bitmap)
        def setNullBitmap():
            node = Player.getElementByID("fullimg")
            node.setBitmap(None)

        self.start("setbitmap.avg",
                (
                    setBitmap,
                    lambda: self.compareImage("testSetBitmap1", False),
                    lambda: setBitmapLinked("fullimg"),
                    lambda: self.compareImage("testSetBitmap2", False),
                    lambda: setBitmapLinked("emptyimg"),
                    lambda: self.compareImage("testSetBitmap3", False),
                    lambda: self.assertException(setNullBitmap)
                ))

    def testBlend(self):
        self.start("blend.avg",
                [lambda: self.compareImage("testBlend", False)])

    def testCropImage(self):
        def moveTLCrop():
            node = Player.getElementByID("img")
            node.x = -20
            node.y = -20
        def moveBRCrop():
            node = Player.getElementByID("img")
            node.x = 60
            node.y = 40
        def moveTLNegative():
            node = Player.getElementByID("img")
            node.x = -60
            node.y = -50
        def moveBRGone():
            node = Player.getElementByID("img")
            node.x = 140
            node.y = 100
        def rotate():
            node = Player.getElementByID("img")
            node.x = 10
            node.y = 10
            Player.getElementByID("nestedavg").angle = 1.0
            Player.getElementByID("bkgd").angle = 1.0
        self.start("crop2.avg",
                (lambda: self.compareImage("testCropImage1", False),
                 moveTLCrop,
                 lambda: self.compareImage("testCropImage2", False),
                 moveBRCrop,
                 lambda: self.compareImage("testCropImage3", False),
                 moveTLNegative,
                 lambda: self.compareImage("testCropImage4", False),
                 moveBRGone,
                 lambda: self.compareImage("testCropImage5", False),

                 rotate,
                 lambda: self.compareImage("testCropImage6", False),
                 moveTLCrop,
                 lambda: self.compareImage("testCropImage7", False),
                 moveBRCrop,
                 lambda: self.compareImage("testCropImage8", False),
                 moveTLNegative,
                 lambda: self.compareImage("testCropImage9", False),
                 moveBRGone,
                 lambda: self.compareImage("testCropImage10", False)
               ))

    def testCropMovie(self):
        def playMovie():
            node = Player.getElementByID("movie")
            node.play()
        def moveTLCrop():
            node = Player.getElementByID("movie")
            node.x = -20
            node.y = -20
        def moveBRCrop():
            node = Player.getElementByID("movie")
            node.x = 60
            node.y = 40
        def moveTLNegative():
            node = Player.getElementByID("movie")
            node.x = -60
            node.y = -50
        def moveBRGone():
            node = Player.getElementByID("movie")
            node.x = 140
            node.y = 100
        def rotate():
            node = Player.getElementByID("movie")
            node.x = 10
            node.y = 10
            Player.getElementByID("nestedavg").angle = 1.0
            Player.getElementByID("bkgd").angle = 1.0
        Player.setFakeFPS(30)
        self.start("crop.avg",
                (playMovie,
                 lambda: self.compareImage("testCropMovie1", False),
                 moveTLCrop,
                 lambda: self.compareImage("testCropMovie2", False),
                 moveBRCrop,
                 lambda: self.compareImage("testCropMovie3", False),
                 moveTLNegative,
                 lambda: self.compareImage("testCropMovie4", False),
                 moveBRGone,
                 lambda: self.compareImage("testCropMovie5", False),

                 rotate,
                 lambda: self.compareImage("testCropMovie6", False),
                 moveTLCrop,
                 lambda: self.compareImage("testCropMovie7", False),
                 moveBRCrop,
                 lambda: self.compareImage("testCropMovie8", False),
                 moveTLNegative,
                 lambda: self.compareImage("testCropMovie9", False),
                 moveBRGone,
                 lambda: self.compareImage("testCropMovie10", False)
                ))

    def testWarp(self):
        def moveVertex():
            node = Player.getElementByID("testtiles")
            grid = node.getWarpedVertexCoords()
            grid[1][1] = (grid[1][1][0]+0.06, grid[1][1][1]+0.06)
            node.setWarpedVertexCoords(grid)
            node = Player.getElementByID("clogo1")
            grid = node.getWarpedVertexCoords()
            grid[0][0] = (grid[0][0][0]+0.06, grid[0][0][1]+0.06)
            grid[1][1] = (grid[1][1][0]-0.06, grid[1][1][1]-0.06)
            node.setWarpedVertexCoords(grid)
        def flip():
            node = Player.getElementByID("testtiles")
            grid = node.getOrigVertexCoords()
            grid = [ [ (1-pos[0], pos[1]) for pos in line ] for line in grid]
            node.setWarpedVertexCoords(grid)
        Player.loadFile("video.avg")
        node = Player.getElementByID("testtiles")
        self.assertException(node.getOrigVertexCoords)
        self.assertException(node.getWarpedVertexCoords)
        Player.setFakeFPS(30)
        self.start(None,
                (lambda: Player.getElementByID("clogo1").play(),
                 lambda: self.compareImage("testWarp1", False),
                 moveVertex,
                 lambda: self.compareImage("testWarp2", False),
                 flip,
                 lambda: self.compareImage("testWarp3", False)
                ))

#    def testCamera(self):
#        def createCameraNode(deviceFile):
#            return Player.createNode("<camera id='camera1' width='640' height='480' "
#                    "source='v4l' pixelformat='YUYV422' "
#                    "capturewidth='640' captureheight='480' device="+deviceFile+
#                    " framerate='30'/>")
#        def findCamera():
#            node = createCameraNode("/dev/video0")
#            if node.getDriverName() != "vivi":
#                node = createCameraNode("/dev/video1")
#            if node.getDriverName() != "vivi":
#                print("Kernel camera test driver not found - skipping camera test.")
#               ()
#            else:
#                Player.getRootNode().appendChild(node)
#                node.play()
#
#        self.start("empty.avg",
#                (lambda: findCamera,
#                 lambda: self.compareImage("testCamera", False)
#                ))

    def testMediaDir(self):
        def setDir():
            Player.getElementByID("main").mediadir="../video/testfiles"
        def setAbsDir():
            def absDir():
                # Should not find any media here...
                Player.getElementByID("main").mediadir="/testmediadir"
            self.assertException(absDir)
        def createNode():
            node = Player.createNode("video", {"href":"mjpeg1-48x48.avi", "fps":30})
        self.start("mediadir.avg",
                (lambda: Player.getElementByID("video").play(),
                 lambda: self.compareImage("testMediaDir1", False),
                 setDir,
                 lambda: Player.getElementByID("video").play(), 
                 lambda: self.compareImage("testMediaDir2", False),
                 lambda: self.assert_(Player.getElementByID("img").width == 1),
                 createNode,
                 setAbsDir
                ))

    def testGPUMemoryQuery(self):
        def createNode():
            self.node = Player.createNode("<image id='img' href='rgb24-64x64.png'/>")
            self.assert_(Player.getGPUMemoryUsage() == 0)
        def appendToTree():
            self.rootNode = Player.getRootNode()
            self.rootNode.appendChild(self.node)
            self.assert_(Player.getGPUMemoryUsage() == 64 * 64 * 4)
        def removeFromTree():
            self.rootNode.removeChild(self.rootNode.indexOf(self.node))
            self.assert_(Player.getGPUMemoryUsage() == 0)
        def runTest():
            self.setUpVideo()
            self._loadEmpty()
            self.start(None,
                    (lambda: self.assert_(Player.getGPUMemoryUsage() == 0),
                     createNode,
                     appendToTree,
                     removeFromTree
                    ))
        runTest()
    def testStopOnEscape(self):
        def pressEscape():
            Helper = Player.getTestHelper()
            escape = 27
            Helper.fakeKeyEvent(avg.KEYDOWN, escape, escape, "escape", escape, 
                    avg.KEYMOD_NONE),
            Helper.fakeKeyEvent(avg.KEYUP, escape, escape, "escape", escape, 
                    avg.KEYMOD_NONE),
        def testEscape1():
            Player.stopOnEscape(False)
            pressEscape()
        def testEscape2():
            Player.stopOnEscape(True)
            Player.stopOnEscape(False)
            pressEscape()
        def testEscape3():
            Player.stopOnEscape(True)
            pressEscape()
        def setAlive():
            self.testStopOnEscapeAlive = True

        self.testStopOnEscapeAlive = False
        self.start('avg.avg',
                (testEscape1,
                testEscape2,
                setAlive))
        self.assert_(self.testStopOnEscapeAlive)
        self.start('avg.avg',
                (testEscape3, # this should exit the player
                lambda: self.assert_(False),
                ))
            
def playerTestSuite(bpp, tests):
    availableTests = (
            "testPoint",
            "testImage",
            "testDivResize",
            "testBitmap",
            "testRotate",
            "testRotate2",
            "testRotate3",
            "testRotatePivot",
            "testError",
            "testExceptionInTimeout",
            "testInvalidImageFilename",
            "testInvalidVideoFilename",
            "testEvents",
            "testEventCapture",
            "testMouseOver",
            "testTimeouts",
            "testEventErr",
            "testHugeImage",
            "testNOPShader",
            "testPanoImage",
            "testBroken",
            "testMove",
            "testSetBitmap",
            "testBlend",
            "testCropImage",
            "testCropMovie",
            "testWarp",
            "testMediaDir",
            "testGPUMemoryQuery",
            "testStopOnEscape",
            )
    return AVGTestSuite(availableTests, PlayerTestCase, tests, (), {'bpp' : bpp})

Player = avg.Player.get()

if __name__ == '__main__':
    runStandaloneTest(playerTestSuite)