import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#111827"

    readonly property color panelBg: "#1f2937"
    readonly property color borderColor: "#334155"
    readonly property color textColor: "#e5e7eb"
    readonly property color mutedText: "#9ca3af"
    readonly property color okColor: "#22c55e"
    readonly property color warnColor: "#f59e0b"
    readonly property color errorColor: "#ef4444"
    readonly property color accentColor: "#38bdf8"

    property var adapter: (typeof appController !== "undefined") ? appController.rollingSignaturesAdapter : null
    property string selectedPacketId: ""
    property bool localSimulationMode: adapter === null

    ListModel { id: packetModel }
    ListModel { id: eventLogModel }
    ListModel { id: tamperModel }

    function logEvent(tag, message, level) {
        const color = level === "error" ? errorColor : (level === "warn" ? warnColor : accentColor)
        eventLogModel.insert(0, {
            "time": new Date().toLocaleTimeString(),
            "tag": tag,
            "message": message,
            "color": color
        })
        while (eventLogModel.count > 120)
            eventLogModel.remove(eventLogModel.count - 1)
    }

    function upsertPacket(stateJson) {
        try {
            const state = JSON.parse(stateJson)
            if (!state.packetId)
                return

            let index = -1
            for (let i = 0; i < packetModel.count; ++i) {
                if (packetModel.get(i).packetId === state.packetId) {
                    index = i
                    break
                }
            }

            const route = state.route || []
            const hopCount = Math.max(1, state.hopCount || (route.length > 1 ? route.length - 1 : 1))
            const progress = Math.min(1.0, (state.currentHop || 0) / hopCount)

            const row = {
                "packetId": state.packetId,
                "source": state.source || "-",
                "destination": state.destination || "-",
                "routeText": route.join(" -> "),
                "routeArray": route,
                "currentHop": state.currentHop || 0,
                "hopCount": hopCount,
                "verificationStatus": state.verificationStatus || "PENDING",
                "tampered": !!state.tampered,
                "progress": progress,
                "payload": state.payload || ""
            }

            if (index === -1) {
                packetModel.append(row)
            } else {
                packetModel.set(index, row)
            }

            if (selectedPacketId === "")
                selectedPacketId = state.packetId
        } catch (e) {
            logEvent("ERROR", "Failed to parse packet state JSON", "error")
        }
    }

    function selectedPacketData() {
        for (let i = 0; i < packetModel.count; ++i) {
            const row = packetModel.get(i)
            if (row.packetId === selectedPacketId)
                return row
        }
        return null
    }

    function createPacketAction() {
        const src = sourceInput.text.trim()
        const dst = destinationInput.text.trim()
        const payload = payloadInput.text.trim()

        if (src === "" || dst === "" || payload === "") {
            logEvent("WARN", "Source, destination and payload are required", "warn")
            return
        }

        if (adapter) {
            adapter.createPacket(src, dst, payload)
            return
        }

        const fakeId = "sim_" + Date.now()
        const route = [src, "Relay-B", "Relay-C", dst]
        upsertPacket(JSON.stringify({
            packetId: fakeId,
            source: src,
            destination: dst,
            payload: payload,
            route: route,
            currentHop: 0,
            hopCount: route.length - 1,
            verificationStatus: "SIM_PENDING",
            tampered: false
        }))
        logEvent("SIM", "Created packet in local simulation mode", "info")
    }

    function forwardPacketAction() {
        if (selectedPacketId === "") {
            logEvent("WARN", "Select a packet first", "warn")
            return
        }

        if (adapter) {
            adapter.forwardPacket(selectedPacketId)
            return
        }

        const row = selectedPacketData()
        if (!row)
            return

        const nextHop = Math.min(row.hopCount, row.currentHop + 1)
        upsertPacket(JSON.stringify({
            packetId: row.packetId,
            source: row.source,
            destination: row.destination,
            payload: row.payload,
            route: row.routeArray,
            currentHop: nextHop,
            hopCount: row.hopCount,
            verificationStatus: "SIM_IN_TRANSIT",
            tampered: row.tampered
        }))
        logEvent("SIM", "Forwarded packet in local simulation mode", "info")
    }

    function verifyPacketAction() {
        if (selectedPacketId === "") {
            logEvent("WARN", "Select a packet first", "warn")
            return
        }

        if (adapter) {
            adapter.verifyPacket(selectedPacketId)
            return
        }

        const row = selectedPacketData()
        if (!row)
            return

        const status = row.tampered ? "SIM_TAMPERED" : "SIM_VERIFIED"
        upsertPacket(JSON.stringify({
            packetId: row.packetId,
            source: row.source,
            destination: row.destination,
            payload: row.payload,
            route: row.routeArray,
            currentHop: row.currentHop,
            hopCount: row.hopCount,
            verificationStatus: status,
            tampered: row.tampered
        }))
        logEvent("SIM", "Verification executed in local simulation mode", row.tampered ? "warn" : "info")
    }

    function tamperAction() {
        if (selectedPacketId === "") {
            logEvent("WARN", "Select a packet first", "warn")
            return
        }

        if (adapter) {
            adapter.injectTamper(selectedPacketId)
            return
        }

        const row = selectedPacketData()
        if (!row)
            return

        upsertPacket(JSON.stringify({
            packetId: row.packetId,
            source: row.source,
            destination: row.destination,
            payload: "TAMPERED::" + row.payload,
            route: row.routeArray,
            currentHop: row.currentHop,
            hopCount: row.hopCount,
            verificationStatus: "SIM_TAMPERED",
            tampered: true
        }))
        tamperModel.insert(0, {
            "packetId": row.packetId,
            "reason": "Manual tamper injection (simulation fallback)",
            "time": new Date().toLocaleTimeString()
        })
        while (tamperModel.count > 30)
            tamperModel.remove(tamperModel.count - 1)
        logEvent("SIM", "Tamper injected in local simulation mode", "warn")
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            color: panelBg
            border.color: borderColor
            radius: 6

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                Text {
                    text: "RollingSignatures Packet Routing Dashboard"
                    color: textColor
                    font.pixelSize: 14
                    font.bold: true
                }

                Item { Layout.fillWidth: true }

                Text {
                    text: localSimulationMode ? "UI FALLBACK MODE" : (adapter.status || "READY")
                    color: localSimulationMode ? warnColor : accentColor
                    font.pixelSize: 11
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10

            Rectangle {
                Layout.preferredWidth: 340
                Layout.fillHeight: true
                color: panelBg
                border.color: borderColor
                radius: 6

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8

                    Text { text: "INPUT"; color: accentColor; font.pixelSize: 12; font.bold: true }

                    Text { text: "Source Node"; color: mutedText; font.pixelSize: 10 }
                    TextField { id: sourceInput; text: "Node-A"; Layout.fillWidth: true }

                    Text { text: "Destination Node"; color: mutedText; font.pixelSize: 10 }
                    TextField { id: destinationInput; text: "Node-Z"; Layout.fillWidth: true }

                    Text { text: "Payload"; color: mutedText; font.pixelSize: 10 }
                    TextArea {
                        id: payloadInput
                        text: "Telemetry payload"
                        Layout.fillWidth: true
                        Layout.preferredHeight: 90
                        wrapMode: Text.WordWrap
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Switch {
                            id: debugSwitch
                            checked: adapter ? adapter.debugMode : true
                            onToggled: {
                                if (adapter)
                                    adapter.setDebugMode(checked)
                                logEvent("CONFIG", "Debug mode " + (checked ? "ON" : "OFF"), "info")
                            }
                        }
                        Text { text: "Debug mode"; color: mutedText; font.pixelSize: 10 }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Switch {
                            id: simulationSwitch
                            checked: adapter ? adapter.simulationMode : true
                            onToggled: {
                                if (adapter)
                                    adapter.setSimulationMode(checked)
                                logEvent("CONFIG", "Simulation mode " + (checked ? "ON" : "OFF"), "info")
                            }
                        }
                        Text { text: "Simulation fallback"; color: mutedText; font.pixelSize: 10 }
                    }

                    Button {
                        text: "Create Packet"
                        Layout.fillWidth: true
                        onClicked: createPacketAction()
                    }
                    Button {
                        text: "Forward Packet"
                        Layout.fillWidth: true
                        onClicked: forwardPacketAction()
                    }
                    Button {
                        text: "Verify Packet"
                        Layout.fillWidth: true
                        onClicked: verifyPacketAction()
                    }
                    Button {
                        text: "Inject Tamper (Debug)"
                        Layout.fillWidth: true
                        enabled: debugSwitch.checked
                        onClicked: tamperAction()
                    }
                    Button {
                        text: "Invalid Packet Injection Test"
                        Layout.fillWidth: true
                        onClicked: {
                            if (adapter) {
                                adapter.runInvalidPacketInjectionTest()
                            } else {
                                logEvent("SIM", "Invalid packet test: simulation fallback has no persistent IDs", "warn")
                            }
                        }
                    }

                    Text { text: "Selected Packet ID"; color: mutedText; font.pixelSize: 10 }
                    ComboBox {
                        id: packetSelector
                        Layout.fillWidth: true
                        model: packetModel
                        textRole: "packetId"
                        onActivated: {
                            if (currentIndex >= 0)
                                selectedPacketId = packetModel.get(currentIndex).packetId
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

                    Text { text: "PACKETS"; color: accentColor; font.pixelSize: 12; font.bold: true }

                    ListView {
                        id: packetsView
                        Layout.fillWidth: true
                        Layout.preferredHeight: 210
                        model: packetModel
                        clip: true
                        spacing: 6

                        delegate: Rectangle {
                            width: packetsView.width
                            height: 56
                            color: packetId === selectedPacketId ? "#0f172a" : "#111827"
                            border.color: tampered ? errorColor : borderColor
                            radius: 4

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    selectedPacketId = packetId
                                }
                            }

                            Column {
                                anchors.fill: parent
                                anchors.margins: 6
                                spacing: 3

                                Text {
                                    text: packetId + " | " + source + " -> " + destination
                                    color: textColor
                                    font.pixelSize: 10
                                }
                                Text {
                                    text: "Status: " + verificationStatus + " | hop " + currentHop + "/" + hopCount
                                    color: tampered ? errorColor : mutedText
                                    font.pixelSize: 9
                                }
                                Rectangle {
                                    width: parent.width
                                    height: 6
                                    radius: 3
                                    color: "#1e293b"

                                    Rectangle {
                                        width: Math.max(6, parent.width * progress)
                                        height: parent.height
                                        radius: 3
                                        color: tampered ? errorColor : okColor
                                    }
                                }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 170
                        color: "#111827"
                        border.color: borderColor
                        radius: 4

                        Item {
                            anchors.fill: parent
                            anchors.margins: 8

                            property var selectedData: selectedPacketData()

                            Repeater {
                                model: selectedData ? selectedData.routeArray.length : 0
                                Rectangle {
                                    width: 92
                                    height: 26
                                    radius: 13
                                    color: "#1e293b"
                                    border.color: index <= (selectedData ? selectedData.currentHop : 0) ? okColor : borderColor

                                    x: (index * ((parent.width - 92) / Math.max(1, model - 1)))
                                    y: parent.height * 0.55

                                    Text {
                                        anchors.centerIn: parent
                                        text: selectedData ? selectedData.routeArray[index] : ""
                                        color: textColor
                                        font.pixelSize: 9
                                    }
                                }
                            }

                            Rectangle {
                                id: packetDot
                                visible: selectedData && selectedData.routeArray.length > 1
                                width: 12
                                height: 12
                                radius: 6
                                color: selectedData && selectedData.tampered ? errorColor : accentColor
                                y: parent.height * 0.45
                                x: {
                                    if (!selectedData)
                                        return 0
                                    const routeSize = Math.max(2, selectedData.routeArray.length)
                                    const step = (parent.width - 12) / (routeSize - 1)
                                    return Math.min(parent.width - 12, step * selectedData.currentHop)
                                }
                                Behavior on x { NumberAnimation { duration: 280; easing.type: Easing.InOutQuad } }
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 180
                            color: "#111827"
                            border.color: borderColor
                            radius: 4

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 8

                                Text { text: "TAMPER ALERTS"; color: errorColor; font.pixelSize: 11; font.bold: true }
                                ListView {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    model: tamperModel
                                    clip: true
                                    delegate: Text {
                                        width: parent.width
                                        text: time + " | " + packetId + " | " + reason
                                        color: errorColor
                                        font.pixelSize: 9
                                        wrapMode: Text.WordWrap
                                    }
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 180
                            color: "#111827"
                            border.color: borderColor
                            radius: 4

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 8

                                Text { text: "EVENT LOG"; color: accentColor; font.pixelSize: 11; font.bold: true }
                                ListView {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    model: eventLogModel
                                    clip: true
                                    delegate: Text {
                                        width: parent.width
                                        text: time + " [" + tag + "] " + message
                                        color: model.color
                                        font.pixelSize: 9
                                        wrapMode: Text.WordWrap
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        if (adapter) {
            logEvent("SYSTEM", "RollingSignatures adapter connected", "info")

            adapter.packetCreated.connect(function(packetId, route) {
                logEvent("CREATE", packetId + " route: " + route, "info")
            })

            adapter.packetForwarded.connect(function(packetId, fromNode, toNode) {
                logEvent("FORWARD", packetId + " " + fromNode + " -> " + toNode, "info")
            })

            adapter.packetVerified.connect(function(packetId, status) {
                logEvent("VERIFY", packetId + " status: " + status, status.indexOf("TAMPER") >= 0 ? "warn" : "info")
            })

            adapter.tamperDetected.connect(function(packetId, reason) {
                tamperModel.insert(0, {
                    "packetId": packetId,
                    "reason": reason,
                    "time": new Date().toLocaleTimeString()
                })
                while (tamperModel.count > 30)
                    tamperModel.remove(tamperModel.count - 1)
                logEvent("ALERT", packetId + " tamper: " + reason, "warn")
            })

            adapter.errorOccurred.connect(function(message) {
                logEvent("ERROR", message, "error")
            })

            adapter.eventStream.connect(function(tag, message) {
                logEvent(tag, message, tag === "ERROR" ? "error" : (tag === "ALERT" ? "warn" : "info"))
            })

            adapter.packetStateUpdated.connect(function(packetId, stateJson) {
                upsertPacket(stateJson)
            })
        } else {
            logEvent("SYSTEM", "Adapter missing, running in local fallback simulation", "warn")
        }
    }
}
