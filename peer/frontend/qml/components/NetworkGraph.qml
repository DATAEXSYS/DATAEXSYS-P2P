import QtQuick 2.15

Item {
    id: root

    property var nodes: []
    property var route: []
    property string sourceNode: "Node A"
    property string destNode: "Node C"
    
    property var nodePositions: ({
        "Node A": { x: 150, y: 350 },
        "Node B": { x: 450, y: 150 },
        "Node C": { x: 750, y: 350 }
    })

    Connections {
        target: appController
        function onNodeJoined(nodeId) {
            var newNodes = root.nodes
            newNodes.push(nodeId)
            root.nodes = newNodes
        }
        function onRouteSelected(path) {
            root.route = path
            canvas.requestPaint()
        }
    }

    Canvas {
        id: canvas
        anchors.fill: parent
        opacity: 0.8
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            
            // Draw all possible links
            ctx.lineWidth = 1
            ctx.strokeStyle = "#1F2937"
            ctx.beginPath()
            ctx.moveTo(150, 350); ctx.lineTo(450, 150)
            ctx.lineTo(750, 350)
            ctx.stroke()

            // Draw active route
            if (root.route.length < 2) return

            ctx.lineWidth = 4
            ctx.strokeStyle = "#4DA3FF"
            ctx.shadowBlur = 10
            ctx.shadowColor = "#4DA3FF"
            ctx.beginPath()
            
            var firstNode = root.nodePositions[root.route[0]]
            ctx.moveTo(firstNode.x, firstNode.y)
            
            for (var i = 1; i < root.route.length; i++) {
                var pos = root.nodePositions[root.route[i]]
                ctx.lineTo(pos.x, pos.y)
            }
            ctx.stroke()
        }
    }

    // Data Flow Particles
    Repeater {
        model: root.route.length > 1 ? root.route.length - 1 : 0
        delegate: Item {
            property var start: root.nodePositions[root.route[index]]
            property var end: root.nodePositions[root.route[index+1]]

            Rectangle {
                width: 6; height: 6; radius: 3; color: "#00F2FF"
                x: start.x - 3; y: start.y - 3
                
                SequentialAnimation on x {
                    loops: Animation.Infinite
                    NumberAnimation { from: start.x - 3; to: end.x - 3; duration: 2000; easing.type: Easing.Linear }
                }
                SequentialAnimation on y {
                    loops: Animation.Infinite
                    NumberAnimation { from: start.y - 3; to: end.y - 3; duration: 2000; easing.type: Easing.Linear }
                }
                SequentialAnimation on opacity {
                    loops: Animation.Infinite
                    NumberAnimation { from: 1.0; to: 0.0; duration: 2000; easing.type: Easing.InQuad }
                }
            }
        }
    }

    Repeater {
        model: nodes
        delegate: Item {
            x: root.nodePositions[modelData].x - 40
            y: root.nodePositions[modelData].y - 40
            width: 80
            height: 80

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (mouse.button === Qt.LeftButton) sourceNode = modelData
                    else if (mouse.button === Qt.RightButton) destNode = modelData
                }
            }

            Rectangle {
                anchors.centerIn: parent
                width: 50
                height: 50
                radius: 25
                color: "#0B0F1A"
                border.color: modelData === sourceNode ? "#00F2FF" : (modelData === destNode ? "#FF4D4D" : "#4DA3FF")
                border.width: 3

                Rectangle {
                    anchors.fill: parent
                    radius: 25
                    color: border.color
                    opacity: 0.1
                }

                // Inner pulse
                Rectangle {
                    anchors.centerIn: parent
                    width: 46
                    height: 46
                    radius: 23
                    color: "transparent"
                    border.color: parent.border.color
                    border.width: 1
                    opacity: 0.5

                    SequentialAnimation on scale {
                        loops: Animation.Infinite
                        NumberAnimation { from: 1.0; to: 1.2; duration: 1500; easing.type: Easing.InOutQuad }
                        NumberAnimation { from: 1.2; to: 1.0; duration: 1500; easing.type: Easing.InOutQuad }
                    }
                }
            }

            Text {
                anchors.top: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                text: modelData + (modelData === sourceNode ? " [SRC]" : (modelData === destNode ? " [DST]" : ""))
                color: "#E5E7EB"
                font.pixelSize: 10
                font.weight: Font.Bold
            }
        }
    }
}
