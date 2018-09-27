import QtQuick 1.0

Rectangle {
    id : qenesview
    property int sizeWidth:  854
    property int sizeHeight:  480
    property int boxWidth: 250
    property int boxHeight: 150
    property int fatherX:  160
    property int fatherY: 5
    property int motherX: 160
    property int motherY: 160
    property int childSize: 70
    property int currentX: 10 + childSize
    property int currentY: 5
    property bool test: false
    width: sizeWidth
    height: sizeHeight

    signal qmlSignal(string msg)

    function testtest(msg) {
        console.log("This is QML testtest function called from c++")
        currentName.text = msg
    }

    function motherAnimComplete() {
        child.color = current.color
        current.color = mother.color
        setup()
    }

    function fatherAnimComplete() {
        child.color = current.color
        current.color = father.color
        setup()
    }

    function setup() {
        console.log("setup");
        qmlSignal("Signal from QML received in C++")

        current.width = boxWidth
        current.height = sizeHeight - 12
        current.x = currentX
        current.y = currentY
        current.border.color = "black"
        current.border.width = 2
        current.radius = 5

        father.width = boxWidth
        father.height = current.height / 2 - 4
        father.x = current.x + boxWidth + 8
        father.y = fatherY

        mother.width =  boxWidth
        mother.height = father.height
        mother.x = current.x + boxWidth + 8
        mother.y = father.height + 12

        gMother.x = qenesview.width
        gMother.y = qenesview.height
        gMother.scale =  0
        gMother.width = boxWidth
        gMother.height = boxHeight

        gFather.x = qenesview.width
        gFather.y = 0
        gFather.scale = 0
        gFather.width = boxWidth
        gFather.height = boxHeight

        child.x = 5
        child.y = 5
        child.width = childSize
        child.height = childSize

    }

    Rectangle {
        id: child
        color: "#999999"; border.color: "black"; border.width: 2; radius: 5
    }

    Rectangle {
        id: current

        Component.onCompleted: setup();
        color: "#999999"

        Text { id: currentName; x: 5; y: 5; text: "NameFam Name1st"; font.pixelSize: 12 }

        Rectangle {
            id: text_goBack
            x: 5
            y: 290
            width: 87
            height: 30
            color: "#ffffff"
            opacity: 0
            scale : 0

            Text { id: text1; x: 5; y: 5; text: "Back"; font.pixelSize: 12; opacity: 0 }
        }

        MouseArea {
            id: ma_gotoDetail

            width: current.width
            height: current.height
            onClicked: {
                myObject.showMsg("mroo")
                if (qenesview.state == "") qenesview.state = "show_details"
                else qenesview.state = ""
            }
        }
    }


    Rectangle {
        id: father

        color: "#9999ff"; border.color: "black"; border.width: 2; radius: 5

        Text { id: fatherName; x: 5; y: 5; text: "NameFam Name1st"; font.pixelSize: 12; width: 140 }

        MouseArea {
            id: ma_father
            width: father.width
            height: father.height

            onClicked: {
                animFatherToCurrent.start()
                animMotherOut.start()
                animGFatherIn.start()
                animGMotherIn.start()
                animCurrentToChild.start()
                animChildOut.start()
            }
        }
    }

    Rectangle {
        id: mother

        color: "#ff9999"; border.color: "black"; border.width: 2; radius: 5

        Text { id: motherName; x: 5; y: 5; text: "NameFam Name1st"; font.pixelSize: 12 }

        MouseArea {
            id: ma_mother
            width: mother.width
            height: mother.height

            onClicked: {
                animMotherToCurrent.start()
                animFatherOut.start()
                animGFatherIn.start()
                animGMotherIn.start()
                animCurrentToChild.start()
                animChildOut.start()
            }
        }
    }

    Rectangle {
        id: gMother

        color: "#ff9999"; border.color: "black"; border.width: 2; radius: 5

        Text { id: gMotherName; x: 5; y: 5; text: "GMother"; font.pixelSize: 12 }
    }

    Rectangle {
        id: gFather

        color: "#9999ff"; border.color: "black"; border.width: 2; radius: 5

        Text { id: gFatherName; x: 5; y: 5; text: "GFather"; font.pixelSize: 12 }
    }

    ParallelAnimation {
        id: animMotherToCurrent
        NumberAnimation { target: mother; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 550; properties: "x"; to: currentX }
        NumberAnimation { target: mother; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 550; properties: "y"; to: currentY }
        NumberAnimation { target: mother; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 550; properties: "height"; to: sizeHeight - 12 }
        onCompleted: qenesview.motherAnimComplete()
    }

    ParallelAnimation {
        id: animFatherToCurrent
        NumberAnimation { target: father; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 550; properties: "x"; to: currentX }
        NumberAnimation { target: father; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 550; properties: "y"; to: currentY }
        NumberAnimation { target: father; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 550; properties: "height"; to: sizeHeight - 12 }
        onCompleted: qenesview.fatherAnimComplete()
    }

    ParallelAnimation {
        id: animFatherOut
        NumberAnimation { target: father; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration:  500; properties: "x"; to: currentX }
        NumberAnimation { target: father; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration:  500; properties: "y"; to: -400 }
    }

    ParallelAnimation {
        id: animMotherOut
        NumberAnimation { target: mother; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration:  500; properties: "x"; to: currentX }
        NumberAnimation { target: mother; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration:  500; properties: "y"; to: sizeHeight+400 }
    }

    ParallelAnimation {
        id: animGMotherIn
        NumberAnimation { target: gMother; properties: "x"; to: currentX + boxWidth + 8; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 500 }
        NumberAnimation { target: gMother; properties: "y"; to: (sizeHeight - 12) / 2 + 8; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 500 }
        NumberAnimation { target: gMother; properties: "height"; to: (sizeHeight - 12) / 2 - 4; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 500 }
        NumberAnimation { target: gMother; properties: "width"; to: boxWidth; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 500 }
        NumberAnimation { target: gMother; properties: "scale"; to: 1; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 500 }
    }

    ParallelAnimation {
        id: animGFatherIn
        NumberAnimation { target: gFather; properties: "x"; to: currentX + boxWidth + 8; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 500 }
        NumberAnimation { target: gFather; properties: "y"; to: fatherY; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 500 }
        NumberAnimation { target: gFather; properties: "height"; to: (sizeHeight - 12) / 2 - 4; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 500 }
        NumberAnimation { target: gFather; properties: "width"; to: boxWidth; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 500 }
        NumberAnimation { target: gFather; properties: "scale"; to: 1; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 500 }
    }

    ParallelAnimation {
        id: animCurrentToChild
        NumberAnimation { target: current; properties: "x"; to: 5; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 499 }
        NumberAnimation { target: current; properties: "y"; to: 5; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 499 }
        NumberAnimation { target: current; properties: "width"; to: childSize; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 499 }
        NumberAnimation { target: current; properties: "height"; to: childSize; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 499 }
    }

    ParallelAnimation {
        id : animChildOut
        NumberAnimation { target: child; properties: "x"; to: -100; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 499 }
        NumberAnimation { target: child; properties: "y"; to: -100; alwaysRunToEnd: true; easing.type: Easing.OutBounce; duration: 499 }
    }

    states: [

            State {
                name: "show_details"

                PropertyChanges {
                    target: father
                    x: sizeWidth
                }

                PropertyChanges {
                    target: mother
                    x: sizeWidth
                }

                PropertyChanges {
                    target: current
                    x: 10
                    y: 10
                    width: sizeWidth - 20
                    height: sizeHeight - 20
                }

                PropertyChanges {
                    target: ma_gotoDetail
                    x: 5
                    y: 291
                    width: boxWidth
                    height: boxHeight
                }

                PropertyChanges {
                    target: text_goBack
                    x: 5
                    y: 290
                    width: 87
                    height: 30
                    opacity: 1
                    scale : 1
                }

                PropertyChanges {
                    target: text1
                    opacity: 1
                }

                PropertyChanges {
                    target: child
                    x:-100
                }
            }
    ]
    transitions: Transition {
        reversible: false
        NumberAnimation {
            duration: 500
            easing.type: Easing.InOutCubic
            properties: "x, y, width, height"
        }
    }
}

