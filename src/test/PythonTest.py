# -*- coding: utf-8 -*-
# libavg - Media Playback Engine.
# Copyright (C) 2003-2011 Ulrich von Zadow
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# Current versions can be found at www.libavg.de
#

from libavg import avg, anim, draggable, geom, statemachine

from testcase import *

class PythonTestCase(AVGTestCase):
    def __init__(self, testFuncName):
        AVGTestCase.__init__(self, testFuncName)

    def testAnimType(self, curAnim, imgBaseName):
        def onStop():
            self.__onStopCalled = True

        def startAnim():
            self.__onStopCalled = False
            node = Player.getElementByID("test")
            self.__anim.start()

        def startKeepAttr():
            node = Player.getElementByID("test")
            node.x = 25
            self.__anim.start(keepAttr=True)

        def abortAnim():
            self.__anim.abort()

        self.__anim = curAnim
        self.__anim.setHandler(onStop, None)
        self.__onStopCalled = False
        Player.setFakeFPS(10)
        self.start((
                 startAnim,
                 lambda: self.compareImage(imgBaseName+"1", False),
                 lambda: self.assert_(anim.getNumRunningAnims() == 1),
                 None,
                 None,
                 lambda: self.assert_(self.__onStopCalled),
                 lambda: self.assert_(self.__anim.isDone()),
                 lambda: self.compareImage(imgBaseName+"2", False),
                 lambda: self.assert_(Player.getElementByID("test").x == 100),
                 startAnim,
                 lambda: self.compareImage(imgBaseName+"1", False),
                 abortAnim,
                 lambda: self.assert_(anim.getNumRunningAnims() == 0),
                 lambda: self.compareImage(imgBaseName+"3", False),
                 lambda: self.assert_(self.__anim.isDone()),
                 None,
                 lambda: self.assert_(not(self.__onStopCalled)),
                 startAnim,
                 startKeepAttr,
                 lambda: self.assert_(anim.getNumRunningAnims() == 1),
                 abortAnim
                ))
        self.__anim = None

    def testLinearAnim(self):
        self.initDefaultImageScene()
        node = Player.getElementByID("test")
        curAnim = anim.LinearAnim(node, "x", 200, 0, 100, False)
        self.testAnimType(curAnim, "testLinearAnim")

    def testLinearAnimZeroDuration(self):
        def onStop():
            self.__onStopCalled = True

        def startAnim():
            self.__onStopCalled = False
            node = Player.getElementByID("test")
            self.__anim.start()

        self.initDefaultImageScene()
        node = Player.getElementByID("test")
        self.__anim = anim.LinearAnim(node, "x", 0, 0, 100, False)
        self.__anim.setHandler(onStop, None)
        self.__onStopCalled = False
        Player.setFakeFPS(10)
        self.start((
                 startAnim,
                 lambda: self.compareImage("testLinearAnimZeroDuration1", False),
                 lambda: self.assert_(anim.getNumRunningAnims() == 0),
                 lambda: self.assert_(self.__onStopCalled),
                 lambda: self.assert_(self.__anim.isDone())
                ))
        self.__anim = None

    def testEaseInOutAnim(self):
        self.initDefaultImageScene()
        node = Player.getElementByID("test")
        curAnim = anim.EaseInOutAnim(node, "x", 400, 0, 100, 100, 100, False)
        self.testAnimType(curAnim, "testEaseInOutAnim")

    def testSplineAnim(self):
        self.initDefaultImageScene()
        node = Player.getElementByID("test")
        curAnim = anim.SplineAnim(node, "x", 300, 0, 0, 100, 0, False)
        self.testAnimType(curAnim, "testSplineAnim")

    def testContinuousAnim(self):
        def onStart():
            Player.setTimeout(10,startAnim)
            Player.setTimeout(100,lambda:self.compareImage("testContAnim1", False))
            Player.setTimeout(200,startAnim2)
            Player.setTimeout(400,lambda:self.compareImage("testContAnim2", False))
            Player.setTimeout(450,startAnim3)
            Player.setTimeout(700,lambda:self.compareImage("testContAnim3", False))
            Player.setTimeout(800,stopAnim)
            Player.setTimeout(900,lambda:self.compareImage("testContAnim4", False))
            Player.setTimeout(1000,Player.stop)

        def startAnim():
            node=Player.getElementByID("testtiles")
            self.anim=anim.ContinuousAnim(node,"angle",0,1,0)
            self.anim.start()

        def startAnim2():
            node=Player.getElementByID("test")
            self.anim2=anim.ContinuousAnim(node,"width",0,50,0)
            self.anim2.start()

        def startAnim3():
            node=Player.getElementByID("test1")
            self.anim3=anim.ContinuousAnim(node,"x",0,50,0)
            self.anim3.start()

        def stopAnim():
            self.anim.abort()
            self.anim2.abort()
            self.anim3.abort()
            self.anim = None
            self.anim2 = None
            self.anim3 = None

        Player.setFakeFPS(25)
        anim.init(avg)
        self.initDefaultImageScene()
        Player.setTimeout(1, onStart)
        Player.play()

    def testWaitAnim(self):
        def animStopped():
            self.__endCalled = True

        def startAnim():
            self.anim = anim.WaitAnim(200, animStopped, False)
            self.anim.start()

        anim.init(avg)
        Player.setFakeFPS(10)
        self.__endCalled = False
        self.initDefaultImageScene()
        self.start((
                 startAnim, 
                 lambda: self.assert_(not(self.anim.isDone())),
                 None,
                 None,
                 lambda: self.assert_(self.anim.isDone()),
                 lambda: self.assert_(self.__endCalled)
                ))

    def testStateAnim(self):
        def state2Callback():
            self.__state2CallbackCalled = True

        def makeAnim():
            node = Player.getElementByID("test")
            self.anim = anim.StateAnim(
                    {"STATE1": anim.LinearAnim(node, "x", 200, 64, 128),
                     "STATE2": anim.LinearAnim(node, "x", 200, 128, 64),
                     "STATE3": anim.WaitAnim()},
                    {"STATE1": anim.AnimTransition("STATE2", state2Callback),
                     "STATE2": anim.AnimTransition("STATE3")})
        anim.init(avg)
        Player.setFakeFPS(10)
        self.__state2CallbackCalled = False
        self.initDefaultImageScene()
        self.start((
                 makeAnim,
                 lambda: self.compareImage("testStateAnim1", False),
                 lambda: self.anim.setState("STATE1"),
                 None,
                 lambda: self.compareImage("testStateAnim2", False),
                 lambda: self.anim.getState() == "STATE2",
                 lambda: self.compareImage("testStateAnim3", False),
                 lambda: self.assert_(self.__state2CallbackCalled),
                 lambda: self.anim.getState() == "STATE3",
                 lambda: self.compareImage("testStateAnim4", False),
                 lambda: self.anim.setState("STATE1"),
                 lambda: self.assert_(anim.getNumRunningAnims() == 1),
                 lambda: self.compareImage("testStateAnim5", False)
                ))

    def testParallelAnim(self):
        def animStopped():
            self.__endCalled = True

        def startAnim():
            node0 = Player.getElementByID("mainimg")
            node1 = Player.getElementByID("test")
            node2 = Player.getElementByID("test1")
            self.anim = anim.ParallelAnim(
                    [ anim.SplineAnim(node1, "x", 400, 0, 40, 0, 0),
                      anim.EaseInOutAnim(node2, "x", 300, 129, 99, 100, 100)
                    ], animStopped)
            self.anim.start()
        
        anim.init(avg)
        self.__endCalled = False
        Player.setFakeFPS(10)
        self.initDefaultImageScene()
        self.start((
                 startAnim,
                 lambda: self.assert_(anim.getNumRunningAnims() == 2),
                 lambda: self.assert_(not(self.anim.isDone())),
                 lambda: self.compareImage("testParallelAnims1", False),
                 None,
                 None,
                 lambda: self.compareImage("testParallelAnims2", False),
                 lambda: self.assert_(self.anim.isDone()),
                 lambda: self.assert_(self.__endCalled)
                ))

    def testDraggable(self):
        def onDragStart(event):
            self.__dragStartCalled = True
        
        def onDragEnd(event):
            self.__dragEndCalled = True
        
        def startDrag():
            Helper.fakeMouseEvent(avg.CURSORDOWN, True, False, False, 140, 40, 1)
        
        def move():
            Helper.fakeMouseEvent(avg.CURSORMOTION, True, False, False, 150, 50, 1)
        
        def stop():
            Helper.fakeMouseEvent(avg.CURSORUP, False, False, False, 140, 40, 1)
        
        self.__dragEndCalled = False
        self.__dragStartCalled = False
        Helper = Player.getTestHelper()    
        self.initDefaultImageScene()
        draggable.init(avg)
        dragger = draggable.Draggable(Player.getElementByID("test1"),
                onDragStart, onDragEnd)
        dragger.enable()
        self.start((
                 startDrag,
                 lambda: self.assert_(self.__dragStartCalled),
                 move,
                 lambda: self.compareImage("testDraggable1", False),
                 stop,
                 lambda: self.assert_(self.__dragEndCalled),
                 lambda: self.compareImage("testDraggable2", False),
                 dragger.disable,
                 startDrag,
                 move,
                 lambda: self.compareImage("testDraggable2", False)
                ))

    def testRoundedRect(self):
        def setPos():
            self.rect.pos = (20.5, 3.5)

        def setSize():
            self.rect.size = (40, 20)

        def setRadius(r):
            self.rect.radius = r

        def setFill():
            self.rect.fillcolor = "0000FF"
            self.rect.fillopacity = 0.5

        root = self.loadEmptyScene()
        self.rect = geom.RoundedRect(parent=root, pos=(2.5,2.5), 
                size=(64,64), radius=5, color="FF0000")
        self.start((
                 lambda: self.compareImage("testRoundedRect1", True),
                 setPos,
                 lambda: self.compareImage("testRoundedRect2", True),
                 setSize,
                 lambda: self.compareImage("testRoundedRect3", True),
                 lambda: self.assertException(setRadius(15)),
                 lambda: setRadius(10),
                 lambda: self.compareImage("testRoundedRect4", True),
                 setFill,
                 lambda: self.compareImage("testRoundedRect5", True),
                ))

    def testPieSlice(self):
        def changeAttrs():
            self.pieSlice.startangle = -1
            self.pieSlice.endangle = 3.14
            self.pieSlice.radius = 50
            self.pieSlice.pos = (80.5, 60.5)
            self.pieSlice.fillcolor = "00FFFF"
            self.pieSlice.fillopacity = 0.5

        def makeSmall():
            self.pieSlice.radius = 0.6

        root = self.loadEmptyScene()
        self.pieSlice = geom.PieSlice(parent=root, pos=(20.5,20.5), 
                radius=40, startangle=0, endangle=1.57, color="FF0000")
       
        self.start((
                 lambda: self.compareImage("testPieSlice1", True),
                 changeAttrs,
                 lambda: self.compareImage("testPieSlice2", True),
                 makeSmall,
                 lambda: self.compareImage("testPieSlice3", True),
                ))

    def testArc(self):
        def changeAttrs():
            self.arc.startangle = -1
            self.arc.endangle = 3.14
            self.arc.radius = 50
            self.arc.pos = (80.5, 60.5)

        root = self.loadEmptyScene()
        self.arc = geom.Arc(parent=root, pos=(20.5,20.5), 
                radius=40, startangle=0, endangle=1.57, color="FF0000")
       
        self.start((
                 lambda: self.compareImage("testArc1", True),
                 changeAttrs,
                 lambda: self.compareImage("testArc2", True),
                ))

    def testStateMachine(self):
        def atob(oldState, newState):
            self.atobCalled = True

        def btoc():
            self.btocCalled = True

        def btoa(oldState, newState):
            self.btoaCalled = True

        def aEntered():
            self.aEnteredCalled = True

        def aLeft():
            self.aLeftCalled = True

        self.atobCalled = False
        self.btocCalled = False
        self.btoaCalled = False
        self.aLeftCalled = False
        self.aEnteredCalled = False
        machine = statemachine.StateMachine("testmachine", 'A')
        machine.addState('A', {'B': atob, 'nostate': atob}, aEntered, aLeft)
        machine.addState('B', {'C': btoc, 'A': btoa})
        machine.addState('C', {'A': None})
        self.assertException(lambda: machine.addState('C', {'A': None}))
        self.assertException(lambda: machine.changeState('C'))
        self.assertException(lambda: machine.changeState('nostate'))
        machine.changeState('B')
        self.assert_(self.atobCalled)
        self.assert_(self.aLeftCalled)
        machine.changeState('A')
        self.assert_(self.aEnteredCalled)
        self.assert_(self.btoaCalled)
        machine.changeState('B')
        machine.changeState('C')
        self.assert_(self.btocCalled)
        machine.changeState('A')
        self.assert_(machine.state == 'A')
        self.assertException(lambda: machine.addState('illegal', {}))

        # Create a machine without transition callbacks
        machine = statemachine.StateMachine("testmachine", 'A')
        machine.addState('A', ('B',), aEntered, aLeft)
        machine.addState('B', ('C', 'A'))
        machine.addState('C', ('A',))
        machine.changeState('B')

        # Make a machine with a transition to a nonexistent state.
        kaputtMachine = statemachine.StateMachine("kaputt", 'A')
        kaputtMachine.addState('A', {'B': None})
        self.assertException(lambda: kaputtMachine.changeState('B'))


def pythonTestSuite(tests):
    availableTests = (
        "testLinearAnim",
        "testLinearAnimZeroDuration",
        "testEaseInOutAnim",
        "testSplineAnim",
        "testContinuousAnim",
        "testWaitAnim",
        "testParallelAnim",
        "testStateAnim",
        "testDraggable",
        "testRoundedRect",
        "testPieSlice",
        "testArc",
        "testStateMachine",
        )
    
    return createAVGTestSuite(availableTests, PythonTestCase, tests)

Player = avg.Player.get()
anim.init(avg)
