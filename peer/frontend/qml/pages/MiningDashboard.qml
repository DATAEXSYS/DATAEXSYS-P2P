import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#0b1020"

    readonly property color panelBg: "#111827"
    readonly property color panelAlt: "#0f172a"
    readonly property color borderColor: "#243047"
    readonly property color textColor: "#e5e7eb"
    readonly property color mutedText: "#94a3b8"
    readonly property color accent: "#22c55e"
    readonly property color warn: "#f59e0b"
    readonly property color danger: "#ef4444"
    readonly property color info: "#38bdf8"

    property var adapter: (typeof appController !== "undefined") ? appController.pkCertChainAdapter : null
    property bool simulationMode: adapter ? adapter.testMode : true
    property bool miningActive: false
    property int currentProgress: 0
    property string currentHash: "-"
    property string validationStatus: "Not validated"
    property string difficultyText: "1"
    property int selectedBlockIndex: -1

    ListModel { id: blockModel }
    ListModel { id: logModel }

    Timer {
        id: simulationTimer
        interval: 90
        repeat: true
        onTriggered: {
            if (!simulationMining)
                return
            currentProgress = Math.min(100, currentProgress + 4)
            currentHash = "SIM-" + root.hashSeed + "-" + currentProgress.toString(16).toUpperCase()
            if (currentProgress >= 100) {
                simulationMining = false
                miningActive = false
                simulationTimer.stop()
                finalizeSimulatedBlock()
                appendLog("SIM", "Simulation mining completed", "info")
            }
        }
    }

    property bool simulationMining: false
    property string hashSeed: ""
    property string pendingBlockData: ""

    function appendLog(tag, message, level) {
        const color = level === "error" ? danger : level === "warn" ? warn : info
        logModel.insert(0, {
            "time": new Date().toLocaleTimeString(),
            "tag": tag,
            "message": message,
            "color": color
        })
        while (logModel.count > 150)
            logModel.remove(logModel.count - 1)
    }

    function refreshBlocksFromJson(chainStateJson) {
        try {
            const state = JSON.parse(chainStateJson)
            blockModel.clear()
            const blocks = state.blocks || []
            for (let i = 0; i < blocks.length; ++i) {
                const block = blocks[i]
                blockModel.append({
                    "index": block.index,
                    "data": block.data,
                    "previousHash": block.previousHash,
                    "hash": block.hash,
                    "nonce": block.nonce,
                    "difficulty": block.difficulty,
                    "timestamp": block.timestamp,
                    "mined": block.mined,
                    "valid": block.valid
                })
            }
            validationStatus = state.isValid ? "Chain valid" : (state.validationMessage || "Chain invalid")
            difficultyText = String(state.activeDifficulty || difficultySlider.value)
            simulationMode = !!state.testMode
            currentHash = blocks.length > 0 ? blocks[blocks.length - 1].hash : "-"
            selectedBlockIndex = blocks.length > 0 ? blocks.length - 1 : -1
        } catch (e) {
            appendLog("ERROR", "Failed to parse chain JSON", "error")
        }
    }

    function addLocalPendingBlock(data) {
        const index = blockModel.count
        const previousHash = index > 0 ? blockModel.get(index - 1).hash : "0000000000000000"
        blockModel.append({
            "index": index,
            "data": data,
            "previousHash": previousHash,
            "hash": "PENDING",
            "nonce": 0,
            "difficulty": difficultySlider.value,
            "timestamp": new Date().toLocaleTimeString(),
            "mined": false,
            "valid": false
        })
        selectedBlockIndex = index
        currentHash = "PENDING"
        appendLog("CREATE", "Created pending block in simulation mode", "info")
    }

    function finalizeSimulatedBlock() {
        if (selectedBlockIndex < 0 || selectedBlockIndex >= blockModel.count)
            return
        const block = blockModel.get(selectedBlockIndex)
        blockModel.set(selectedBlockIndex, {
            "index": block.index,
            "data": block.data,
            "previousHash": block.previousHash,
            "hash": currentHash,
            "nonce": Math.floor(currentProgress * 200),
            "difficulty": block.difficulty,
            "timestamp": new Date().toLocaleTimeString(),
            "mined": true,
            "valid": true
        })
        validationStatus = "Chain valid (simulation)"
        appendLog("BLOCK", "Block mined in simulation mode", "info")
    }

    function startMiningAction() {
        const difficulty = Math.max(1, parseInt(difficultySlider.value))
        if (adapter) {
            adapter.startMining(difficulty)
            appendLog("MINING", "Mining requested at difficulty " + difficulty, "info")
            return
        }

        simulationMining = true
        miningActive = true
        currentProgress = 0
        hashSeed = String(Date.now() % 100000)
        if (selectedBlockIndex < 0 || selectedBlockIndex >= blockModel.count) {
            addLocalPendingBlock(blockDataInput.text.trim() || "Auto-generated block")
        }
        appendLog("SIM", "Starting simulation mining", "info")
        simulationTimer.start()
    }

    function stopMiningAction() {
        if (adapter) {
            adapter.stopMining()
        }
        simulationMining = false
        miningActive = false
        simulationTimer.stop()
        appendLog("MINING", "Stop requested", "warn")
    }

    function createBlockAction() {
        const data = blockDataInput.text.trim()
        if (data === "") {
            appendLog("WARN", "Block data is required", "warn")
            return
        }
        if (adapter) {
            adapter.createBlock(data)
        } else {
            addLocalPendingBlock(data)
        }
    }

    function validateChainAction() {
        if (adapter) {
            adapter.validateChain()
        } else {
            validationStatus = blockModel.count === 0 ? "Chain empty" : "Chain valid (simulation)"
            appendLog("VALIDATE", validationStatus, blockModel.count === 0 ? "warn" : "info")
        }
    }

    function stressTestAction() {
        appendLog("STRESS", "Starting rapid block creation test", "warn")
        if (adapter) {
            for (let i = 0; i < 3; ++i) {
                adapter.createBlock("Stress block " + (i + 1) + " @ " + new Date().toLocaleTimeString())
            }
            adapter.startMining(Math.max(1, parseInt(difficultySlider.value)))
        } else {
            for (let i = 0; i < 3; ++i)
                addLocalPendingBlock("Stress block " + (i + 1))
            startMiningAction()
        }
    }

    function toggleTestMode() {
        if (adapter) {
            adapter.setTestMode(testModeSwitch.checked)
        }
        simulationMode = testModeSwitch.checked
        appendLog("MODE", testModeSwitch.checked ? "Test mode enabled" : "Real mode enabled", "info")
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 54
            radius: 6
            color: panelBg
            border.color: borderColor

            RowLayout {
                anchors.fill: parent
                anchors.margins: 12
                Text {
                    text: "PKCertChain Mining Dashboard"
                    color: textColor
                    font.pixelSize: 15
                    font.bold: true
                }
                Item { Layout.fillWidth: true }
                Text {
                    text: miningActive ? "MINING" : "IDLE"
                    color: miningActive ? accent : mutedText
                    font.pixelSize: 11
                    font.bold: true
                }
                Text {
                    text: simulationMode ? "TEST MODE" : "REAL MODE"
                    color: simulationMode ? warn : info
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
                radius: 6
                color: panelBg
                border.color: borderColor

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8

                    Text { text: "INPUT"; color: info; font.pixelSize: 12; font.bold: true }

                    Text { text: "Difficulty: " + Math.round(difficultySlider.value); color: mutedText; font.pixelSize: 10 }
                    Slider {
                        id: difficultySlider
                        from: 1
                        to: 6
                        stepSize: 1
                        value: 2
                        Layout.fillWidth: true
                    }

                    Text { text: "Block Data"; color: mutedText; font.pixelSize: 10 }
                    TextArea {
                        id: blockDataInput
                        text: "Genesis payload"
                        Layout.fillWidth: true
                        Layout.preferredHeight: 110
                        wrapMode: Text.WordWrap
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Switch {
                            id: testModeSwitch
                            checked: true
                            onToggled: toggleTestMode()
                        }
                        Text { text: "Test Mode Fallback"; color: mutedText; font.pixelSize: 10 }
                    }

                    Button {
                        text: "Start Mining"
                        Layout.fillWidth: true
                        onClicked: startMiningAction()
                    }
                    Button {
                        text: "Stop Mining"
                        Layout.fillWidth: true
                        onClicked: stopMiningAction()
                    }
                    Button {
                        text: "Create Block"
                        Layout.fillWidth: true
                        onClicked: createBlockAction()
                    }
                    Button {
                        text: "Validate Chain"
                        Layout.fillWidth: true
                        onClicked: validateChainAction()
                    }
                    Button {
                        text: "Stress Test Mode"
                        Layout.fillWidth: true
                        onClicked: stressTestAction()
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 56
                        color: panelAlt
                        border.color: borderColor
                        radius: 4

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            Text { text: "Difficulty Indicator"; color: mutedText; font.pixelSize: 9 }
                            Text { text: difficultyText; color: accent; font.pixelSize: 16; font.bold: true }
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 6
                color: panelBg
                border.color: borderColor

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8

                    Text { text: "BLOCKCHAIN STATE"; color: info; font.pixelSize: 12; font.bold: true }

                    ProgressBar {
                        id: progressBar
                        Layout.fillWidth: true
                        from: 0
                        to: 100
                        value: currentProgress
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 70
                            radius: 4
                            color: panelAlt
                            border.color: borderColor
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 8
                                Text { text: "Current Hash"; color: mutedText; font.pixelSize: 9 }
                                Text {
                                    text: currentHash
                                    color: accent
                                    font.family: "Monospace"
                                    font.pixelSize: 12
                                    wrapMode: Text.WrapAnywhere
                                }
                            }
                        }
                        Rectangle {
                            Layout.preferredWidth: 180
                            Layout.preferredHeight: 70
                            radius: 4
                            color: panelAlt
                            border.color: borderColor
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 8
                                Text { text: "Validation"; color: mutedText; font.pixelSize: 9 }
                                Text { text: validationStatus; color: miningActive ? warn : accent; font.pixelSize: 11; wrapMode: Text.WordWrap }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 220
                        radius: 4
                        color: panelAlt
                        border.color: borderColor

                        ListView {
                            id: blockView
                            anchors.fill: parent
                            anchors.margins: 8
                            model: blockModel
                            spacing: 6
                            clip: true
                            delegate: Rectangle {
                                width: blockView.width - 8
                                height: 64
                                radius: 4
                                color: index % 2 === 0 ? "#111827" : "#0f172a"
                                border.color: mined ? accent : (valid ? info : danger)
                                border.width: 1

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 8
                                    spacing: 8

                                    Rectangle {
                                        Layout.preferredWidth: 70
                                        Layout.preferredHeight: 44
                                        radius: 4
                                        color: mined ? accent : warn
                                        Text {
                                            anchors.centerIn: parent
                                            text: "#" + index
                                            color: "white"
                                            font.pixelSize: 12
                                            font.bold: true
                                        }
                                    }

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 2
                                        Text { text: data; color: textColor; font.pixelSize: 10; elide: Text.ElideRight }
                                        Text { text: hash; color: mined ? accent : warn; font.family: "Monospace"; font.pixelSize: 9; elide: Text.ElideRight }
                                        Text { text: previousHash; color: mutedText; font.family: "Monospace"; font.pixelSize: 8; elide: Text.ElideRight }
                                    }
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    onClicked: {
                                        selectedBlockIndex = index
                                        currentHash = hash
                                    }
                                }
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        radius: 4
                        color: panelAlt
                        border.color: borderColor
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 8
                            Text { text: "CPU / Simulation Meter"; color: mutedText; font.pixelSize: 9 }
                            Item { Layout.fillWidth: true }
                            Rectangle {
                                Layout.preferredWidth: 180
                                Layout.preferredHeight: 10
                                radius: 5
                                color: "#1f2937"
                                Rectangle {
                                    width: Math.max(8, parent.width * ((miningActive ? currentProgress : 18) / 100))
                                    height: parent.height
                                    radius: 5
                                    color: miningActive ? accent : info
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                Layout.preferredWidth: 300
                Layout.fillHeight: true
                radius: 6
                color: panelBg
                border.color: borderColor

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    spacing: 8

                    Text { text: "EVENT LOG"; color: info; font.pixelSize: 12; font.bold: true }

                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        model: logModel
                        clip: true
                        spacing: 4
                        delegate: Rectangle {
                            width: parent.width - 8
                            height: 42
                            color: "#0f172a"
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
        appendLog("SYSTEM", "Mining dashboard loaded", "info")

        if (adapter) {
            adapter.miningStarted.connect(function() {
                miningActive = true
                appendLog("MINING", "Mining started", "info")
            })
            adapter.miningProgress.connect(function(percent) {
                currentProgress = percent
            })
            adapter.blockMined.connect(function(blockData) {
                try {
                    const block = JSON.parse(blockData)
                    currentHash = block.hash
                    selectedBlockIndex = block.index
                    appendLog("BLOCK", "Block #" + block.index + " mined", "info")
                } catch (e) {
                    appendLog("BLOCK", "Block mined", "info")
                }
            })
            adapter.chainUpdated.connect(function(chainState) {
                refreshBlocksFromJson(chainState)
            })
            adapter.miningStopped.connect(function() {
                miningActive = false
                currentProgress = 0
                appendLog("MINING", "Mining stopped", "warn")
            })
            adapter.errorOccurred.connect(function(message) {
                appendLog("ERROR", message, "error")
            })
            refreshBlocksFromJson(adapter.chainStateJson())
        } else {
            appendLog("SIM", "Adapter unavailable, local simulation mode enabled", "warn")
        }
    }
}
