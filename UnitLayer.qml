import QtQuick

Item {
    id: rootLayer
    anchors.fill: parent

    Repeater {
        model: gameController.unitModel
        delegate: unitDelegate
    }

    Component {
        id: unitDelegate
        Item {
            id: unitItem


            property bool isBlocking: false
            property int tileSize: 55
            property var myUnitId: model.unitId

            Timer {
                id: blockTimer
                interval: 1000
                onTriggered: unitItem.isBlocking = false
            }

            Connections {
                target: gameController
                function onBlockAnimationTriggered(blockedUnitId) {

                    if (blockedUnitId === unitItem.myUnitId) {
                        unitItem.isBlocking = true;
                        blockTimer.restart();
                    }
                }
            }


            Component.onCompleted: {
                x = model.unitX * tileSize
                y = model.unitY * tileSize
                ready = true
            }
            property int targetX: model.unitX * tileSize
            property int targetY: model.unitY * tileSize
            property bool ready: false


            Behavior on x { enabled: unitItem.ready; NumberAnimation { duration: 400; easing.type: Easing.InOutQuad } }
            Behavior on y { enabled: unitItem.ready; NumberAnimation { duration: 400; easing.type: Easing.InOutQuad } }

            x: targetX; y: targetY
            z: Math.floor(y + height) + 10


            property bool isMoving: false
            onXChanged: { if(ready) { isMoving = true; stopRunTimer.restart() } }
            onYChanged: { if(ready) { isMoving = true; stopRunTimer.restart() } }
            Timer { id: stopRunTimer; interval: 420; onTriggered: unitItem.isMoving = false }

            property bool isAttacking: false
            property string attackDirectionSuffix: "Side" // Pro Cavalry (Front, Back, Side...)
            property string warriorVariant: "1" // Pro Warriora (1 nebo 2)


            property int attackFrameIndex: 0
            NumberAnimation {
                id: attackAnimPlayer
                target: unitItem
                property: "attackFrameIndex"
                from: 0
                to: spriteContainer.attackTotalFrames
                duration: spriteContainer.attackDuration // Rychlost animace
                running: false
                onFinished: {
                    unitItem.isAttacking = false;
                    unitItem.attackFrameIndex = 0;
                }
            }

            Connections {
                target: gameController
                function onAttackAnimationTriggered(attackerId, tX, tY) {
                    if (attackerId === unitItem.myUnitId) {
                        startAttack(tX, tY);
                    }
                }
            }

            function startAttack(targetGridX, targetGridY) {
                // 1. Nahodny attack pro warriora
                if (model.unitType === 0) {
                    warriorVariant = (Math.random() > 0.5) ? "1" : "2"
                }

                // 2. Vypocet smeru pro cav
                if (model.unitType === 2) {
                    var dx = targetGridX - model.unitX;
                    var dy = targetGridY - model.unitY;

                    // Logika smÄ›rÅ¯ (Front, Back, Side, FrontSide, BackSide)
                    if (dy > 0 && dx === 0) attackDirectionSuffix = "Front";      // Dolu
                    else if (dy < 0 && dx === 0) attackDirectionSuffix = "Back";  // Nahoru
                    else if (dy === 0) attackDirectionSuffix = "Side";            // Do boku
                    else if (dy > 0) attackDirectionSuffix = "FrontSide";         //
                    else if (dy < 0) attackDirectionSuffix = "BackSide";          //
                }

                isAttacking = true;
                attackAnimPlayer.start();
            }

            // --- VIZUÃL ---
            Item {
                id: spriteContainer
                anchors.centerIn: parent

                anchors.verticalCenterOffset: (model.unitType === 2) ? 22 : 22
                anchors.horizontalCenterOffset: 23
                width: (model.unitType === 2) ? 192 : 100
                height: (model.unitType === 2) ? 192 : 100
                clip: true

                property string colorSuffix: (model.unitOwner === 0) ? "Red" : "Blue"
                property string unitNameStr: {
                    if (model.unitType === 0) return "Warrior";
                    if (model.unitType === 1) return "Archer";
                    if (model.unitType === 2) return "Cavalry";
                    if (model.unitType === 3) return "Bard";
                    return "Warrior";
                }

                property int attackTotalFrames: {
                    if (model.unitType === 0) return 4;
                    if (model.unitType === 1) return 8;
                    if (model.unitType === 2) return 3;
                    return 1;
                }

                property int attackDuration: {
                    if (model.unitType === 0) return 400;
                    if (model.unitType === 1) return 500;
                    if (model.unitType === 2) return 200;
                    return 400;
                }

                // ObrÃ¡zky: run
                Image {
                    source: "qrc:/Projekt_CPP/images_units/" + parent.unitNameStr + "_" + parent.colorSuffix + "_Run.png"
                    visible: unitItem.isMoving && !unitItem.isAttacking

                    property int frameCount: (model.unitType === 1 || model.unitType === 3) ? 4 : 6
                    property int currentFrame: (gameScreenRoot ? gameScreenRoot.globalUnitFrame : 0) % frameCount
                    x: -(currentFrame * parent.width); width: parent.width * frameCount; height: parent.height
                    fillMode: Image.PreserveAspectFit; smooth: false
                }

                // ObrÃ¡zky: idle
                Image {
                    source: "qrc:/Projekt_CPP/images_units/" + parent.unitNameStr + "_" + parent.colorSuffix + "_Idle.png"
                    visible: !unitItem.isMoving && !unitItem.isAttacking && !unitItem.isBlocking

                    property int frameCount: {
                         if (model.unitType === 0) return 8;
                         if (model.unitType === 2) return 12;
                         return 6;
                    }
                    property int currentFrame: (gameScreenRoot ? gameScreenRoot.globalUnitFrame : 0) % frameCount
                    x: -(currentFrame * parent.width); width: parent.width * frameCount; height: parent.height
                    fillMode: Image.PreserveAspectFit; smooth: false
                }

                // 3. obrazek utok
                Image {
                    id: attackSprite
                    visible: unitItem.isAttacking


                    source: {
                        var base = "qrc:/Projekt_CPP/images_units/" + parent.unitNameStr + "_" + parent.colorSuffix + "_Attack";

                        // Warrior: Attack1 nebo Attack2
                        if (model.unitType === 0) return base + unitItem.warriorVariant + ".png";

                        // Cavalry: Attack_Front, Attack_Side, atd.
                        if (model.unitType === 2) return base + "_" + unitItem.attackDirectionSuffix + ".png";

                        // Archer: Attack
                        return base + ".png";
                    }

                    x: -(Math.floor(unitItem.attackFrameIndex) * parent.width)

                    // Å Ã­Å™ka pÃ¡su
                    width: parent.width * parent.attackTotalFrames
                    height: parent.height
                    fillMode: Image.PreserveAspectFit; smooth: false
                }

                //obrazek bloku pro swordsman
                Image {
                        id: guardSprite
                        source: "qrc:/Projekt_CPP/images_units/" + parent.unitNameStr + "_" + parent.colorSuffix + "_Guard.png"

                        // viditelny jen kdyz blokuje
                        visible: unitItem.isBlocking

                        // animace , guard ma 6 frames
                        property int frameCount: 6
                        property int currentFrame: (gameScreenRoot ? gameScreenRoot.globalUnitFrame : 0) % frameCount

                        x: -(currentFrame * parent.width)
                        width: parent.width * frameCount
                        height: parent.height
                        fillMode: Image.PreserveAspectFit
                        smooth: false
                    }


                // zrcadleni
                transform: Scale {
                    origin.x: spriteContainer.width / 2
                    xScale: (model.unitOwner === 1) ? -1 : 1
                }


                Rectangle {
                    id: healthBarContainer


                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: spriteContainer.top
                    anchors.bottomMargin: -3

                    width: 40
                    height: 5
                    color: "#333333"
                    border.color: "black"
                    border.width: 1
                    radius: 2


                    Rectangle {
                        height: parent.height - 2
                        width: (parent.width - 2) * (model.unitHealth / model.unitMaxHealth)
                        x: 1; y: 1
                        radius: 2


                        color: {
                            var pct = model.unitHealth / model.unitMaxHealth;
                            if (pct > 0.6) return "#2ecc71";
                            if (pct > 0.3) return "#f1c40f";
                            return "#e74c3c";
                        }

                        Behavior on width { NumberAnimation { duration: 200 } }
                        Behavior on color { ColorAnimation { duration: 200 } }
                    }
                }
            }
        }
    }

    
    // Custom pixel font (sdileny s GameScreen)
    FontLoader {
        id: pixelFontUnit
        source: "qrc:/Projekt_CPP/fonts/pixelated.ttf"
    }

    // Model pro floating texty
    ListModel {
        id: floatingTextModel
    }

    // Funkce pro pridani floating textu
    function addFloatingText(gridX, gridY, text, color) {
        var tileSize = 55;
        floatingTextModel.append({
            "posX": gridX * tileSize + tileSize / 2,
            "posY": gridY * tileSize,
            "displayText": text,
            "textColor": color
        });
    }

    // Repeater pro floating texty
    Repeater {
        model: floatingTextModel
        delegate: Item {
            id: floatingItem
            x: model.posX
            y: model.posY
            z: 1000
            
            property real startY: model.posY
            property real opacity_anim: 1.0

            Text {
                id: floatingText
                anchors.centerIn: parent
                text: model.displayText
                font.family: pixelFontUnit.name
                font.pixelSize: 20
                font.bold: true
                color: model.textColor
                opacity: floatingItem.opacity_anim
                
                // Outline/shadow efekt
                style: Text.Outline
                styleColor: "#000000"
            }

            // Animace nahoru
            NumberAnimation {
                id: moveUpAnim
                target: floatingItem
                property: "y"
                from: floatingItem.startY
                to: floatingItem.startY - 60
                duration: 1500
                easing.type: Easing.OutQuad
                running: true
            }

            // Animace fade out
            NumberAnimation {
                id: fadeAnim
                target: floatingItem
                property: "opacity_anim"
                from: 1.0
                to: 0.0
                duration: 1500
                running: true
                onFinished: {
                    // Odstranime z modelu po dokonceni animace
                    for (var i = 0; i < floatingTextModel.count; i++) {
                        if (floatingTextModel.get(i).posX === model.posX && 
                            floatingTextModel.get(i).posY === model.posY &&
                            floatingTextModel.get(i).displayText === model.displayText) {
                            floatingTextModel.remove(i);
                            break;
                        }
                    }
                }
            }
        }
    }

    // Connections pro signaly z GameController
    Connections {
        target: gameController
        
        function onFloatingDamageTriggered(gridX, gridY, damage) {
            addFloatingText(gridX, gridY, "-" + damage, "#ff4444");
        }
        
        function onFloatingHealTriggered(gridX, gridY, healAmount) {
            addFloatingText(gridX, gridY, "+" + healAmount, "#44ff44");
        }
        
        function onFloatingBlockTriggered(gridX, gridY) {
            addFloatingText(gridX, gridY, "BLOCK!", "#9b59b6");
        }
    }
}
