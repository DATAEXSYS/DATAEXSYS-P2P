import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#0b1020"

    readonly property color panelBg: "#111827"
    readonly property color panelAlt: "#0f172a"
    readonly property color borderColor: "#25324a"
    readonly property color textColor: "#e5e7eb"
    readonly property color mutedText: "#94a3b8"
    readonly property color accent: "#38bdf8"
    readonly property color good: "#22c55e"
    readonly property color warn: "#f59e0b"
    readonly property color bad: "#ef4444"

    property var adapter: (typeof appController !== "undefined" && appController.trustAdapter) ? appController.trustAdapter : null
    property string systemStateText: adapter ? adapter.systemState : "Standalone simulation lab"
    property bool backendAvailable: adapter !== null
    property var localPeers: []
    property var localTimeline: []
    property string localSnapshotJson: ""
    property alias peerIdInput: peerAInput

    ListModel { id: peerModel }
    ListModel { id: timelineModel }
    ListModel { id: eventModel }

    function logEvent(tag, message, level) {
        const color = level === "error" ? bad : (level === "warn" ? warn : accent)
        eventModel.insert(0, {
            "time": new Date().toLocaleTimeString(),
            "tag": tag,
            "message": message,
            "color": color
        })
        while (eventModel.count > 140)
            eventModel.remove(eventModel.count - 1)
    }

    function clampTrust(value) {
        return Math.max(0.0, Math.min(1.0, value))
    }

    function trendFor(value) {
        if (value >= 0.7)
            return "Rising"
        if (value <= 0.3)
            return "Declining"
        return "Stable"
    }

    function normalizeType(type) {
        const key = type.trim().toLowerCase()
        return key === "" ? "generic" : key
    }

    function baseDeltaFor(type) {
        switch (normalizeType(type)) {
        case "send": return 0.03
        case "receive": return 0.02
        case "validate": return 0.05
        case "forward": return 0.015
        case "store": return 0.01
        default: return 0.025
        }
    }

    function seededHash(text) {
        let hash = 2166136261
        for (let i = 0; i < text.length; ++i) {
            hash ^= text.charCodeAt(i)
            hash = Math.imul(hash, 16777619)
        }
        return hash >>> 0
    }

    function deterministicOutcome(peerA, peerB, type, sequence) {
        const seed = seededHash(peerA + "|" + peerB + "|" + type + "|" + sequence)
        const score = seed % 100
        const a = getLocalPeer(peerA)
        const b = getLocalPeer(peerB)
        const trustScore = Math.round(((a.trust + b.trust) / 2.0) * 100)
        return score <= trustScore
    }

    function getLocalPeer(peerId) {
        for (let i = 0; i < localPeers.length; ++i) {
            if (localPeers[i].peerId === peerId)
                return localPeers[i]
        }
        return null
    }

    function updateLocalPeer(peerId, updater) {
        for (let i = 0; i < localPeers.length; ++i) {
            if (localPeers[i].peerId === peerId) {
                updater(localPeers[i])
                localPeers[i].trend = trendFor(localPeers[i].trust)
                return localPeers[i]
            }
        }
        return null
    }

    function rebuildPeerModel(peers) {
        peerModel.clear()
        for (let i = 0; i < peers.length; ++i) {
            const peer = peers[i]
            peerModel.append({
                "peerId": peer.peerId,
                "trust": peer.trust,
                "interactions": peer.interactions,
                "positive": peer.positive,
                "negative": peer.negative,
                "trend": peer.trend,
                "trustPercent": Math.round(peer.trust * 100)
            })
        }
    }

    function rebuildTimelineModel(timeline) {
        timelineModel.clear()
        for (let i = 0; i < timeline.length; ++i) {
            const item = timeline[i]
            timelineModel.append({
                "index": item.index,
                "peerA": item.peerA,
                "peerB": item.peerB,
                "type": item.type,
                "success": item.success,
                "deltaA": item.deltaA,
                "deltaB": item.deltaB,
                "resultingState": item.resultingState,
                "timestamp": item.timestamp
            })
        }
    }

    function syncViewFromLocal() {
        rebuildPeerModel(localPeers)
        rebuildTimelineModel(localTimeline)
        systemStateText = "Standalone simulation lab | " + localPeers.length + " peers | " + localTimeline.length + " interactions"
    }

    function syncViewFromStateJson(stateJson) {
        try {
            const state = JSON.parse(stateJson)
            const peers = state.peers || []
            const timeline = state.timeline || []
            localPeers = peers.map(function(peer) {
                return {
                    peerId: peer.peerId,
                    trust: peer.trust,
                    interactions: peer.interactions,
                    positive: peer.positive,
                    negative: peer.negative,
                    trend: peer.trend
                }
            })
            localTimeline = timeline.map(function(item) {
                return {
                    index: item.index,
                    peerA: item.peerA,
                    peerB: item.peerB,
                    type: item.type,
                    success: item.success,
                    deltaA: item.deltaA,
                    deltaB: item.deltaB,
                    resultingState: item.resultingState,
                    timestamp: item.timestamp
                }
            })
            rebuildPeerModel(localPeers)
            rebuildTimelineModel(localTimeline)
            systemStateText = state.systemState || (backendAvailable ? "Adapter connected" : "Standalone simulation lab")
        } catch (e) {
            logEvent("ERROR", "Failed to parse state snapshot", "error")
        }
    }

    function localSnapshot() {
        return JSON.stringify({
            systemState: systemStateText,
            peers: localPeers,
            timeline: localTimeline,
            timestamp: new Date().toISOString()
        })
    }

    function restoreLocalSnapshot(snapshotJson) {
        try {
            const state = JSON.parse(snapshotJson)
            localPeers = state.peers || []
            localTimeline = state.timeline || []
            rebuildPeerModel(localPeers)
            rebuildTimelineModel(localTimeline)
            systemStateText = state.systemState || systemStateText
        } catch (e) {
            logEvent("ERROR", "Unable to restore snapshot", "error")
        }
    }

    function createPeerAction() {
        const peerId = peerIdInput.text.trim()
        if (peerId === "") {
            logEvent("WARN", "Peer ID is required", "warn")
            return
        }

        if (backendAvailable) {
            adapter.createPeer(peerId)
            return
        }

        if (getLocalPeer(peerId)) {
            logEvent("WARN", "Peer already exists: " + peerId, "warn")
            return
        }

        localPeers.push({ peerId: peerId, trust: 0.5, interactions: 0, positive: 0, negative: 0, trend: "Neutral" })
        localSnapshotJson = localSnapshot()
        systemStateText = "Peer created: " + peerId
        logEvent("PEER", "Created peer " + peerId, "info")
        syncViewFromLocal()
    }

    function deletePeerAction() {
        const peerId = peerIdInput.text.trim()
        if (peerId === "") {
            logEvent("WARN", "Peer ID is required", "warn")
            return
        }

        if (backendAvailable) {
            adapter.deletePeer(peerId)
            return
        }

        const before = localPeers.length
        localPeers = localPeers.filter(function(peer) { return peer.peerId !== peerId })
        if (localPeers.length === before) {
            logEvent("ERROR", "Peer not found: " + peerId, "error")
            return
        }

        localSnapshotJson = localSnapshot()
        systemStateText = "Peer removed: " + peerId
        logEvent("PEER", "Removed peer " + peerId, "warn")
        syncViewFromLocal()
    }

    function setTrustAction() {
        const peerId = peerIdInput.text.trim()
        const trustValue = parseFloat(trustInput.text)
        if (peerId === "" || isNaN(trustValue)) {
            logEvent("WARN", "Peer ID and valid trust value are required", "warn")
            return
        }

        const value = clampTrust(trustValue)
        if (backendAvailable) {
            adapter.setTrust(peerId, value)
            return
        }

        const peer = updateLocalPeer(peerId, function(record) {
            record.trust = value
        })
        if (!peer) {
            logEvent("ERROR", "Peer not found: " + peerId, "error")
            return
        }

        localSnapshotJson = localSnapshot()
        systemStateText = "Trust updated: " + peerId
        logEvent("TRUST", "Set trust for " + peerId + " to " + value.toFixed(2), "info")
        syncViewFromLocal()
    }

    function simulateInteractionAction(countOverride) {
        const peerA = peerAInput.text.trim()
        const peerB = peerBInput.text.trim()
        const type = interactionTypeCombo.currentText
        const count = countOverride ? countOverride : 1

        if (peerA === "" || peerB === "") {
            logEvent("WARN", "Both peer IDs are required", "warn")
            return
        }

        if (backendAvailable && count === 1) {
            adapter.simulateInteraction(peerA, peerB, type)
            return
        }

        for (let i = 0; i < count; ++i) {
            const left = getLocalPeer(peerA)
            const right = getLocalPeer(peerB)
            if (!left || !right) {
                logEvent("ERROR", "Both peers must exist before simulation", "error")
                return
            }

            const sequence = localTimeline.length
            const normalizedType = normalizeType(type)
            const success = deterministicOutcome(peerA, peerB, normalizedType, sequence)
            const baseDelta = baseDeltaFor(normalizedType)
            const deltaA = success ? baseDelta : -baseDelta * 1.5
            const deltaB = success ? baseDelta * 0.6 : -baseDelta * 0.9

            left.interactions += 1
            right.interactions += 1
            if (success) {
                left.positive += 1
                right.positive += 1
            } else {
                left.negative += 1
                right.negative += 1
            }

            left.trust = clampTrust(left.trust + deltaA)
            right.trust = clampTrust(right.trust + deltaB)
            left.trend = trendFor(left.trust)
            right.trend = trendFor(right.trust)

            localTimeline.push({
                index: sequence,
                peerA: peerA,
                peerB: peerB,
                type: normalizedType,
                success: success,
                deltaA: deltaA,
                deltaB: deltaB,
                resultingState: peerA + "=" + left.trust.toFixed(2) + " | " + peerB + "=" + right.trust.toFixed(2),
                timestamp: new Date().toISOString()
            })

            logEvent("INTERACTION", peerA + " / " + peerB + " / " + normalizedType + " => " + (success ? "SUCCESS" : "FAILURE"), success ? "info" : "warn")
        }

        localSnapshotJson = localSnapshot()
        systemStateText = "Interaction logged: " + peerA + " -> " + peerB
        syncViewFromLocal()
    }

    function resetSystemAction() {
        if (backendAvailable) {
            adapter.resetSystem()
            return
        }

        localSnapshotJson = localSnapshot()
        localPeers = []
        localTimeline = []
        systemStateText = "System reset"
        logEvent("SYSTEM", "System reset", "warn")
        syncViewFromLocal()
    }

    function replayLastStateAction() {
        if (backendAvailable) {
            adapter.replayLastState()
            return
        }

        if (localSnapshotJson === "") {
            logEvent("WARN", "No snapshot available to replay", "warn")
            return
        }

        restoreLocalSnapshot(localSnapshotJson)
        logEvent("SYSTEM", "Replayed last local snapshot", "info")
    }

    function simulateHundredInteractionsAction() {
        logEvent("TEST", "Running 100 interaction stress test", "warn")
        if (backendAvailable) {
            for (let i = 0; i < 100; ++i) {
                adapter.simulateInteraction(peerAInput.text.trim(), peerBInput.text.trim(), interactionTypeCombo.currentText)
            }
            return
        }
        simulateInteractionAction(100)
    }

    function stressTrustUpdatesAction() {
        const peerId = peerIdInput.text.trim()
        if (peerId === "") {
            logEvent("WARN", "Peer ID is required", "warn")
            return
        }

        logEvent("TEST", "Running stress trust update sequence", "warn")
        if (backendAvailable) {
            for (let i = 0; i < 25; ++i) {
                const nextValue = clampTrust((i % 2 === 0 ? 0.2 : 0.8) + (i * 0.01))
                adapter.setTrust(peerId, nextValue)
            }
            return
        }

        if (!getLocalPeer(peerId)) {
            logEvent("ERROR", "Peer not found: " + peerId, "error")
            return
        }

        for (let i = 0; i < 25; ++i) {
            updateLocalPeer(peerId, function(record) {
                record.trust = clampTrust((i % 2 === 0 ? 0.2 : 0.8) + (i * 0.01))
            })
        }

        localSnapshotJson = localSnapshot()
        systemStateText = "Stress trust updates executed"
        syncViewFromLocal()
    }

    function refreshFromAdapter(peersJson, timelineJson, stateJson) {
        try {
            const peers = JSON.parse(peersJson)
            const timeline = JSON.parse(timelineJson)
            const state = JSON.parse(stateJson)
            localPeers = peers.map(function(peer) {
                return {
                    peerId: peer.peerId,
                    trust: peer.trust,
                    interactions: peer.interactions,
                    positive: peer.positive,
                    negative: peer.negative,
                    trend: peer.trend
                }
            })
            localTimeline = timeline.map(function(item) {
                return {
                    index: item.index,
                    peerA: item.peerA,
                    peerB: item.peerB,
                    type: item.type,
                    success: item.success,
                    deltaA: item.deltaA,
                    deltaB: item.deltaB,
                    resultingState: item.resultingState,
                    timestamp: item.timestamp
                }
            })
            rebuildPeerModel(localPeers)
            rebuildTimelineModel(localTimeline)
            systemStateText = state.systemState || "Adapter connected"
        } catch (e) {
            logEvent("ERROR", "Failed to refresh adapter state", "error")
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 58
            color: panelBg
            border.color: borderColor
            radius: 6

            RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 10

                Text {
                    text: "LocalTrustDiaries Simulation Lab"
                    color: textColor
                    font.pixelSize: 15
                    font.bold: true
                }

                Item { Layout.fillWidth: true }

                Rectangle {
                    width: 12
                    height: 12
                    radius: 6
                    color: backendAvailable ? good : warn
                }

                Text {
                    text: backendAvailable ? "Adapter Connected" : "Standalone Mode"
                    color: backendAvailable ? good : warn
                    font.pixelSize: 11
                    font.bold: true
                }

                Text {
                    text: systemStateText
                    color: mutedText
                    font.pixelSize: 10
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10

            Rectangle {
                Layout.preferredWidth: 360
                Layout.fillHeight: true
                color: panelBg
                border.color: borderColor
                radius: 6

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8

                    Text { text: "CONTROL PANEL"; color: accent; font.pixelSize: 12; font.bold: true }

                    Text { text: "Peer A / Peer ID"; color: mutedText; font.pixelSize: 10 }
                    TextField { id: peerAInput; placeholderText: "peer_alpha"; Layout.fillWidth: true }

                    Text { text: "Partner Peer ID"; color: mutedText; font.pixelSize: 10 }
                    TextField { id: peerBInput; placeholderText: "peer_beta"; Layout.fillWidth: true }

                    Text { text: "Trust Value: " + trustSlider.value.toFixed(2); color: mutedText; font.pixelSize: 10 }
                    Slider {
                        id: trustSlider
                        from: 0.0
                        to: 1.0
                        value: 0.5
                        stepSize: 0.01
                        Layout.fillWidth: true
                        onMoved: trustInput.text = value.toFixed(2)
                    }
                    TextField {
                        id: trustInput
                        text: "0.50"
                        Layout.fillWidth: true
                        onEditingFinished: {
                            const parsed = parseFloat(text)
                            if (!isNaN(parsed))
                                trustSlider.value = clampTrust(parsed)
                        }
                    }

                    Text { text: "Interaction Type"; color: mutedText; font.pixelSize: 10 }
                    ComboBox {
                        id: interactionTypeCombo
                        model: ["send", "receive", "validate", "forward", "store", "audit"]
                        Layout.fillWidth: true
                    }

                    Button {
                        text: "Create Peer"
                        Layout.fillWidth: true
                        onClicked: createPeerAction()
                    }
                    Button {
                        text: "Delete Peer"
                        Layout.fillWidth: true
                        onClicked: deletePeerAction()
                    }
                    Button {
                        text: "Set Trust"
                        Layout.fillWidth: true
                        onClicked: setTrustAction()
                    }
                    Button {
                        text: "Simulate Interaction"
                        Layout.fillWidth: true
                        onClicked: simulateInteractionAction(1)
                    }
                    Button {
                        text: "Reset System"
                        Layout.fillWidth: true
                        onClicked: resetSystemAction()
                    }
                    Button {
                        text: "Replay Last State"
                        Layout.fillWidth: true
                        onClicked: replayLastStateAction()
                    }
                    Button {
                        text: "Simulate 100 Interactions"
                        Layout.fillWidth: true
                        onClicked: simulateHundredInteractionsAction()
                    }
                    Button {
                        text: "Stress Trust Updates"
                        Layout.fillWidth: true
                        onClicked: stressTrustUpdatesAction()
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 74
                        color: panelAlt
                        border.color: borderColor
                        radius: 4

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            Text { text: "System Indicator"; color: mutedText; font.pixelSize: 9 }
                            Text { text: systemStateText; color: textColor; font.pixelSize: 11; wrapMode: Text.WordWrap }
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: panelBg
                border.color: borderColor
                radius: 6

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8

                    Text { text: "PEER STATE"; color: accent; font.pixelSize: 12; font.bold: true }

                    ListView {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 190
                        model: peerModel
                        clip: true
                        spacing: 6

                        delegate: Rectangle {
                            width: parent.width - 8
                            height: 54
                            color: index % 2 === 0 ? panelAlt : "#0b1324"
                            border.color: trust >= 0.7 ? good : (trust <= 0.3 ? bad : borderColor)
                            radius: 4

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 8
                                spacing: 8

                                ColumnLayout {
                                    Layout.preferredWidth: 130
                                    Text { text: peerId; color: textColor; font.pixelSize: 11; font.bold: true }
                                    Text { text: trend; color: mutedText; font.pixelSize: 9 }
                                }

                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 18
                                    radius: 9
                                    color: "#1f2937"

                                    Rectangle {
                                        width: parent.width * trust
                                        height: parent.height
                                        radius: 9
                                        color: trust >= 0.7 ? good : (trust <= 0.3 ? bad : warn)
                                    }

                                    Text {
                                        anchors.centerIn: parent
                                        text: (trust * 100).toFixed(0) + "%"
                                        color: "white"
                                        font.pixelSize: 9
                                    }
                                }

                                Text { text: interactions; color: textColor; font.pixelSize: 10; Layout.preferredWidth: 40 }
                                Text { text: positive; color: good; font.pixelSize: 10; Layout.preferredWidth: 40 }
                                Text { text: negative; color: bad; font.pixelSize: 10; Layout.preferredWidth: 40 }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 140
                        color: panelAlt
                        border.color: borderColor
                        radius: 4

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            Text { text: "INTERACTION TIMELINE"; color: accent; font.pixelSize: 11; font.bold: true }
                            ListView {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                model: timelineModel
                                clip: true
                                delegate: Text {
                                    width: parent.width
                                    text: "#" + index + " " + timestamp + " | " + peerA + " -> " + peerB + " | " + type + " | " + (success ? "SUCCESS" : "FAILURE") + " | " + resultingState
                                    color: success ? good : warn
                                    font.pixelSize: 9
                                    wrapMode: Text.WordWrap
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.preferredWidth: 310
                Layout.fillHeight: true
                color: panelBg
                border.color: borderColor
                radius: 6

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8

                    Text { text: "EVENT LOG"; color: accent; font.pixelSize: 12; font.bold: true }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 70
                        color: panelAlt
                        border.color: borderColor
                        radius: 4
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            Text { text: "Backend State"; color: mutedText; font.pixelSize: 9 }
                            Text { text: backendAvailable ? "Adapter connected" : "No backend needed"; color: backendAvailable ? good : warn; font.pixelSize: 11; font.bold: true }
                            Text { text: "Peers: " + peerModel.count + " | Timeline: " + timelineModel.count; color: mutedText; font.pixelSize: 10 }
                        }
                    }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: eventModel
                        clip: true
                        spacing: 4

                        delegate: Rectangle {
                            width: parent.width - 6
                            height: 42
                            color: "#0b1324"
                            border.color: color
                            border.width: 1
                            radius: 4

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 6
                                Text { text: time + " [" + tag + "]"; color: color; font.pixelSize: 8 }
                                Text { text: message; color: textColor; font.pixelSize: 9; wrapMode: Text.WordWrap }
                            }
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        if (backendAvailable) {
            logEvent("SYSTEM", "Trust adapter connected", "info")
            adapter.stateUpdated.connect(function(peersJson, timelineJson, stateJson) {
                refreshFromAdapter(peersJson, timelineJson, stateJson)
            })
            adapter.peerAdded.connect(function(peerId) {
                logEvent("PEER", "Created peer " + peerId, "info")
            })
            adapter.peerRemoved.connect(function(peerId) {
                logEvent("PEER", "Removed peer " + peerId, "warn")
            })
            adapter.trustUpdated.connect(function(peerId, value) {
                logEvent("TRUST", peerId + " -> " + value.toFixed(2), "info")
            })
            adapter.interactionLogged.connect(function(peerA, peerB, type) {
                logEvent("INTERACTION", peerA + " / " + peerB + " / " + type, "info")
            })
            adapter.systemReset.connect(function() {
                logEvent("SYSTEM", "System reset", "warn")
            })
            adapter.errorOccurred.connect(function(message) {
                logEvent("ERROR", message, "error")
            })
            adapter.debugLog.connect(function(tag, message) {
                logEvent(tag, message, tag === "SYSTEM" ? "info" : "warn")
            })
            syncViewFromStateJson(adapter.stateSnapshotJson())
        } else {
            logEvent("SYSTEM", "Running in standalone fallback mode", "warn")
            syncViewFromLocal()
        }
    }
}