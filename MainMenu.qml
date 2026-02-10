import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: mainMenuRoot
    color: "#1a1a2e"

    required property StackView navStack

    // Custom pixel font
    FontLoader {
        id: pixelFont
        source: "qrc:/Projekt_CPP/fonts/pixelated.ttf"
    }

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 30

        Text {
            text: "Medieval Massacre"
            font.family: pixelFont.name
            font.pixelSize: 60
            font.bold: true
            color: "#eee"
            Layout.alignment: Qt.AlignHCenter
        }

        Text {
            text: "somewhat an RTS game"
            font.family: pixelFont.name
            font.pixelSize: 45
            color: "#aaa"
            Layout.alignment: Qt.AlignHCenter
        }

        Button {
            text: "2 Players"
            font.family: pixelFont.name
            Layout.preferredWidth: 300
            Layout.preferredHeight: 50
            Layout.alignment: Qt.AlignHCenter

            onClicked: {
                gameController.startNewGame(2)
                navStack.push("GameScreen.qml")
            }
        }
    }
}
