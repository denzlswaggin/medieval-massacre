import QtQuick
import QtQuick.Controls
import QtMultimedia

ApplicationWindow {
    id: root
    visible: true
    width: 1400
    height: 1100
    title: "Medieval Massacre"

    MediaPlayer{
        id: backgroundMusic
        source: "qrc:/Projekt_CPP/sounds/soundtrack.wav"
        loops: MediaPlayer.Infinite
        audioOutput: AudioOutput{ volume: 0.01 }
    }

    Component.onCompleted: backgroundMusic.play()

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: MainMenu {
            navStack: stackView
        }
    }
}
