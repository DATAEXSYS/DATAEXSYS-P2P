import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#050A15"
    radius: 8
    border.color: "#1A2533"
    clip: true

    ListModel { id: logModel }

    Connections {
        target: appController
        function onLogEvent(tag, message) {
            logModel.append({
                "tag": tag,
                "message": message,
                "timestamp": new Date().toLocaleTimeString()
            })
            listView.positionViewAtEnd()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            color: "#0F172A"
            
            Text {
                anchors.centerIn: parent
                text: "SECURITY & CRYPTO CONSOLE"
                color: "#94A3B8"
                font.pixelSize: 10
                font.weight: Font.Bold
                font.letterSpacing: 2
            }
        }

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: logModel
            delegate: Item {
                width: listView.width
                height: 25
                
                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    spacing: 10

                    Text {
                        text: "[" + tag + "]"
                        color: {
                            if (tag === "CRYPTO") return "#00F2FF"
                            if (tag === "TRUST") return "#FFD700"
                            if (tag === "WARN") return "#FF4D4D"
                            if (tag === "BOOTSTRAP") return "#3DFFB3"
                            return "#64748B"
                        }
                        font.family: "Monospace"
                        font.pixelSize: 11
                        font.weight: Font.Bold
                    }

                    Text {
                        Layout.fillWidth: true
                        text: message
                        color: "#CBD5E1"
                        font.family: "Monospace"
                        font.pixelSize: 11
                    }
                }
            }
            
            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }
        }
    }
}
