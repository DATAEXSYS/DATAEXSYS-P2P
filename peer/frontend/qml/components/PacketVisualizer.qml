import QtQuick 2.15

Item {
    id: root
    width: 30
    height: 30
    visible: false

    property var nodePositions: ({
        "Node A": { x: 150, y: 350 },
        "Node B": { x: 450, y: 150 },
        "Node C": { x: 750, y: 350 }
    })

    Rectangle {
        anchors.centerIn: parent
        width: 20
        height: 20
        radius: 10
        color: "#00F2FF"
        
        // Glow
        Rectangle {
            anchors.centerIn: parent
            width: 30
            height: 30
            radius: 15
            color: "#00F2FF"
            opacity: 0.3
            
            SequentialAnimation on scale {
                loops: Animation.Infinite
                NumberAnimation { from: 1.0; to: 1.5; duration: 500 }
                NumberAnimation { from: 1.5; to: 1.0; duration: 500 }
            }
        }
    }

    // Dynamic Crypto Label
    Rectangle {
        id: labelContainer
        anchors.bottom: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 15
        width: stageLabel.width + 20
        height: 25
        color: "#1E293B"
        radius: 4
        border.color: "#00F2FF"
        
        Text {
            id: stageLabel
            anchors.centerIn: parent
            text: ""
            color: "#00F2FF"
            font.pixelSize: 10
            font.weight: Font.Bold
        }

        SequentialAnimation on opacity {
            id: labelPulse
            loops: Animation.Infinite
            NumberAnimation { from: 1.0; to: 0.6; duration: 800 }
            NumberAnimation { from: 0.6; to: 1.0; duration: 800 }
        }
    }

    Connections {
        target: appController
        function onPacketEntered(source, destination) {
            var pos = root.nodePositions[source]
            root.x = pos.x - root.width/2
            root.y = pos.y - root.height/2
            root.visible = true
            stageLabel.text = "ENCRYPTING..."
        }
        function onPacketHop(from, to, stage) {
            var targetPos = root.nodePositions[to]
            stageLabel.text = stage
            
            moveAnimX.to = targetPos.x - root.width/2
            moveAnimY.to = targetPos.y - root.height/2
            moveGroup.start()
        }
        function onPacketDelivered(nodeId) {
            stageLabel.text = "DECRYPTING & DELIVERED"
            fadeAnim.start()
        }
    }

    ParallelAnimation {
        id: moveGroup
        NumberAnimation { id: moveAnimX; target: root; property: "x"; duration: 1500; easing.type: Easing.InOutQuad }
        NumberAnimation { id: moveAnimY; target: root; property: "y"; duration: 1500; easing.type: Easing.InOutQuad }
    }

    SequentialAnimation {
        id: fadeAnim
        PauseAnimation { duration: 1000 }
        NumberAnimation { target: root; property: "opacity"; from: 1.0; to: 0.0; duration: 1000 }
        PropertyAction { target: root; property: "visible"; value: false }
        PropertyAction { target: root; property: "opacity"; value: 1.0 }
    }
}
