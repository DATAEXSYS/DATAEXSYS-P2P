import QtQuick 2.15
import QtQuick.Layouts 1.15
import "../theme"
import "../components"

Item {
    Colors { id: theme }

    Flickable {
        anchors.fill: parent
        contentHeight: contentColumn.height + 40
        clip: true

        ColumnLayout {
            id: contentColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 30
            spacing: 30

            // Welcome Section
            ColumnLayout {
                spacing: 5
                Text {
                    text: "Welcome back, Operator"
                    color: theme.text
                    font.pixelSize: 24
                    font.weight: Font.Bold
                }
                Text {
                    text: "System status is stable. 4 active mesh nodes detected."
                    color: theme.muted
                    font.pixelSize: 14
                }
            }

            // Stats Cards
            RowLayout {
                Layout.fillWidth: true
                spacing: 20

                StatCard {
                    title: "TOTAL PEERS"
                    value: appController.totalPeers.toString()
                    icon: "👤"
                    accentColor: theme.primary
                }
                StatCard {
                    title: "ACTIVE NETWORKS"
                    value: appController.networkModel.rowCount().toString()
                    icon: "🌐"
                    accentColor: theme.accent
                }
                StatCard {
                    title: "SYSTEM LOAD"
                    value: "1.2%"
                    icon: "📊"
                    accentColor: theme.success
                }
            }

            // Quick Actions
            Text {
                text: "QUICK ACTIONS"
                color: theme.muted
                font.pixelSize: 12
                font.weight: Font.Bold
                font.letterSpacing: 1
            }

            RowLayout {
                spacing: 15
                ActionButton { label: "Scan for Peers"; icon: "🔍" }
                ActionButton { label: "Establish Bridge"; icon: "🔗" }
                ActionButton { label: "Fetch Topology"; icon: "🗺️" }
            }
        }
    }

    // --- HELPER COMPONENTS ---

    component StatCard: Rectangle {
        property string title: ""
        property string value: ""
        property string icon: ""
        property color accentColor: theme.primary

        Layout.fillWidth: true
        Layout.preferredHeight: 120
        color: theme.panel
        radius: 12
        border.color: theme.border
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 10
            RowLayout {
                Text { text: icon; font.pixelSize: 20 }
                Item { Layout.fillWidth: true }
                Text {
                    text: title
                    color: theme.muted
                    font.pixelSize: 11
                    font.weight: Font.Bold
                }
            }
            Text {
                text: value
                color: theme.text
                font.pixelSize: 32
                font.weight: Font.Bold
            }
        }

        // Accent Line
        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 3
            color: accentColor
            radius: 2
            opacity: 0.6
        }
    }

    component ActionButton: Rectangle {
        property string label: ""
        property string icon: ""

        Layout.preferredWidth: 180
        Layout.preferredHeight: 50
        color: mouseArea.containsMouse ? Qt.rgba(1,1,1,0.05) : "transparent"
        border.color: theme.border
        border.width: 1
        radius: 8

        RowLayout {
            anchors.centerIn: parent
            spacing: 10
            Text { text: icon; font.pixelSize: 16 }
            Text { text: label; color: theme.text; font.pixelSize: 14 }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
        }
        
        Behavior on color { ColorAnimation { duration: 150 } }
    }
}
