import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./theme"
import "./components"

Window {
    width: 1100
    height: 750
    visible: true
    title: "DATAEXSYS | Decentralized Transfer & Access Exchange"
    color: theme.background

    Colors { id: theme }

    // --- BACKGROUND EFFECTS ---
    Rectangle {
        anchors.fill: parent
        color: theme.background

        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#0D4DA3FF" }
                GradientStop { position: 1.0; color: "transparent" }
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // --- SIDEBAR ---
        Sidebar {
            Layout.fillHeight: true
            Layout.preferredWidth: 240
            onPageChanged: (page) => navStack.replace(page)
        }

        // --- CONTENT AREA ---
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // Top Bar
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 60
                color: "transparent"

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20

                    Text {
                        text: "DASHBOARD"
                        color: theme.text
                        font.pixelSize: 18
                        font.weight: Font.Bold
                        font.letterSpacing: 1.5
                    }

                    Item { Layout.fillWidth: true }

                    StatusBadge {
                        statusText: appController.isOnline ? "ONLINE" : "OFFLINE"
                        statusColor: appController.isOnline ? theme.success : theme.danger
                    }
                }

                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: theme.border
                    opacity: 0.3
                }
            }

            StackView {
                id: navStack
                Layout.fillWidth: true
                Layout.fillHeight: true
                initialItem: "pages/Home.qml"
                
                pushEnter: Transition {
                    PropertyAnimation { property: "opacity"; from: 0; to: 1; duration: 250 }
                }
                pushExit: Transition {
                    PropertyAnimation { property: "opacity"; from: 1; to: 0; duration: 250 }
                }
            }
        }
    }
}
