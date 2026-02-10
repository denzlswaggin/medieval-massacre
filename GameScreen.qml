import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia


Rectangle {
    id: gameScreenRoot
    color: "#0f0f1e"

    property StackView navStack: StackView.view
    property bool controllerValid: typeof gameController !== 'undefined' && gameController !== null

    // Stav nakupu
    property int selectedBuyType: -1      // -1 = nic, 0-3 = jednotky
    property int selectedBuildType: -1    // -1 = nic, 0-2 = budovy
    property int globalWaterFrame: 0
    property int globalTreeFrame: 0
    property int globalUnitFrame: 0
    property real zoomLevel: 1.0

    // Custom pixel font
    FontLoader {
        id: pixelFont
        source: "qrc:/Projekt_CPP/fonts/pixelated.ttf"
    }
    
    FontLoader {
        id: pixelFont2
        source: "qrc:/Projekt_CPP/fonts/pixelated2.ttf"
    }
    
    // Model pro herni log
    ListModel {
        id: gameLogModel
    }
    
    // Funkce pro pridani zpravy do logu
    function addLogMessage(message) {
        gameLogModel.append({"text": message})
        // Omezime pocet zprav na 50
        while (gameLogModel.count > 50) {
            gameLogModel.remove(0)
        }
    }

    Timer {
        interval: 100
        running: true
        repeat: true
        onTriggered: {
            globalWaterFrame = (globalWaterFrame + 1) % 16
            globalTreeFrame = (globalTreeFrame + 1) % 8
            globalUnitFrame = (globalUnitFrame + 1) % 24
        }
    }

    // Zvuky jednotek
    SoundEffect { id: swordsmanBuySound; source: "qrc:/Projekt_CPP/sounds/swordsman_buy.wav"; volume: 0.5 }
    SoundEffect { id: archerBuySound; source: "qrc:/Projekt_CPP/sounds/archer_buy.wav"; volume: 0.5 }
    SoundEffect { id: cavalryBuySound; source: "qrc:/Projekt_CPP/sounds/cavalry_buy.wav"; volume: 0.5 }
    SoundEffect { id: bardBuySound; source: "qrc:/Projekt_CPP/sounds/bard_buy.wav"; volume: 0.5 }

    SoundEffect { id: swordsmanAttackSound; source: "qrc:/Projekt_CPP/sounds/swordsman_attack.wav"; volume: 0.5 }
    SoundEffect { id: archerAttackSound; source: "qrc:/Projekt_CPP/sounds/archer_attack.wav"; volume: 0.5 }
    SoundEffect { id: cavalryAttackSound; source: "qrc:/Projekt_CPP/sounds/cavalry_attack.wav"; volume: 0.5 }
    SoundEffect { id: bardAttackSound; source: "qrc:/Projekt_CPP/sounds/bard_attack.wav"; volume: 0.5 }

    SoundEffect{id:warriorBlockSound; source:"qrc:/Projekt_CPP/sounds/Warrior_Shield.wav"; volume: 0.5 }

    // Zvuky budov
    SoundEffect { id: castleBuySound; source: "qrc:/Projekt_CPP/sounds/castle_place.wav"; volume: 0.5 }
    SoundEffect { id: barracksBuySound; source: "qrc:/Projekt_CPP/sounds/barracks_place.wav"; volume: 0.5 }
    SoundEffect { id: churchBuySound; source: "qrc:/Projekt_CPP/sounds/church_place.wav"; volume: 0.5 }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // --- HORNI LISTA ---
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: "#1a1a2e"
            RowLayout {
                anchors.fill: parent; anchors.margins: 10; spacing: 20

                Item { Layout.fillWidth: true }
                Text { text: "Gold: " + gameController.currentPlayerGold; font.family: pixelFont.name; font.pixelSize: 22; color: "#ffd700" }
                Text { text: "Units: " + gameController.currentPlayerUnits; font.family: pixelFont.name; font.pixelSize: 22; color: "#4fc3f7" }
                Text { text: "Buildings: " + gameController.currentPlayerBuildings; font.family: pixelFont.name; font.pixelSize: 22; color: "#a5d6a7" }
                Button {
                    text: "End Turn"
                    font.family: pixelFont.name
                    Layout.preferredWidth: 120; Layout.preferredHeight: 40
                    onClicked: {
                        selectedBuyType = -1
                        selectedBuildType = -1
                        gameController.endTurn()
                    }
                }
            }
        }

        // --- HLAVNI OBLAST (MAPA + LOG) ---
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // --- HERNI PLOCHA (MAPA) ---
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#0a0a14"

                // PLAYER TURN OVERLAY - primo ve hre nahore uprostred
                Text {
                    id: playerTurnOverlay
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    z: 500
                    
                    text: gameController.isGameOver ? "GAME OVER!" :
                          (gameController.currentPlayerUnits === 0 && gameController.currentPlayerBuildings === 0) ?
                          "PLACE YOUR CASTLE!" :
                          gameController.currentPlayerName + "'s Turn"
                    
                    font.family: pixelFont.name
                    font.pixelSize: 32
                    font.bold: true
                    
                    color: {
                        if (gameController.isGameOver) return "#ff4444";
                        if (gameController.currentPlayerUnits === 0 && gameController.currentPlayerBuildings === 0) return "#ffd700";
                        return gameController.currentPlayerIndex === 0 ? "#e74c3c" : "#3498db";
                    }
                    
                    // Outline efekt pro lepsi citelnost
                    style: Text.Outline
                    styleColor: "#000000"
                    
                    // Animace pri zmene tahu
                    opacity: 1.0
                    scale: 1.0
                    
                    Behavior on color {
                        ColorAnimation { duration: 300 }
                    }
                    
                    // Pulse animace pri zmene
                    SequentialAnimation {
                        id: turnChangeAnim
                        NumberAnimation { target: playerTurnOverlay; property: "scale"; to: 1.3; duration: 150 }
                        NumberAnimation { target: playerTurnOverlay; property: "scale"; to: 1.0; duration: 150 }
                    }
                    
                    Connections {
                        target: gameController
                        function onTurnChanged() {
                            turnChangeAnim.start()
                        }
                    }
                }

                Flickable {
                    anchors.fill: parent
                    clip: true
                    boundsBehavior: Flickable.StopAtBounds

                    contentWidth: mapContainer.width * gameScreenRoot.zoomLevel
                    contentHeight: mapContainer.height * gameScreenRoot.zoomLevel

                    leftMargin: (width - contentWidth) > 0 ? (width - contentWidth) / 2 : 0
                    topMargin: (height - contentHeight) > 0 ? (height - contentHeight) / 2 : 0

                    Component.onCompleted: {
                        contentX = (contentWidth - width) / 2
                        contentY = (contentHeight - height) / 2
                    }

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.NoButton
                        onWheel: (wheel) => {
                            if (wheel.angleDelta.y > 0) {
                                gameScreenRoot.zoomLevel = Math.min(gameScreenRoot.zoomLevel + 0.1, 2.5)
                            } else {
                                gameScreenRoot.zoomLevel = Math.max(gameScreenRoot.zoomLevel - 0.1, 0.5)
                            }
                        }
                    }

                    Item {
                        id: mapContainer
                        width: mapGrid.width
                        height: mapGrid.height

                        scale: gameScreenRoot.zoomLevel
                        transformOrigin: Item.TopLeft

                        Grid {
                            id: mapGrid
                            columns: gameController.mapWidth

                            Repeater {
                                id: mapRepeater
                                model: gameController.mapModel

                                delegate: MapTile {
                                    currentBuyType: gameScreenRoot.selectedBuyType
                                    currentBuildType: gameScreenRoot.selectedBuildType
                                    currentFrameIndex: gameScreenRoot.globalWaterFrame
                                }
                            }
                        }

                        // Vrstva budov (pod jednotkami)
                        BuildingLayer {
                            anchors.fill: mapGrid
                            z: 500
                        }

                        // Vrstva jednotek
                        UnitLayer {
                            anchors.fill: mapGrid
                            z: 600
                        }
                    }
                }
            }

            // --- GAME LOG PANEL ---
            Rectangle {
                Layout.preferredWidth: 280
                Layout.fillHeight: true
                color: "#12121f"
                border.color: "#333"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 5

                    // Nadpis
                    Text {
                        text: "GAME LOG"
                        font.family: pixelFont.name
                        font.pixelSize: 16
                        font.bold: true
                        color: "#ffd700"
                        Layout.alignment: Qt.AlignHCenter
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        height: 2
                        color: "#333"
                    }

                    // Log ListView
                    ListView {
                        id: gameLogView
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        model: gameLogModel
                        spacing: 3

                        delegate: Text {
                            width: gameLogView.width
                            text: model.text
                            font.family: pixelFont.name
                            font.pixelSize: 12
                            color: {
                                if (model.text.indexOf("GAME OVER") !== -1 || model.text.indexOf("WINS") !== -1) return "#ff4444";
                                if (model.text.indexOf("===") !== -1) return "#ffd700";
                                if (model.text.indexOf("---") !== -1) return "#4fc3f7";
                                if (model.text.indexOf("KILL") !== -1) return "#ff6b6b";
                                if (model.text.indexOf("BLOCKED") !== -1) return "#9b59b6";
                                if (model.text.indexOf("healed") !== -1) return "#2ecc71";
                                if (model.text.indexOf("deals") !== -1) return "#e74c3c";
                                if (model.text.indexOf("recruited") !== -1) return "#3498db";
                                if (model.text.indexOf("built") !== -1) return "#27ae60";
                                return "#aaa";
                            }
                            wrapMode: Text.WordWrap
                        }

                        // Auto-scroll na konec
                        onCountChanged: {
                            Qt.callLater(function() {
                                gameLogView.positionViewAtEnd()
                            })
                        }
                    }
                }
            }
        }

        // --- DOLNI LISTA (NAKUP) ---
        Rectangle {
            Layout.fillWidth: true; Layout.preferredHeight: 140
            color: "#16213e"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 5
                spacing: 5

                // Radek jednotek
                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 10

                    Text {
                        text: "Units:"
                        color: "#aaa"
                        font.family: pixelFont.name
                        font.pixelSize: 14
                    }

                    Repeater {
                        model: ListModel {
                            ListElement { name: "Swordsman"; cost: 50; type: 0 }
                            ListElement { name: "Archer"; cost: 60; type: 1 }
                            ListElement { name: "Cavalry"; cost: 80; type: 2 }
                            ListElement { name: "Bard"; cost: 70; type: 3 }
                        }

                        delegate: Button {
                            property int effectiveCost: gameController.getEffectiveUnitCost(type)
                            text: name + "\n" + effectiveCost + "g"
                            Layout.preferredWidth: 90; Layout.preferredHeight: 50
                            background: Rectangle {
                                color: (selectedBuyType === type) ? "#ffd700" : (gameController.currentPlayerGold >= effectiveCost ? "#3498db" : "#555")
                                radius: 4
                            }
                            contentItem: Text {
                                text: parent.text
                                color: (gameController.currentPlayerGold >= parent.effectiveCost) ? "white" : "#aaa"
                                horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                                font.family: pixelFont.name
                                font.pixelSize: 14
                            }
                            enabled: gameController.currentPlayerGold >= effectiveCost
                            onClicked: {
                                gameController.deselectUnit()
                                selectedBuildType = -1
                                selectedBuyType = type
                            }
                        }
                    }
                }

                // Radek budov
                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 10

                    Text {
                        text: "Buildings:"
                        color: "#aaa"
                        font.family: pixelFont.name
                        font.pixelSize: 14
                    }

                    Repeater {
                        model: ListModel {
                            ListElement { name: "Castle"; cost: 150; type: 0; desc: "+5g/turn" }
                            ListElement { name: "Barracks"; cost: 100; type: 1; desc: "-20% units" }
                            ListElement { name: "Church"; cost: 120; type: 2; desc: "Heals" }
                        }

                        delegate: Button {
                            text: name + "\n" + cost + "g\n" + desc
                            Layout.preferredWidth: 90; Layout.preferredHeight: 60
                            background: Rectangle {
                                color: (selectedBuildType === type) ? "#ffd700" : (gameController.currentPlayerGold >= cost ? "#27ae60" : "#555")
                                radius: 4
                            }
                            contentItem: Text {
                                text: parent.text
                                color: (gameController.currentPlayerGold >= cost) ? "white" : "#aaa"
                                horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                                font.family: pixelFont.name
                                font.pixelSize: 14
                            }
                            enabled: gameController.currentPlayerGold >= cost
                            onClicked: {
                                gameController.deselectUnit()
                                selectedBuyType = -1
                                selectedBuildType = type
                            }
                        }
                    }

                    Item { width: 20 }

                    Button {
                        text: "Back/Cancel"
                        font.family: pixelFont.name
                        Layout.preferredWidth: 100
                        Layout.preferredHeight: 40
                        onClicked: {
                            if (selectedBuyType !== -1 || selectedBuildType !== -1) {
                                selectedBuyType = -1
                                selectedBuildType = -1
                            } else {
                                gameController.deselectUnit()
                                navStack.pop()
                            }
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: gameController
        function onUnitBought(unitType){
            selectedBuyType = -1
            switch(unitType){
                case 0: swordsmanBuySound.play(); break;
                case 1: archerBuySound.play(); break;
                case 2: cavalryBuySound.play(); break;
                case 3: bardBuySound.play(); break;
            }
        }
        function onBuildingBought(buildingType){
            selectedBuildType = -1
            switch(buildingType){
                case 0: castleBuySound.play(); break;
                case 1: barracksBuySound.play(); break;
                case 2: churchBuySound.play(); break;
            }
        }
        function onUnitAttacked(attackerType){
            switch(attackerType){
                case 0: swordsmanAttackSound.play(); break;
                case 1: archerAttackSound.play(); break;
                case 2: cavalryAttackSound.play(); break;
                case 3: bardAttackSound.play(); break;
            }
        }
        function onBlockAnimationTriggered(unitId) {
            warriorBlockSound.play();
        }
        function onGameLogMessage(message) {
            addLogMessage(message);
        }
    }
}
