import QtQuick 2.15
import QtQuick.Layouts 1.15
import "../theme"

Rectangle {
    id: root
    color: theme.panel
    
    signal pageChanged(string page)

    Colors { id: theme }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 30

        // Logo Section
        RowLayout {
            spacing: 10
            Rectangle {
                width: 32; height: 32
                radius: 6
                color: theme.primary
                Text {
                    anchors.centerIn: parent
                    text: "D"
                    color: "white"
                    font.bold: true
                }
            }
            Text {
                text: "DATAEXSYS"
                color: theme.text
                font.pixelSize: 18
                font.weight: Font.DemiBold
                font.letterSpacing: 1
            }
        }

        // Navigation Items
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 10

            NavButton { label: "Home"; icon: "H"; target: "pages/Home.qml"; active: true }
            NavButton { label: "Networks"; icon: "N"; target: "pages/Networks.qml" }
            NavButton { label: "Peers"; icon: "P"; target: "pages/Peers.qml" }
            NavButton { label: "Settings"; icon: "S"; target: "pages/Settings.qml" }
        }

        Item { Layout.fillHeight: true }

        // System Info Footer
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 5
            Text {
                text: "v0.1.0-alpha"
                color: theme.muted
                font.pixelSize: 11
            }
            Text {
                text: "IPv6 Mesh Ready"
                color: theme.primary
                font.pixelSize: 11
                opacity: 0.8
            }
        }
    }

    // Helper Component for Sidebar Buttons
    component NavButton: Rectangle {
        property string label: ""
        property string icon: ""
        property string target: ""
        property bool active: false

        Layout.fillWidth: true
        Layout.preferredHeight: 45
        radius: 8
        color: active || mouseArea.containsMouse ? Qt.rgba(77/255, 163/255, 255/255, 0.1) : "transparent"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 15
            spacing: 15
            Text {
                text: icon
                color: active ? theme.primary : theme.muted
                font.pixelSize: 16
                font.bold: true
            }
            Text {
                text: label
                color: active ? theme.text : theme.muted
                font.pixelSize: 14
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                // Reset other buttons (simplified for demo)
                root.pageChanged(target)
            }
        }

        Behavior on color { ColorAnimation { duration: 200 } }
    }
}
