import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#0B0F1A"
    radius: 8
    border.color: "#1F2937"

    ListModel { id: chatModel }
    ListModel { id: logModel }
    property string activeNode: "Node A"

    Connections {
        target: appController
        function onLogEvent(tag, message) {
            logModel.append({
                "time": new Date().toLocaleTimeString(),
                "tag": tag,
                "message": message
            })
            if (logModel.count > 100) logModel.remove(0)
            logList.positionViewAtEnd()
        }
        function onMessageSent(from, to, text) {
            chatModel.append({
                "from": "Me",
                "to": to,
                "text": text,
                "status": "Sent",
                "timestamp": new Date().toLocaleTimeString()
            })
            chatList.positionViewAtEnd()
        }
        function onRealMessageReceived(ip, text) {
            chatModel.append({
                "from": ip,
                "to": "Me",
                "text": text,
                "status": "Received",
                "timestamp": new Date().toLocaleTimeString()
            })
            chatList.positionViewAtEnd()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15

        Text {
            text: "P2P MESSAGING INTERFACE"
            color: "#64748B"
            font.pixelSize: 12
            font.weight: Font.Bold
            font.letterSpacing: 2
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Text {
                text: "DESTINATION IP:"
                color: "#64748B"
                font.weight: Font.Bold
                font.pixelSize: 11
                Layout.preferredWidth: 120
            }
            
            TextField {
                id: destIpInput
                Layout.fillWidth: true
                placeholderText: "Enter IPv6 Address (e.g. ::1)"
                color: "white"
                text: "::1"
                background: Rectangle { color: "#111827"; radius: 4; border.color: "#1F2937" }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Text {
                text: "INTERMEDIATE IP:"
                color: "#64748B"
                font.weight: Font.Bold
                font.pixelSize: 11
                Layout.preferredWidth: 120
            }
            
            TextField {
                id: intermediateIpInput
                Layout.fillWidth: true
                placeholderText: "(Optional) Enter Router IPv6 Address"
                color: "white"
                background: Rectangle { color: "#111827"; radius: 4; border.color: "#1F2937" }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Text {
                text: "ATTACK SIMULATION:"
                color: "#EF4444"
                font.weight: Font.Bold
                font.pixelSize: 11
                Layout.preferredWidth: 120
            }
            
            Switch {
                id: blackholeSwitch
                checked: appController.blackholeEnabled
                onToggled: appController.blackholeEnabled = checked
            }
            Text {
                text: "Blackhole Attack (Drop Routed Packets)"
                color: blackholeSwitch.checked ? "#EF4444" : "#64748B"
                font.pixelSize: 11
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            
            Text {
                text: "BYPASS SIMULATION:"
                color: "#A855F7"
                font.weight: Font.Bold
                font.pixelSize: 11
                Layout.preferredWidth: 120
            }
            
            Switch {
                id: wormholeSwitch
                checked: appController.wormholeEnabled
                onToggled: appController.wormholeEnabled = checked
            }
            Text {
                text: "Wormhole Attack (Direct Send, Bypass Router)"
                color: wormholeSwitch.checked ? "#A855F7" : "#64748B"
                font.pixelSize: 11
            }
        }

        ListView {
            id: chatList
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: chatModel
            spacing: 10
            clip: true
            delegate: Item {
                width: chatList.width
                height: 70
                
                Rectangle {
                    anchors.right: from === "Me" ? parent.right : undefined
                    anchors.left: from !== "Me" ? parent.left : undefined
                    width: chatList.width * 0.7
                    height: 60
                    color: from === "Me" ? "#1E293B" : (from === "Blackhole" ? "#3F181D" : (from.indexOf("WORMHOLE") !== -1 ? "#2E1A40" : (from.indexOf("Sybil") !== -1 ? "#3F2605" : (from.indexOf("Network ACK") !== -1 || from.indexOf("Router") !== -1 ? "#1A1A1A" : "#111827"))))
                    radius: 10
                    border.color: from === "Me" ? "#4DA3FF" : (from === "Blackhole" ? "#EF4444" : (from.indexOf("WORMHOLE") !== -1 ? "#A855F7" : (from.indexOf("Sybil") !== -1 ? "#F59E0B" : (from.indexOf("Network ACK") !== -1 ? "#3DFFB3" : "#1F2937"))))

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 2

                        RowLayout {
                            Layout.fillWidth: true
                            Text { 
                                text: from; 
                                color: from === "Me" ? "#4DA3FF" : (from === "Blackhole" ? "#EF4444" : (from.indexOf("WORMHOLE") !== -1 ? "#D8B4FE" : (from.indexOf("Sybil") !== -1 ? "#F59E0B" : (from.indexOf("Network ACK") !== -1 ? "#3DFFB3" : "#94A3B8")))); 
                                font.weight: Font.Bold; 
                                font.pixelSize: 10 
                            }
                            Item { Layout.fillWidth: true }
                            Text { 
                                text: status; 
                                color: from === "Blackhole" ? "#EF4444" : (from.indexOf("WORMHOLE") !== -1 ? "#A855F7" : (from.indexOf("Sybil") !== -1 ? "#F59E0B" : (status === "Received" ? "#3DFFB3" : "#4DA3FF"))); 
                                font.pixelSize: 9; 
                                font.italic: true 
                            }
                        }

                        Text {
                            Layout.fillWidth: true
                            text: text
                            color: "#E2E8F0"
                            wrapMode: Text.WordWrap
                        }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            color: "#111827"
            border.color: "#1F2937"
            radius: 4

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 4

                Text { text: "CONSOLE LOGS"; color: "#38bdf8"; font.pixelSize: 11; font.bold: true }

                ListView {
                    id: logList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: logModel
                    clip: true
                    spacing: 2
                    delegate: Text {
                        width: parent.width
                        text: time + " [" + tag + "] " + message
                        color: tag === "ATTACK" ? "#EF4444" : (tag === "ROUTING" ? "#F59E0B" : (tag.indexOf("ERR") !== -1 ? "#EF4444" : "#9CA3AF"))
                        font.pixelSize: 10
                        wrapMode: Text.WordWrap
                        font.family: "monospace"
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: msgInput
                Layout.fillWidth: true
                placeholderText: "Type a message..."
                color: "white"
                background: Rectangle { color: "#111827"; radius: 4; border.color: "#1F2937" }
            }
            Button {
                text: "SYBIL FLOOD"
                onClicked: {
                    if (destIpInput.text !== "") {
                        appController.launchSybilAttack(destIpInput.text, msgInput.text)
                    }
                }
                background: Rectangle { color: "#F59E0B"; radius: 4 }
                contentItem: Text { text: parent.text; color: "white"; font.weight: Font.Bold; horizontalAlignment: Text.AlignHCenter }
            }
            Button {
                text: "SEND"
                onClicked: {
                    if (msgInput.text !== "" && destIpInput.text !== "") {
                        appController.sendRoutedMessage(destIpInput.text, intermediateIpInput.text, msgInput.text)
                        msgInput.text = ""
                    }
                }
                background: Rectangle { color: "#4DA3FF"; radius: 4 }
                contentItem: Text { text: parent.text; color: "white"; font.weight: Font.Bold; horizontalAlignment: Text.AlignHCenter }
            }
        }
    }
}
