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

    property string currentTab: "TOPOLOGY"

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
                        model: ["TOPOLOGY", "BLOCKCHAIN", "P2P CHAT"]
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

                RowLayout {
                    spacing: 15
                    Button {
                        text: "PACKET ROUTING"
                        onClicked: currentTab = "ROLLING"
                        background: Rectangle {
                            color: currentTab === "ROLLING" ? theme.success : "transparent"
                            border.color: theme.border
                            radius: 4
                        }
                        contentItem: Text {
                            text: parent.text
                            color: currentTab === "ROLLING" ? "white" : theme.text
                            font.pixelSize: 11
                            font.weight: Font.Bold
                        }
                    }
                    Button {
                        text: "TRUST DASHBOARD"
                        onClicked: currentTab = "TRUST"
                        background: Rectangle {
                            color: currentTab === "TRUST" ? theme.success : "transparent"
                            border.color: theme.border
                            radius: 4
                        }
                        contentItem: Text {
                            text: parent.text
                            color: currentTab === "TRUST" ? "white" : theme.text
                            font.pixelSize: 11
                            font.weight: Font.Bold
                        }
                    }
                    Button {
                        text: "RESET"
                        onClicked: appController.resetEngine()
                        background: Rectangle { color: "transparent"; border.color: theme.border; radius: 4 }
                        contentItem: Text { text: parent.text; color: theme.text; font.pixelSize: 11 }
                    }
                    Button {
                        text: "NEXT STEP"
                        onClicked: appController.nextStep()
                        background: Rectangle { color: theme.primary; radius: 4 }
                        contentItem: Text { text: parent.text; color: "white"; font.weight: Font.Bold; font.pixelSize: 11 }
                    }
                    Switch {
                        checked: appController.autoRun
                        onToggled: appController.autoRun = checked
                    }
                    Text { text: "AUTO-RUN"; color: theme.muted; font.pixelSize: 10 }
                }
            }
        }

        // --- MAIN CONTENT AREA ---
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 1

            // Left Sidebar: Trust & Stats
            TrustDiary {
                Layout.fillHeight: true
                Layout.preferredWidth: 320
            }

            // Central View: Tab Content
            StackLayout {
                currentIndex: {
                    if (currentTab === "TOPOLOGY") return 0
                    if (currentTab === "BLOCKCHAIN") return 1
                    if (currentTab === "ROLLING") return 4
                    if (currentTab === "TRUST") return 3
                    return 2
                }
                
                // TOPOLOGY
                Rectangle {
                    color: theme.panel
                    NetworkGraph { anchors.fill: parent }
                    CertificateFlow { anchors.fill: parent }
                    PacketVisualizer { anchors.fill: parent }
                }

                // BLOCKCHAIN
                BlockchainView {}

                // CHAT
                ChatDashboard {}

                // TRUST DASHBOARD
                Loader {
                    source: "pages/TrustDashboard.qml"
                }

                // ROLLING SIGNATURES DASHBOARD
                Loader {
                    source: "pages/PacketRoutingDashboard.qml"
                }
            }

            // Right Sidebar: Console
            SecurityConsole {
                Layout.fillHeight: true
                Layout.preferredWidth: 400
            }
        }
        
        // Bottom Status Bar
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            color: theme.panel
            border.color: theme.border
            
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                Text { 
                    text: "SYSTEM STATUS: ACTIVE | STEP: " + appController.executionStep + " / 9"
                    color: theme.muted
                    font.pixelSize: 10
                }
            }
        }
    }
}
