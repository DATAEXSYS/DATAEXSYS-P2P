import QtQuick 2.15
import QtQuick.Layouts 1.15
import "../theme"

Item {
    Colors { id: theme }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 30
        spacing: 20

        Text {
            text: "Network Topology"
            color: theme.text
            font.pixelSize: 24
            font.weight: Font.Bold
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: theme.panel
            radius: 12
            border.color: theme.border
            
            Text {
                anchors.centerIn: parent
                text: "Peer discovery in progress...\nScanning IPv6 Local Mesh"
                color: theme.muted
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 16
            }
        }
    }
}
