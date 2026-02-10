import QtQuick

Item {
    id: buildingLayer
    anchors.fill: parent

    // Custom pixel font
    FontLoader {
        id: pixelFontBuilding
        source: "qrc:/Projekt_CPP/fonts/pixelated.ttf"
    }

    Repeater {
        model: gameController.buildingModel
        delegate: buildingDelegate
    }

    Component {
        id: buildingDelegate
        Item {
            id: buildingItem

            property int tileSize: 55
            property var myBuildingId: model.buildingId

            x: model.buildingX * tileSize
            y: model.buildingY * tileSize
            z: 10 + model.buildingY  // Budovy nize na mape jsou nad budovami vyse

            width: tileSize
            height: tileSize

            Item {
                id: spriteContainer
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0  // Budova stoji na tile, presahuje nahoru
                width: 80
                height: 80
                clip: false

                property string colorSuffix: (model.buildingOwner === 0) ? "Red" : "Blue"
                property string buildingNameStr: {
                    if (model.buildingType === 0) return "Castle";
                    if (model.buildingType === 1) return "Barracks";
                    if (model.buildingType === 2) return "Church";
                    return "Castle";
                }

                // obrazky budov
                Image {
                    id: buildingSprite
                    source: "qrc:/Projekt_CPP/images_buildings/" + parent.buildingNameStr + "_" + parent.colorSuffix + ".png"
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                    smooth: false
                }
                
                // Health bar primo nad budovou
                Rectangle {
                    id: healthBarContainer
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.top
                    anchors.bottomMargin: -5
                    
                    width: 50
                    height: 6
                    color: "#333333"
                    border.color: "black"
                    border.width: 1
                    radius: 2
                    z: 200

                    Rectangle {
                        height: parent.height - 2
                        width: Math.max(1, (parent.width - 2) * (model.buildingHealth / model.buildingMaxHealth))
                        x: 1
                        y: 1
                        radius: 2

                        color: {
                            var pct = model.buildingHealth / model.buildingMaxHealth;
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
}
