import QtQuick

Rectangle {
    id: rootTile
    width: 55
    height: 55

    property var tileData: model
    property int currentBuyType: -1
    property int currentBuildType: -1
    property int currentFrameIndex: (gameScreenRoot ? gameScreenRoot.globalWaterFrame : 0)
    property int globalTreeFrame: (gameScreenRoot ? gameScreenRoot.globalTreeFrame : 0)

    //null kontroly
    property bool isMoveTarget: {
        if (!tileData || !gameController) return false
        return gameController.isPosInValidMoves(tileData.x, tileData.y)
    }

    property bool isAttackTarget: {
        if (!tileData || !gameController) return false
        return gameController.isPosInValidAttacks(tileData.x, tileData.y)
    }

    //nacteni dlazdic
    property var northTile: null
    Component.onCompleted: {
        if (tileData) {
            northTile = gameController.getTileData(tileData.x, tileData.y - 1)
        }
    }

    // Pomocné proměnné pro schody a hory
    property bool hasNorthMountain: northTile && northTile.terrain === 2
    property int nStairs: (hasNorthMountain && northTile.stairsType !== undefined) ? northTile.stairsType : 0


    z: {

        if (tileData && tileData.hasUnit) return 200;


        if (nStairs > 0) return 100;


        return 0;
    }

    // Povolíme kreslení mimo hranice dlaždice
    clip: false


    color: "#46aba9"

    // trava voda
    Image {
        anchors.fill: parent
        fillMode: Image.Stretch
        smooth: false
        antialiasing: false

        source: {
            if (!tileData) return ""
            var folder = "qrc:/Projekt_CPP/images/"
            if (tileData.terrain === 3) return "" // Voda se řeší jinde nebo je průhledná
            if (tileData.terrain === 5) {
                var mask = (tileData.bitmask !== undefined) ? tileData.bitmask : 15
                return folder + "grass_" + mask + ".png"
            }
            if (tileData.terrain === 0 || tileData.terrain === 1 || tileData.terrain === 2) {
                return folder + "grass_15.png"
            }
            return ""
        }
    }

    //hora matice
    Item {
        anchors.fill: parent
        visible: tileData && tileData.terrain === 2 // 2 = MOUNTAIN
        z: 10

        property int m: (tileData && tileData.mountainMask !== undefined) ? tileData.mountainMask : 15
        property int cliffId: {
            if (m === 15) return 5;
            if (m === 0) return 5;
            if (m === 6) return 1;
            if (m === 12) return 3;
            if (m === 3) return 7;
            if (m === 9) return 9;
            if (m === 14) return 2;
            if (m === 7) return 4;
            if (m === 13) return 6;
            if (m === 11) return 8;
            if (m === 1) return 8;
            if (m === 2) return 6;
            if (m === 4) return 2;
            if (m === 8) return 4;
            if (m === 5) return 5;
            if (m === 10) return 5;
            return 5;
        }

        Image {
            id: topLayer
            anchors.fill: parent
            source: "qrc:/Projekt_CPP/images/cliff_" + parent.cliffId + ".png"
            fillMode: Image.Stretch
        }
    }

    //cliff pod horou
    Item {
        anchors.fill: parent
        visible: tileData && tileData.terrain !== 2
        z: 5

        property int nm: (hasNorthMountain && northTile.mountainMask !== undefined) ? northTile.mountainMask : 0

        Image {
            anchors.fill: parent
            fillMode: Image.Stretch
            source: {
                if (!rootTile.hasNorthMountain) return "";
                if (parent.nm === 3) return "qrc:/Projekt_CPP/images/cliff_10.png";
                if (parent.nm === 11) return "qrc:/Projekt_CPP/images/cliff_11.png";
                if (parent.nm === 9) return "qrc:/Projekt_CPP/images/cliff_12.png";
                return "";
            }
        }

        //schody vlevo
        Image {
            id: stairsLeft
            source: "qrc:/Projekt_CPP/images/bridge.png"
            visible: (parent.nm === 3 && rootTile.nStairs === 1)

            width: 85
            height: 85
            fillMode: Image.PreserveAspectFit
            clip: false
            z: 20

            anchors.centerIn: parent
            mirror: false
            anchors.horizontalCenterOffset: -18
            anchors.verticalCenterOffset: -18
        }

        //schody vpravo
        Image {
            id: stairsRight
            source: "qrc:/Projekt_CPP/images/bridge.png"
            visible: (parent.nm === 9 && rootTile.nStairs === 2)

            width: 85
            height: 85
            fillMode: Image.PreserveAspectFit
            clip: false
            z: 20

            anchors.centerIn: parent
            mirror: true


            anchors.horizontalCenterOffset: 18
            anchors.verticalCenterOffset: -18
        }
    }

    //dekorace
    //strom
    Item {
        anchors.fill: parent
        visible: tileData && (tileData.terrain === 1 || tileData.terrain === 2)

        Item {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            width: 120
            height: 120
            clip: true
            visible: tileData && tileData.terrain === 1

            Image {
                source: "qrc:/Projekt_CPP/images/tree_strip.png"
                property int frameCount: 8
                fillMode: Image.PreserveAspectFit
                horizontalAlignment: Image.AlignLeft
                width: parent.width * frameCount
                height: parent.height
                x: - (rootTile.globalTreeFrame % frameCount) * (width / frameCount)
            }
        }
    }

    //BUSHES
    Item {
        anchors.fill: parent
        visible: tileData && (tileData.terrain === 0 || tileData.terrain === 5)
        property int randVal: (tileData) ? (tileData.x * 17 + tileData.y * 23) % 100 : 0
        property int mask: (tileData && tileData.bitmask !== undefined) ? tileData.bitmask : 15
        property bool hasBush: (mask === 15) ? (randVal < 10) : (randVal < 8)
        property string bushName: {
            if (mask === 15) return (randVal % 2 === 0) ? "Bushe1.png" : "Bushe2.png"
            else return (randVal % 2 === 0) ? "Bushe3.png" : "Bushe4.png"
        }

        Item {
            anchors.centerIn: parent
            visible: parent.hasBush
            width: 40; height: 40; clip: true
            Image {
                source: "qrc:/Projekt_CPP/images/" + parent.parent.bushName
                property int frameCount: 8
                width: parent.width * frameCount
                height: parent.height
                fillMode: Image.PreserveAspectFit
                horizontalAlignment: Image.AlignLeft
                x: - (rootTile.globalTreeFrame % frameCount) * (width / frameCount)
            }
        }
    }

    // Rock
    Image {
        anchors.centerIn: parent
        width: 40; height: 40
        fillMode: Image.PreserveAspectFit
        source: "qrc:/Projekt_CPP/images/rock.png"
        visible: tileData && (tileData.terrain === 0) && ((tileData.x * 13 + tileData.y * 37) % 100 > 92)
    }

    //mraky a shadow s nimi
    Item {
        anchors.fill: parent

        visible: tileData &&
                 (tileData.terrain === 3) &&
                 ((tileData.x * 7 + tileData.y * 13 + 1) % 100 < 5)

        //shadow
        Image{
            anchors.centerIn: parent

            anchors.horizontalCenterOffset: 10
            anchors.verticalCenterOffset: 10

            width: 90
            height: 90
            opacity: 0.3

            source: "qrc:/Projekt_CPP/images/Clouds_0" + ((tileData.x + tileData.y) % 3 + 1) + ".png"
            fillMode: Image.PreserveAspectFit
        }

        //mrak
        Image {
            anchors.centerIn: parent
            width:130
            height: 130

            fillMode: Image.PreserveAspectFit


            source: "qrc:/Projekt_CPP/images/Clouds_0" + ((tileData.x + tileData.y) % 3 + 1) + ".png"

            opacity: 0.85
        }
    }

    Item {
        id: waterRockItem
        anchors.centerIn: parent
        width: 80
        height: 80

        property bool isWater: (tileData && tileData.terrain === 3)
        property int seed: (tileData) ? (tileData.x * 73 + tileData.y * 19) : 0
        property bool hasRock: isWater && ((seed % 100) < 5)
        property int variant: (seed % 3) + 1
        visible: hasRock
        clip: true

        Image {
            id: rockSprite
            source: "qrc:/Projekt_CPP/images/Water_Rock" + parent.variant + ".png"

            property int frameCount: 8
            property int currentFrame: (gameScreenRoot ? gameScreenRoot.globalWaterFrame : 0) % frameCount

            x: -(currentFrame * parent.width)
            width: parent.width * frameCount
            height: parent.height

            fillMode: Image.PreserveAspectFit
            smooth: false
        }
    }

    //UI
    Rectangle {
        anchors.fill: parent
        z:60
        opacity: 0.3
        color: {
            if (isMoveTarget) return "#0e319d"
            if (isAttackTarget) return "#fa181f"
            return "transparent"
        }
        border.color: {
            if (!gameController || !tileData) return "transparent"

            // Zvýraznění pro nákup jednotek
            if (currentBuyType !== -1 && gameController.canBuyAt(tileData.x, tileData.y))
                return gameController.currentPlayerIndex === 0 ? "#e74c3c" : "#3498db"
            // Zvýraznění pro stavbu budov
            if (currentBuildType !== -1 && gameController.canBuildAt(tileData.x, tileData.y))
                return gameController.currentPlayerIndex === 0 ? "#c0392b" : "#2980b9"
            if (tileData && (isValidMove(tileData.x, tileData.y) || isValidAttack(tileData.x, tileData.y)))
                return gameController.currentPlayerIndex === 0 ? "#e74c3c" : "#3498db"
            return "transparent"
        }
        border.width: (border.color !== "transparent") ? 2 : 0
    }

    Rectangle {
        anchors.fill: parent
        z:60
        color: "transparent"
        border.color: {
            // Zvýraznění pro nákup jednotek
            if (currentBuyType !== -1 && gameController.canBuyAt(tileData.x, tileData.y))
                return gameController.currentPlayerIndex === 0 ? "#e74c3c" : "#3498db"
            // Zvýraznění pro stavbu budov
            if (currentBuildType !== -1 && gameController.canBuildAt(tileData.x, tileData.y))
                return gameController.currentPlayerIndex === 0 ? "#c0392b" : "#2980b9"
            if (tileData && (isValidMove(tileData.x, tileData.y) || isValidAttack(tileData.x, tileData.y)))
                return gameController.currentPlayerIndex === 0 ? "#e74c3c" : "#3498db"
            return "transparent"
        }
        border.width: (border.color !== "transparent") ? 2 : 0
    }

    // Indikátor stavby budovy
    Rectangle {
        anchors.fill: parent
        z: 55
        color: "#27ae60"
        opacity: 0.2
        visible: currentBuildType !== -1 && gameController.canBuildAt(tileData.x, tileData.y)
    }


    Rectangle {
        width: 12; height: 12; radius: 6
        anchors.bottom: parent.bottom; anchors.right: parent.right; anchors.margins: 4
        z: 101
        color: (!tileData || !tileData.hasUnit) ? "transparent" : (tileData.unitOwner === 0 ? "#e74c3c" : "#3498db")
        border.color: "white"; border.width: 1
        visible: (tileData && tileData.hasUnit)
    }


    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (!tileData || !gameController) return

            // Stavba budovy
            if (currentBuildType !== -1) {
                gameController.buyBuilding(currentBuildType, tileData.x, tileData.y)
                return
            }

            // Nákup jednotky
            if (currentBuyType !== -1) {
                gameController.buyUnit(currentBuyType, tileData.x, tileData.y)
                return
            }

            // vyber/pohyb/utok jednotky
            if (gameController.hasSelectedUnit) {
                if (tileData.hasBuilding) {
                    if (tileData.buildingOwner !== gameController.currentPlayerIndex) {
                        // útok na budovu
                        gameController.attackWithSelectedUnit(tileData.x, tileData.y)
                        return
                    }
                }

                // Pak zkontrolovat jednotku
                if (tileData.hasUnit) {
                    if (tileData.unitOwner === gameController.currentPlayerIndex) {
                        var currentSel = gameController.getSelectedUnitInfo()
                        if (currentSel.x === tileData.x && currentSel.y === tileData.y) {
                            gameController.deselectUnit()
                        } else {
                            gameController.selectUnit(tileData.x, tileData.y)
                        }
                    } else {
                        gameController.attackWithSelectedUnit(tileData.x, tileData.y)
                    }
                } else {
                    // Pohyb na prázdné políčko
                    gameController.moveSelectedUnit(tileData.x, tileData.y)
                }
            } else {
                gameController.selectUnit(tileData.x, tileData.y)
            }
        }
    }

    function isValidMove(x,y){
        if (!gameController || !gameController.hasSelectedUnit) return false
        var moves = gameController.getValidMoves()
        for (var i = 0; i < moves.length; i++){
            if (moves[i].x === x && moves[i].y === y) return true
        }
        return false
    }

    function isValidAttack(x,y){
        if (!gameController || !gameController.hasSelectedUnit) return false
        var attacks = gameController.getValidAttacks()
        for (var i = 0; i < attacks.length; i++){
            if (attacks[i].x === x && attacks[i].y === y) return true
        }
        return false
    }
}
