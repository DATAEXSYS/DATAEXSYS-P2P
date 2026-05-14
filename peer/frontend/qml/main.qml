import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "./theme"
import "./components"

Window {
    id: window
    width: 1400
    height: 950
    visible: true
    title: "DATAEXSYS | Distributed Systems Educational Dashboard"
    color: theme.background

    Colors { id: theme }

    property string currentTab: "P2P CHAT"

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // --- TOP NAVIGATION ---
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 70
            color: theme.panel
            border.color: theme.border
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 30
                anchors.rightMargin: 30
                spacing: 40

                Text {
                    text: "DATAEXSYS"
                    color: theme.primary
                    font.pixelSize: 20
                    font.weight: Font.Black
                    font.letterSpacing: 2
                }

                RowLayout {
                    spacing: 20
                    Repeater {
                        model: ["NODE REGISTRATION", "P2P CHAT", "TRUST DASHBOARD", "MINING DASHBOARD"]
                        Button {
                            text: modelData
                            flat: true
                            onClicked: currentTab = modelData
                            contentItem: Text {
                                text: parent.text
                                color: currentTab === modelData ? theme.success : theme.muted
                                font.weight: Font.Bold
                                font.pixelSize: 12
                                font.letterSpacing: 1
                            }
                            Rectangle {
                                anchors.bottom: parent.bottom
                                width: parent.width
                                height: 2
                                color: theme.success
                                visible: currentTab === modelData
                            }
                        }
                    }
                }

                Item { Layout.fillWidth: true }
            }
        }

        // --- MAIN CONTENT AREA ---
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 1

            // Central View: Tab Content
            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: {
                    if (currentTab === "NODE REGISTRATION") return 0
                    if (currentTab === "P2P CHAT") return 1
                    if (currentTab === "TRUST DASHBOARD") return 2
                    if (currentTab === "MINING DASHBOARD") return 3
                    return 0
                }
                
                Loader { source: "pages/NodeRegistration.qml" }
                ChatDashboard { }
                Loader { source: "pages/TrustDashboard.qml" }
                Loader { source: "pages/MiningDashboard.qml" }
            }
        }
    }
}
