import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Rectangle {
    id: trustRoot
    color: "#0a0e27"
    
    /**
     * LocalTrustDiaries Standalone Test Dashboard
     * 
     ┌─────────────────────────────────────────────────────┐
     │ HEADER: Backend Status & Test Controls              │
     ├─────────────────────────────────────────────────────┤
     │ LEFT PANEL: Input Controls                           │
     │ CENTER PANEL: Real-time Data Display                 │
     │ RIGHT PANEL: Event Log & Diagnostics                │
     └─────────────────────────────────────────────────────┘
     */

    // =====================================================================
    // THEME COLORS
    // =====================================================================
    readonly property color panelBg: "#1a1f3a"
    readonly property color borderColor: "#2a3f5f"
    readonly property color textPrimary: "#e0e0e0"
    readonly property color textMuted: "#8a94a6"
    readonly property color accentGreen: "#00d084"
    readonly property color accentRed: "#ff6b6b"
    readonly property color accentYellow: "#ffd93d"
    readonly property color accentBlue: "#4ecdc4"

    // =====================================================================
    // SIGNALS FROM ADAPTER (Connected in AppController)
    // =====================================================================
    signal trustScoreChanged(string peerId, real score);
    signal interactionRecorded(string peerId, string type, bool success);
    signal errorOccurred(string message);
    signal peerAdded(string peerId);
    signal connectionTestResult(string resultJson);

    // =====================================================================
    // EVENT LOG MANAGEMENT
    // =====================================================================
    ListModel {
        id: eventLogModel
    }

    function addEventLog(tag, message, level) {
        const timestamp = new Date().toLocaleTimeString();
        const levelColor = level === "error" ? accentRed : 
                          level === "warn" ? accentYellow :
                          level === "success" ? accentGreen : accentBlue;
        
        eventLogModel.insert(0, {
            "timestamp": timestamp,
            "tag": tag,
            "message": message,
            "level": level,
            "color": levelColor
        });
        
        // Keep only last 100 events
        while (eventLogModel.count > 100) {
            eventLogModel.remove(eventLogModel.count - 1);
        }
    }

    // =====================================================================
    // PEER DATA MODEL
    // =====================================================================
    ListModel {
        id: peerListModel
    }

    function updatePeerList(peersJson) {
        try {
            const peers = JSON.parse(peersJson);
            peerListModel.clear();
            
            for (let peer of peers) {
                peerListModel.append({
                    "peerId": peer.peerId,
                    "score": peer.score,
                    "scorePercent": (peer.score * 100).toFixed(1),
                    "trend": peer.trendString,
                    "successCount": peer.successCount,
                    "failureCount": peer.failureCount,
                    "total": peer.successCount + peer.failureCount
                });
            }
            
            addEventLog("DATA", `Loaded ${peers.length} peers`, "info");
        } catch (e) {
            addEventLog("ERROR", `Failed to parse peers: ${e}`, "error");
        }
    }

    // =====================================================================
    // TOP HEADER: Status & Test Controls
    // =====================================================================
    Rectangle {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 70
        color: panelBg
        border.color: borderColor
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 15

            // Status indicator
            Rectangle {
                width: 12
                height: 12
                radius: 6
                color: accentGreen
                ToolTip.text: "Backend Connected"
                MouseArea { anchors.fill: parent; hoverEnabled: true }
            }

            Text {
                text: "LocalTrustDiaries Test Dashboard"
                color: textPrimary
                font.pixelSize: 14
                font.bold: true
            }

            Item { Layout.fillWidth: true }

            // Status string
            Text {
                id: statusText
                text: "Ready"
                color: textMuted
                font.pixelSize: 11
            }

            Button {
                text: "Test Connection"
                onClicked: {
                    // Call adapter test function
                    addEventLog("TEST", "Running backend connection test...", "warn");
                    if (typeof appController !== 'undefined' && appController.trustAdapter) {
                        appController.trustAdapter.testBackendConnection();
                    }
                }
                background: Rectangle {
                    color: accentBlue
                    radius: 4
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 11
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            Button {
                text: "Clear All"
                onClicked: {
                    if (typeof appController !== 'undefined' && appController.trustAdapter) {
                        appController.trustAdapter.clearAllData();
                        peerListModel.clear();
                        eventLogModel.clear();
                        addEventLog("SYSTEM", "All data cleared", "warn");
                    }
                }
                background: Rectangle {
                    color: accentRed
                    radius: 4
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.pixelSize: 11
                }
            }
        }
    }

    // =====================================================================
    // MAIN CONTENT: 3-COLUMN LAYOUT
    // =====================================================================
    RowLayout {
        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        spacing: 10

        // ────────────────────────────────────────────────────────────────
        // LEFT PANEL: INPUT CONTROLS
        // ────────────────────────────────────────────────────────────────
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 300
            color: panelBg
            border.color: borderColor
            border.width: 1
            radius: 4

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 12

                Text {
                    text: "CONTROLS"
                    color: accentGreen
                    font.pixelSize: 12
                    font.bold: true
                }

                // Peer ID Input
                ColumnLayout {
                    spacing: 5
                    Text {
                        text: "Peer ID"
                        color: textMuted
                        font.pixelSize: 10
                    }
                    TextField {
                        id: peerIdInput
                        placeholderText: "Enter peer ID"
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: "#0a0e27"
                            border.color: borderColor
                            border.width: 1
                            radius: 3
                        }
                        color: textPrimary
                    }
                }

                // Trust Score Input
                ColumnLayout {
                    spacing: 5
                    Text {
                        text: "Trust Delta (-1.0 to 1.0)"
                        color: textMuted
                        font.pixelSize: 10
                    }
                    TextField {
                        id: trustDeltaInput
                        placeholderText: "0.1"
                        text: "0.1"
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: "#0a0e27"
                            border.color: borderColor
                            border.width: 1
                            radius: 3
                        }
                        color: textPrimary
                    }
                }

                // Interaction Type
                ColumnLayout {
                    spacing: 5
                    Text {
                        text: "Interaction Type"
                        color: textMuted
                        font.pixelSize: 10
                    }
                    ComboBox {
                        id: interactionTypeCombo
                        model: ["send", "receive", "validate", "forward", "store"]
                        Layout.fillWidth: true
                        background: Rectangle {
                            color: panelBg
                            border.color: borderColor
                            border.width: 1
                            radius: 3
                        }
                        contentItem: Text {
                            text: parent.currentText
                            color: textPrimary
                            leftPadding: 8
                        }
                    }
                }

                // Success/Failure Toggle
                Text {
                    text: "Interaction Result"
                    color: textMuted
                    font.pixelSize: 10
                }
                RowLayout {
                    Button {
                        id: successBtn
                        text: "SUCCESS"
                        Layout.fillWidth: true
                        checked: true
                        checkable: true
                        onClicked: { successBtn.checked = true; failureBtn.checked = false }
                        background: Rectangle {
                            color: successBtn.checked ? accentGreen : panelBg
                            border.color: borderColor
                            border.width: 1
                            radius: 3
                        }
                        contentItem: Text { text: parent.text; color: "white"; horizontalAlignment: Text.AlignHCenter }
                    }
                    Button {
                        id: failureBtn
                        text: "FAILURE"
                        Layout.fillWidth: true
                        checkable: true
                        onClicked: { failureBtn.checked = true; successBtn.checked = false }
                        background: Rectangle {
                            color: failureBtn.checked ? accentRed : panelBg
                            border.color: borderColor
                            border.width: 1
                            radius: 3
                        }
                        contentItem: Text { text: parent.text; color: "white"; horizontalAlignment: Text.AlignHCenter }
                    }
                }

                Separator { Layout.fillWidth: true }

                // ACTION BUTTONS
                Button {
                    text: "✓ Add Peer"
                    Layout.fillWidth: true
                    onClicked: {
                        const peerId = peerIdInput.text.trim();
                        if (!peerId) {
                            addEventLog("WARN", "Peer ID is empty", "warn");
                            return;
                        }
                        if (typeof appController !== 'undefined' && appController.trustAdapter) {
                            appController.trustAdapter.addPeer(peerId);
                        }
                    }
                    background: Rectangle { color: accentGreen; radius: 3 }
                    contentItem: Text { text: parent.text; color: "#0a0e27"; font.bold: true; horizontalAlignment: Text.AlignHCenter }
                }

                Button {
                    text: "↑ Update Trust"
                    Layout.fillWidth: true
                    onClicked: {
                        const peerId = peerIdInput.text.trim();
                        const delta = parseFloat(trustDeltaInput.text);
                        
                        if (!peerId || isNaN(delta)) {
                            addEventLog("WARN", "Invalid input", "warn");
                            return;
                        }
                        if (typeof appController !== 'undefined' && appController.trustAdapter) {
                            appController.trustAdapter.updateTrustScore(peerId, delta);
                        }
                    }
                    background: Rectangle { color: accentBlue; radius: 3 }
                    contentItem: Text { text: parent.text; color: "white"; font.bold: true; horizontalAlignment: Text.AlignHCenter }
                }

                Button {
                    text: "📝 Record Interaction"
                    Layout.fillWidth: true
                    onClicked: {
                        const peerId = peerIdInput.text.trim();
                        const type = interactionTypeCombo.currentText;
                        const success = successBtn.checked;
                        
                        if (!peerId) {
                            addEventLog("WARN", "Peer ID is empty", "warn");
                            return;
                        }
                        if (typeof appController !== 'undefined' && appController.trustAdapter) {
                            appController.trustAdapter.recordInteraction(peerId, type, success);
                        }
                    }
                    background: Rectangle { color: accentYellow; radius: 3 }
                    contentItem: Text { text: parent.text; color: "#0a0e27"; font.bold: true; horizontalAlignment: Text.AlignHCenter }
                }

                Button {
                    text: "✕ Remove Peer"
                    Layout.fillWidth: true
                    onClicked: {
                        const peerId = peerIdInput.text.trim();
                        if (!peerId) {
                            addEventLog("WARN", "Peer ID is empty", "warn");
                            return;
                        }
                        if (typeof appController !== 'undefined' && appController.trustAdapter) {
                            appController.trustAdapter.removePeer(peerId);
                        }
                    }
                    background: Rectangle { color: accentRed; radius: 3 }
                    contentItem: Text { text: parent.text; color: "white"; font.bold: true; horizontalAlignment: Text.AlignHCenter }
                }

                Separator { Layout.fillWidth: true }
                
                Text {
                    text: "SAMPLE DATA"
                    color: accentGreen
                    font.pixelSize: 12
                    font.bold: true
                }

                Button {
                    text: "Load Demo Data"
                    Layout.fillWidth: true
                    onClicked: {
                        const demoData = [
                            { id: "peer_alice", interactions: 5 },
                            { id: "peer_bob", interactions: 3 },
                            { id: "peer_charlie", interactions: 8 },
                        ];
                        
                        for (let peer of demoData) {
                            peerIdInput.text = peer.id;
                            appController.trustAdapter.addPeer(peer.id);
                            trustDeltaInput.text = "0.2";
                            
                            // Simulate some interactions
                            for (let i = 0; i < peer.interactions; ++i) {
                                const interactionTypes = ["send", "receive", "validate"];
                                const type = interactionTypes[i % interactionTypes.length];
                                const success = Math.random() > 0.2;
                                appController.trustAdapter.recordInteraction(peer.id, type, success);
                            }
                        }
                        
                        addEventLog("DEMO", "Loaded sample data", "success");
                    }
                    background: Rectangle { color: panelBg; border.color: borderColor; border.width: 1; radius: 3 }
                    contentItem: Text { text: parent.text; color: accentBlue; horizontalAlignment: Text.AlignHCenter }
                }

                Item { Layout.fillHeight: true }
            }
        }

        // ────────────────────────────────────────────────────────────────
        // CENTER PANEL: PEER LEADERBOARD
        // ────────────────────────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: panelBg
            border.color: borderColor
            border.width: 1
            radius: 4

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 10

                Text {
                    text: "PEER LEADERBOARD (Real-Time Data)"
                    color: accentGreen
                    font.pixelSize: 12
                    font.bold: true
                }

                // Header row
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 10

                    Text { text: "Peer ID"; color: textMuted; font.pixelSize: 10; Layout.preferredWidth: 120 }
                    Text { text: "Trust"; color: textMuted; font.pixelSize: 10; Layout.preferredWidth: 80 }
                    Text { text: "Success"; color: textMuted; font.pixelSize: 10; Layout.preferredWidth: 60 }
                    Text { text: "Fail"; color: textMuted; font.pixelSize: 10; Layout.preferredWidth: 40 }
                    Text { text: "Trend"; color: textMuted; font.pixelSize: 10; Layout.preferredWidth: 40 }
                    Item { Layout.fillWidth: true }
                }

                Separator { Layout.fillWidth: true }

                // Peer list with scrolling
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ListView {
                        id: peerListView
                        model: peerListModel
                        spacing: 5

                        delegate: Rectangle {
                            width: peerListView.width - 10
                            height: 40
                            color: index % 2 === 0 ? "#0a0e27" : "#141829"
                            border.color: borderColor
                            border.width: 1
                            radius: 3

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 8
                                spacing: 10

                                Text { 
                                    text: model.peerId; 
                                    color: textPrimary; 
                                    font.pixelSize: 11;
                                    Layout.preferredWidth: 120
                                    elide: Text.ElideRight
                                }

                                Rectangle {
                                    Layout.preferredWidth: 80
                                    height: 20
                                    color: "#0a0e27"
                                    border.color: accentBlue
                                    border.width: 1
                                    radius: 3

                                    Row {
                                        anchors.fill: parent
                                        anchors.margins: 2

                                        Rectangle {
                                            width: (parent.width - 4) * (model.score)
                                            height: parent.height
                                            color: model.score > 0.7 ? accentGreen : 
                                                   model.score > 0.4 ? accentYellow : accentRed
                                            radius: 2
                                        }
                                    }

                                    Text { 
                                        anchors.centerIn: parent
                                        text: model.scorePercent + "%";
                                        color: textPrimary;
                                        font.pixelSize: 9
                                    }
                                }

                                Text { 
                                    text: model.successCount; 
                                    color: accentGreen; 
                                    font.pixelSize: 11;
                                    Layout.preferredWidth: 60
                                }

                                Text { 
                                    text: model.failureCount; 
                                    color: accentRed; 
                                    font.pixelSize: 11;
                                    Layout.preferredWidth: 40
                                }

                                Text { 
                                    text: model.trend; 
                                    color: model.trend === "↑" ? accentGreen : 
                                           model.trend === "↓" ? accentRed : textMuted;
                                    font.pixelSize: 14;
                                    bold: true;
                                    Layout.preferredWidth: 40
                                }

                                Button {
                                    text: "Details"
                                    Layout.fillWidth: true
                                    background: Rectangle { color: panelBg; border.width: 1; border.color: borderColor; radius: 2 }
                                    contentItem: Text { text: parent.text; color: accentBlue; font.pixelSize: 9 }
                                    onClicked: {
                                        addEventLog("DETAILS", `Querying history for ${model.peerId}...`, "info");
                                    }
                                }

                                Item { Layout.fillWidth: true }
                            }
                        }
                    }
                }

                Text {
                    text: `Total Peers: ${peerListModel.count}`
                    color: textMuted
                    font.pixelSize: 10
                }
            }
        }

        // ────────────────────────────────────────────────────────────────
        // RIGHT PANEL: EVENT LOG & STATUS
        // ────────────────────────────────────────────────────────────────
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 280
            color: panelBg
            border.color: borderColor
            border.width: 1
            radius: 4

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                spacing: 10

                Text {
                    text: "EVENT LOG"
                    color: accentGreen
                    font.pixelSize: 12
                    font.bold: true
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ListView {
                        model: eventLogModel
                        spacing: 3

                        delegate: Rectangle {
                            width: parent.width - 10
                            height: contentHeight + 8
                            color: "#0a0e27"
                            border.color: model.color
                            border.width: 0.5
                            radius: 2

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 5
                                spacing: 2

                                RowLayout {
                                    spacing: 5

                                    Text { 
                                        text: model.timestamp; 
                                        color: textMuted;
                                        font.pixelSize: 8
                                    }

                                    Text { 
                                        text: "[" + model.tag + "]"; 
                                        color: model.color;
                                        font.pixelSize: 9;
                                        font.bold: true
                                    }

                                    Item { Layout.fillWidth: true }
                                }

                                Text { 
                                    text: model.message; 
                                    color: textPrimary;
                                    font.pixelSize: 9;
                                    wrapMode: Text.WordWrap;
                                    Layout.fillWidth: true
                                }
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: borderColor
                }

                Text {
                    text: "BACKEND INFO"
                    color: accentGreen
                    font.pixelSize: 11
                    font.bold: true
                }

                Text {
                    id: backendInfoText
                    text: "Connecting..."
                    color: textMuted
                    font.pixelSize: 9
                    font.family: "Courier"
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
            }
        }
    }

    // =====================================================================
    // CONNECTIONS
    // =====================================================================
    Component.onCompleted: {
        addEventLog("SYSTEM", "Trust Dashboard loaded", "success");
        
        if (typeof appController !== 'undefined') {
            // Listen for adapter signals
            if (appController.trustAdapter) {
                appController.trustAdapter.trustScoreChanged.connect(function(peerId, score) {
                    addEventLog("UPDATE", `${peerId} trust → ${score.toFixed(2)}`, "success");
                });
                
                appController.trustAdapter.interactionRecorded.connect(function(peerId, type, success) {
                    const result = success ? "✓" : "✗";
                    addEventLog("INTERACTION", `${result} ${peerId} / ${type}`, success ? "success" : "warn");
                });
                
                appController.trustAdapter.errorOccurred.connect(function(message) {
                    addEventLog("ERROR", message, "error");
                });
                
                appController.trustAdapter.peerAdded.connect(function(peerId) {
                    addEventLog("PEER", `Added: ${peerId}`, "success");
                });
                
                appController.trustAdapter.stateUpdated.connect(function(dataJson) {
                    updatePeerList(dataJson);
                });
                
                appController.trustAdapter.connectionTestResult.connect(function(resultJson) {
                    try {
                        const result = JSON.parse(resultJson);
                        if (result.status === "success") {
                            addEventLog("TEST", "✓ Backend test PASSED", "success");
                        } else {
                            addEventLog("TEST", "✗ " + result.error, "error");
                        }
                    } catch (e) {
                        addEventLog("ERROR", "Failed to parse test result", "error");
                    }
                });
                
                // Update backend info periodically
                const infoUpdateTimer = setInterval(function() {
                    backendInfoText.text = appController.trustAdapter.getStatusString();
                }, 1000);
                
                addEventLog("SYSTEM", "Adapter connected", "success");
            }
        } else {
            addEventLog("ERROR", "AppController not available", "error");
        }
    }
}
