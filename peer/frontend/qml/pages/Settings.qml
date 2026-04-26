import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import "../theme"

Item {
    Colors { id: theme }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 30
        spacing: 30

        Text {
            text: "System Settings"
            color: theme.text
            font.pixelSize: 24
            font.weight: Font.Bold
        }

        ColumnLayout {
            spacing: 20
            
            SettingToggle { label: "Auto-refresh Networks"; active: true }
            SettingToggle { label: "IPv6 Mesh Discovery"; active: true }
            SettingToggle { label: "Enable Global Node Proxy"; active: false }
            
            ColumnLayout {
                spacing: 10
                Text { text: "API BASE URL"; color: theme.muted; font.pixelSize: 11; font.weight: Font.Bold }
                TextField {
                    text: "https://dataexsys-p2p-production.up.railway.app/api/networks"
                    Layout.fillWidth: true
                    enabled: false
                }
            }
        }
        
        Item { Layout.fillHeight: true }
    }

    component SettingToggle: RowLayout {
        property string label: ""
        property bool active: false
        Layout.fillWidth: true
        
        Text { text: label; color: theme.text; font.pixelSize: 14; Layout.fillWidth: true }
        Switch { checked: active }
    }
}
