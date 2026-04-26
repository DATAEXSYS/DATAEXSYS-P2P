import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import "../theme"
import "../components"

Item {
    Colors { id: theme }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 30
        spacing: 20

        RowLayout {
            Layout.fillWidth: true
            ColumnLayout {
                spacing: 5
                Text {
                    text: "Available Networks"
                    color: theme.text
                    font.pixelSize: 24
                    font.weight: Font.Bold
                }
                Text {
                    text: "Select a network to join the mesh swarm."
                    color: theme.muted
                    font.pixelSize: 14
                }
            }
            Item { Layout.fillWidth: true }
            Button {
                text: "+ Create Network"
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                }
                background: Rectangle {
                    implicitWidth: 140
                    implicitHeight: 40
                    color: theme.primary
                    radius: 8
                }
                onClicked: createDialog.open()
            }
        }

        GridView {
            id: networkGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            cellWidth: 320
            cellHeight: 180
            model: appController.networkModel
            clip: true

            delegate: NetworkCard {
                width: 300
                height: 160
                name: model.name
                type: model.type
                peers: model.activePeers
                description: model.description
            }
        }
    }

    Dialog {
        id: createDialog
        anchors.centerIn: parent
        width: 400
        modal: true
        title: "Create New Network"
        
        background: Rectangle {
            color: theme.panel
            border.color: theme.border
            radius: 12
        }

        header: Text {
            text: "  NEW NETWORK"
            color: theme.text
            font.pixelSize: 16
            font.weight: Font.Bold
            padding: 20
        }

        ColumnLayout {
            width: parent.width - 40
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 15
            
            TextField {
                id: nameInput
                placeholderText: "Network Name"
                Layout.fillWidth: true
            }
            TextField {
                id: typeInput
                placeholderText: "Type (e.g. Messaging)"
                Layout.fillWidth: true
            }
            TextField {
                id: descInput
                placeholderText: "Description"
                Layout.fillWidth: true
            }

            Button {
                text: "CREATE"
                Layout.fillWidth: true
                Layout.preferredHeight: 45
                onClicked: {
                    appController.createNetwork(nameInput.text, typeInput.text, descInput.text)
                    createDialog.close()
                }
            }
        }
    }
}
