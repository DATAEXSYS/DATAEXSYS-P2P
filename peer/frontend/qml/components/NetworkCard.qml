import QtQuick 2.15
import QtQuick.Layouts 1.15
import "../theme"

Rectangle {
    property string name: ""
    property string type: ""
    property int peers: 0
    property string description: ""

    color: theme.panel
    radius: 12
    border.color: theme.border
    border.width: 1

    Colors { id: theme }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 8

        RowLayout {
            Text {
                text: name
                color: theme.text
                font.pixelSize: 18
                font.weight: Font.Bold
            }
            Item { Layout.fillWidth: true }
            Rectangle {
                width: 60; height: 22
                radius: 11
                color: Qt.rgba(theme.primary.r, theme.primary.g, theme.primary.b, 0.1)
                Text {
                    anchors.centerIn: parent
                    text: type
                    color: theme.primary
                    font.pixelSize: 10
                    font.bold: true
                }
            }
        }

        Text {
            text: description
            color: theme.muted
            font.pixelSize: 12
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            maximumLineCount: 2
            elide: Text.ElideRight
        }

        Item { Layout.fillHeight: true }

        RowLayout {
            Text {
                text: "👥 " + peers + " active peers"
                color: theme.success
                font.pixelSize: 12
            }
            Item { Layout.fillWidth: true }
            Text {
                text: "JOIN →"
                color: theme.primary
                font.pixelSize: 12
                font.weight: Font.Bold
                
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: { parent.border.color = theme.primary; parent.scale = 1.02 }
        onExited: { parent.border.color = theme.border; parent.scale = 1.0 }
    }

    Behavior on border.color { ColorAnimation { duration: 200 } }
    Behavior on scale { NumberAnimation { duration: 200; easing.type: Easing.OutBack } }
}
