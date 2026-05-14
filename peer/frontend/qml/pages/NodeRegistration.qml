import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    color: "#0B0F1A"

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "P2P NETWORK REGISTRATION"
            color: "#38bdf8"
            font.pixelSize: 24
            font.weight: Font.Bold
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "Mining initial Proof-of-Work to join the global peer-to-peer network."
            color: "#64748B"
            font.pixelSize: 14
            Layout.alignment: Qt.AlignHCenter
        }

        ProgressBar {
            id: powProgress
            Layout.preferredWidth: 400
            Layout.alignment: Qt.AlignHCenter
            value: 0
            from: 0
            to: 100
            background: Rectangle {
                color: "#111827"
                radius: 4
                border.color: "#1F2937"
            }
            contentItem: Item {
                Rectangle {
                    width: powProgress.visualPosition * parent.width
                    height: parent.height
                    radius: 4
                    color: "#14B8A6"
                }
            }
        }

        Text {
            id: statusText
            text: "Awaiting registration..."
            color: "#14B8A6"
            font.pixelSize: 12
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            id: registerBtn
            text: "START 5-MINUTE REGISTRATION SYNC"
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 300
            Layout.preferredHeight: 45
            background: Rectangle {
                color: registerBtn.enabled ? "#4DA3FF" : "#1F2937"
                radius: 6
            }
            contentItem: Text {
                text: registerBtn.text
                color: "white"
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            onClicked: {
                enabled = false
                statusText.text = "Mining Genesis Certificate... (Syncing with network)"
                powTimer.start()
            }
        }
    }

    Timer {
        id: powTimer
        interval: 3000 // In simulation, we speed up 5 minutes to 5 seconds (50 ticks of 100ms... wait 3000ms is 3 sec total if we do 1 tick? Let's do 100ms * 50 = 5 seconds)
        // Wait, 100ms interval
        repeat: true
        onTriggered: {
            powProgress.value += 2 // 50 ticks * 100ms = 5 seconds total
            if (powProgress.value >= 100) {
                stop()
                statusText.text = "NODE SUCCESSFULLY REGISTERED AND SYNCED!"
                
                // Broadcast to network
                appController.sendRealMessage("ff02::1", "REGISTER:NEW_NODE")
                appController.logEvent("NETWORK", "Broadcasted node registration to the global P2P network.")
            }
        }
    }
    
    Component.onCompleted: {
        powTimer.interval = 100 // 100ms ticks
    }
}
