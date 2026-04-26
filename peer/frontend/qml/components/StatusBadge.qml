import QtQuick 2.15
import "../theme"

Rectangle {
    property string statusText: "OFFLINE"
    property color statusColor: "#FF4D4D"

    width: 100
    height: 28
    radius: 14
    color: Qt.rgba(statusColor.r, statusColor.g, statusColor.b, 0.1)
    border.color: Qt.rgba(statusColor.r, statusColor.g, statusColor.b, 0.3)
    border.width: 1

    Row {
        anchors.centerIn: parent
        spacing: 8
        Rectangle {
            width: 8; height: 8
            radius: 4
            color: statusColor
            anchors.verticalCenter: parent.verticalCenter
            
            SequentialAnimation on opacity {
                loops: Animation.Infinite
                NumberAnimation { from: 1.0; to: 0.4; duration: 1000 }
                NumberAnimation { from: 0.4; to: 1.0; duration: 1000 }
            }
        }
        Text {
            text: statusText
            color: statusColor
            font.pixelSize: 11
            font.weight: Font.Bold
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
