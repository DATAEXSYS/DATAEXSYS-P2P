import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#0B0F1A"
    radius: 8
    border.color: "#1F2937"

    ListModel { id: chatModel }
    property string activeNode: "Node A"

    Connections {
        target: appController
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
                    anchors.right: from === activeNode ? parent.right : undefined
                    anchors.left: from !== activeNode ? parent.left : undefined
                    width: chatList.width * 0.7
                    height: 60
                    color: from === activeNode ? "#1E293B" : "#111827"
                    radius: 10
                    border.color: from === activeNode ? "#4DA3FF" : "#1F2937"

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 2

                        RowLayout {
                            Layout.fillWidth: true
                            Text { 
                                text: from; 
                                color: from === activeNode ? "#4DA3FF" : "#94A3B8"; 
                                font.weight: Font.Bold; 
                                font.pixelSize: 10 
                            }
                            Item { Layout.fillWidth: true }
                            Text { 
                                text: status; 
                                color: status === "Received" ? "#3DFFB3" : "#4DA3FF"; 
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
                text: "SEND"
                onClicked: {
                    if (msgInput.text !== "" && destIpInput.text !== "") {
                        appController.sendRealMessage(destIpInput.text, msgInput.text)
                        msgInput.text = ""
                    }
                }
                background: Rectangle { color: "#4DA3FF"; radius: 4 }
                contentItem: Text { text: parent.text; color: "white"; font.weight: Font.Bold; horizontalAlignment: Text.AlignHCenter }
            }
        }
    }
}
