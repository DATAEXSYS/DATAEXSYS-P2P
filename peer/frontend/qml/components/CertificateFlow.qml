import QtQuick 2.15

Item {
    id: root
    anchors.fill: parent

    property var nodePositions: ({
        "Node A": { x: 100, y: 300 },
        "Node B": { x: 400, y: 150 },
        "Node C": { x: 700, y: 300 }
    })

    Connections {
        target: appController
        function onCertificateIssued(nodeId, certId) {
            var pos = root.nodePositions[nodeId]
            var ripple = rippleComponent.createObject(root, {
                "x": pos.x,
                "y": pos.y
            })
        }
    }

    Component {
        id: rippleComponent
        Rectangle {
            id: ripple
            width: 1
            height: 1
            radius: 0.5
            color: "transparent"
            border.color: "#3DFFB3"
            border.width: 2
            
            SequentialAnimation {
                running: true
                ParallelAnimation {
                    NumberAnimation { target: ripple; property: "width"; from: 1; to: 300; duration: 1500; easing.type: Easing.OutQuad }
                    NumberAnimation { target: ripple; property: "height"; from: 1; to: 300; duration: 1500; easing.type: Easing.OutQuad }
                    NumberAnimation { target: ripple; property: "x"; from: x; to: x - 150; duration: 1500; easing.type: Easing.OutQuad }
                    NumberAnimation { target: ripple; property: "y"; from: y; to: y - 150; duration: 1500; easing.type: Easing.OutQuad }
                    NumberAnimation { target: ripple; property: "opacity"; from: 1.0; to: 0.0; duration: 1500; easing.type: Easing.OutQuad }
                }
                ScriptAction { script: ripple.destroy() }
            }
        }
    }
}
