import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Simulator 1.0

Rectangle {
    id: root
    color: "#0f172a"

    SimulatorEngine {
        id: simEngine
        onTogglesChanged: updateDashboard()
        onAttackModeChanged: updateDashboard()
        onMessageSent: function(msgData) {
            chatModel.append({
                "from": msgData.from,
                "msg": msgData.msg,
                "trust": msgData.trust.toFixed(2),
                "route": msgData.route,
                "hash": msgData.hash || ""
            })
            chatList.positionViewAtEnd()
        }
        onTrustUpdated: updateDashboard()
        onBlockMined: function(block) {
            certModel.append({"hash": block.hash, "time": block.timestamp})
        }
        onAnomalyDetected: function(reason) {
            anomalyModel.append({"time": new Date().toLocaleTimeString(), "reason": reason})
        }
    }

    ListModel { id: trustModel }
    ListModel { id: certModel }
    ListModel { id: chatModel }
    ListModel { id: anomalyModel }

    function updateDashboard() {
        trustModel.clear()
        let scores = simEngine.getTrustScores()
        for (let i=0; i<scores.length; i++) trustModel.append(scores[i])
    }

    Component.onCompleted: updateDashboard()

    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        // LEFT PANEL: Controls
        Rectangle {
            Layout.preferredWidth: 300
            Layout.fillHeight: true
            color: "#1e293b"
            radius: 8
            border.color: "#334155"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 15

                Text { text: "MODULE CONTROLS"; color: "#38bdf8"; font.pixelSize: 16; font.bold: true }

                RowLayout {
                    Switch { checked: simEngine.trustEnabled; onToggled: simEngine.trustEnabled = checked }
                    Text { text: "LocalTrustDiaries"; color: "white" }
                }
                RowLayout {
                    Switch { checked: simEngine.certEnabled; onToggled: simEngine.certEnabled = checked }
                    Text { text: "PKCertChain"; color: "white" }
                }
                RowLayout {
                    Switch { checked: simEngine.wormholeEnabled; onToggled: simEngine.wormholeEnabled = checked }
                    Text { text: "Wormhole Simulator"; color: "white" }
                }
                RowLayout {
                    Switch { checked: simEngine.dsrEnabled; onToggled: simEngine.dsrEnabled = checked }
                    Text { text: "DSR Routing"; color: "white" }
                }
                RowLayout {
                    Switch { checked: simEngine.rollingEnabled; onToggled: simEngine.rollingEnabled = checked }
                    Text { text: "Rolling Signatures"; color: "white" }
                }

                Rectangle { height: 1; Layout.fillWidth: true; color: "#334155" }

                RowLayout {
                    Switch { 
                        checked: simEngine.attackMode; 
                        onToggled: simEngine.attackMode = checked 
                        indicator: Rectangle {
                            width: 40; height: 20; radius: 10
                            color: simEngine.attackMode ? "#ef4444" : "#475569"
                            Rectangle { x: simEngine.attackMode ? 20 : 0; width: 20; height: 20; radius: 10; color: "white" }
                        }
                    }
                    Text { text: "ATTACK SIMULATION"; color: "#ef4444"; font.bold: true }
                }

                Item { Layout.fillHeight: true }

                Button {
                    text: "Mine Certificate PoW"
                    Layout.fillWidth: true
                    onClicked: simEngine.simulatePow()
                    background: Rectangle { color: "#8b5cf6"; radius: 4 }
                    contentItem: Text { text: parent.text; color: "white"; horizontalAlignment: Text.AlignHCenter; font.bold: true }
                }
            }
        }

        // CENTER PANEL: Chat & Routing
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#1e293b"
            radius: 8
            border.color: "#334155"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15

                Text { text: "SECURE CHAT"; color: "#38bdf8"; font.pixelSize: 16; font.bold: true }

                ListView {
                    id: chatList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: chatModel
                    spacing: 10
                    clip: true
                    delegate: Rectangle {
                        width: parent.width
                        height: col.height + 16
                        color: "#0f172a"
                        border.color: simEngine.attackMode && index % 3 == 0 ? "#ef4444" : "#334155"
                        radius: 6
                        Column {
                            id: col
                            x: 8; y: 8
                            width: parent.width - 16
                            spacing: 4
                            RowLayout {
                                width: parent.width
                                Text { text: from; color: "#38bdf8"; font.bold: true }
                                Text { text: "(Trust: " + trust + ")"; color: trust < 0.5 ? "#ef4444" : "#22c55e"; font.pixelSize: 10 }
                                Item { Layout.fillWidth: true }
                                Text { text: hash; color: "#8b5cf6"; font.family: "monospace" }
                            }
                            Text { text: msg; color: "white"; wrapMode: Text.WordWrap; width: parent.width }
                            Text { text: "Path: " + route; color: "#64748B"; font.pixelSize: 10; width: parent.width }
                        }
                    }
                }

                RowLayout {
                    TextField {
                        id: chatInput
                        Layout.fillWidth: true
                        placeholderText: "Message..."
                        color: "white"
                        background: Rectangle { color: "#0f172a"; radius: 4; border.color: "#334155" }
                    }
                    Button {
                        text: "Send"
                        onClicked: {
                            if (chatInput.text !== "") {
                                simEngine.sendMessage("NodeA", "NodeD", chatInput.text)
                                chatInput.text = ""
                            }
                        }
                        background: Rectangle { color: "#38bdf8"; radius: 4 }
                        contentItem: Text { text: parent.text; color: "white"; font.bold: true }
                    }
                }
            }
        }

        // RIGHT PANEL: Visualizer
        Rectangle {
            Layout.preferredWidth: 350
            Layout.fillHeight: true
            color: "#1e293b"
            radius: 8
            border.color: "#334155"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 10

                Text { text: "SECURITY DASHBOARD"; color: "#38bdf8"; font.pixelSize: 16; font.bold: true }

                Text { text: "Trust Evolution"; color: "white"; font.bold: true }
                ListView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120
                    model: trustModel
                    delegate: RowLayout {
                        width: parent.width
                        Text { text: node; color: "white"; Layout.preferredWidth: 60 }
                        Rectangle {
                            Layout.fillWidth: true
                            height: 10; color: "#0f172a"; radius: 5
                            Rectangle {
                                width: parent.width * score; height: 10; radius: 5
                                color: score < 0.5 ? "#ef4444" : "#22c55e"
                                Behavior on width { NumberAnimation { duration: 300 } }
                            }
                        }
                    }
                }

                Text { text: "PKCertChain Blocks"; color: "#8b5cf6"; font.bold: true }
                ListView {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120
                    model: certModel
                    clip: true
                    delegate: Rectangle {
                        width: parent.width; height: 25; color: "#0f172a"; border.color: "#334155"; radius: 4
                        RowLayout {
                            anchors.fill: parent; anchors.margins: 4
                            Text { text: "Blk " + index; color: "#64748B"; font.pixelSize: 10 }
                            Text { text: hash; color: "#8b5cf6"; font.pixelSize: 10; font.family: "monospace" }
                        }
                    }
                }

                Text { text: "Anomalies (RouteCache)"; color: "#ef4444"; font.bold: true }
                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: anomalyModel
                    clip: true
                    delegate: Text {
                        width: parent.width
                        text: time + " - " + reason
                        color: "#ef4444"; font.pixelSize: 10; wrapMode: Text.WordWrap
                    }
                }
            }
        }
    }
}
