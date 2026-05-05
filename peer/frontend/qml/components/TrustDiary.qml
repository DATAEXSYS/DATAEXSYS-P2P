import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#0D1117"
    radius: 8
    border.color: "#30363D"

    ListModel { id: trustModel }

    Connections {
        target: appController
        function onNodeJoined(nodeId) {
            trustModel.append({
                "nodeId": nodeId,
                "trust": 0.0,
                "cert": "None",
                "lastEvent": "Joined"
            })
        }
        function onTrustUpdated(nodeId, score) {
            for (let i = 0; i < trustModel.count; ++i) {
                if (trustModel.get(i).nodeId === nodeId) {
                    trustModel.setProperty(i, "trust", score)
                    trustModel.setProperty(i, "lastEvent", "Trust recalculated")
                    break
                }
            }
        }
        function onCertificateIssued(nodeId, certId) {
            for (let i = 0; i < trustModel.count; ++i) {
                if (trustModel.get(i).nodeId === nodeId) {
                    trustModel.setProperty(i, "cert", certId)
                    trustModel.setProperty(i, "lastEvent", "Cert issued")
                    break
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 15

        Text {
            text: "LOCAL TRUST DIARY"
            color: "#8B949E"
            font.pixelSize: 12
            font.weight: Font.Bold
            font.letterSpacing: 1.5
        }

        ListView {
            id: trustList
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: trustModel
            spacing: 10
            delegate: Rectangle {
                width: trustList.width
                height: 60
                color: "#161B22"
                radius: 6
                border.color: "#30363D"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4

                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: nodeId; color: "#E6EDF3"; font.weight: Font.Bold }
                        Item { Layout.fillWidth: true }
                        Text { 
                            text: (trust * 100).toFixed(1) + "%"; 
                            color: trust > 0.7 ? "#3DFFB3" : (trust > 0.4 ? "#FFD700" : "#FF4D4D")
                            font.family: "Monospace"
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 6
                        color: "#0D1117"
                        radius: 3
                        Rectangle {
                            width: parent.width * trust
                            height: parent.height
                            radius: 3
                            color: trust > 0.7 ? "#3DFFB3" : (trust > 0.4 ? "#FFD700" : "#FF4D4D")
                            Behavior on width { NumberAnimation { duration: 500 } }
                        }
                    }

                    Text { 
                        text: "CERT: " + cert + " | " + lastEvent
                        color: "#7D8590"
                        font.pixelSize: 10
                    }
                }
            }
        }
    }
}
