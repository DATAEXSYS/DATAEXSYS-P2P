import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#0B0F1A"
    radius: 8
    border.color: "#1F2937"

    ListModel { id: blockModel }

    Connections {
        target: appController
        function onBlockMined(index, hash, prevHash) {
            blockModel.append({
                "index": index,
                "hash": hash,
                "prevHash": prevHash,
                "timestamp": new Date().toLocaleTimeString()
            })
            // Smoothly scroll to the latest block
            view.positionViewAtEnd()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15

        Text {
            text: "DISTRIBUTED LEDGER (BLOCKCHAIN)"
            color: "#64748B"
            font.pixelSize: 12
            font.weight: Font.Bold
            font.letterSpacing: 2
        }

        ListView {
            id: view
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: blockModel
            orientation: ListView.Horizontal
            spacing: 40
            delegate: Item {
                width: 200
                height: 180
                
                // Link to previous block
                Rectangle {
                    visible: index > 0
                    width: 40
                    height: 2
                    color: "#3DFFB3"
                    x: -40
                    y: 90
                    opacity: 0.5
                }

                Rectangle {
                    anchors.fill: parent
                    color: "#111827"
                    radius: 10
                    border.color: "#3DFFB3"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 15
                        spacing: 10

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 25
                            color: "#059669"
                            radius: 4
                            Text {
                                anchors.centerIn: parent
                                text: "BLOCK #" + index
                                color: "white"
                                font.weight: Font.Bold
                                font.pixelSize: 10
                            }
                        }

                        Column {
                            spacing: 5
                            Text { text: "HASH"; color: "#6B7280"; font.pixelSize: 9; font.weight: Font.Bold }
                            Text { 
                                text: hash.substring(0, 16) + "..."; 
                                color: "#3DFFB3"; 
                                font.family: "Monospace"; 
                                font.pixelSize: 10 
                            }
                        }

                        Column {
                            spacing: 5
                            Text { text: "PREV HASH"; color: "#6B7280"; font.pixelSize: 9; font.weight: Font.Bold }
                            Text { 
                                text: prevHash.substring(0, 16) + "..."; 
                                color: "#94A3B8"; 
                                font.family: "Monospace"; 
                                font.pixelSize: 10 
                            }
                        }

                        Text {
                            Layout.alignment: Qt.AlignRight
                            text: timestamp
                            color: "#475569"
                            font.pixelSize: 10
                        }
                    }

                    // Mining Glow Animation
                    SequentialAnimation on opacity {
                        running: index === blockModel.count - 1
                        NumberAnimation { from: 0.5; to: 1.0; duration: 500 }
                    }
                }
            }
        }
        
        Button {
            Layout.alignment: Qt.AlignRight
            text: "MINE NEW BLOCK"
            onClicked: appController.mineBlock()
            background: Rectangle { color: "#059669"; radius: 4 }
            contentItem: Text { text: parent.text; color: "white"; font.weight: Font.Bold; horizontalAlignment: Text.AlignHCenter }
        }
    }
}
