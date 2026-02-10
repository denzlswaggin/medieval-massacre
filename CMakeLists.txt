cmake_minimum_required(VERSION 3.16)

project(Projekt_CPP VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Quick Qml Multimedia QuickControls2)

if(Qt6_FOUND)
    message(STATUS "Qt6 found, building Qt version")

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)

    # Vytvoreny backend
    add_executable(Projekt_CPP
        main.cpp
        qmlanimace/UnitModel.cpp
        qmlanimace/UnitModel.h
        qmlanimace/BuildingModel.h
        qmlanimace/BuildingModel.cpp

        map/Tile.cpp
        map/Map.cpp

        units/Unit.cpp
        units/Swordsman.cpp
        units/Archer.cpp
        units/Cavalry.cpp
        units/Bard.cpp
        units/Unit.h
        units/Swordsman.h
        units/Archer.h
        units/Cavalry.h
        units/Bard.h
        units/UnitType.h

        player/Player.cpp
        player/Player.h
        game/TurnManager.cpp
        game/Game.cpp

        map/Tile.h
        map/Map.h

        buildings/Building.cpp
        buildings/Castle.cpp
        buildings/Barracks.cpp
        buildings/Church.cpp
        buildings/Building.h
        buildings/Castle.h
        buildings/Barracks.h
        buildings/Church.h
        buildings/BuildingType.h

        game/TurnManager.h
        game/Game.h

        qmlcontroller/GameController.cpp
        qmlcontroller/GameController.h
        qmlcontroller/MapModel.cpp
        qmlcontroller/MapModel.h

    )

    target_include_directories(Projekt_CPP PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    target_link_libraries(Projekt_CPP PRIVATE
        Qt6::Core
        Qt6::Quick
        Qt6::Qml
        Qt6::Multimedia
        Qt6::QuickControls2
    )

        qt_policy(SET QTP0001 NEW)

    # QML soubory
    qt_add_qml_module(Projekt_CPP
        URI Projekt_CPP
        VERSION 1.0
        QML_FILES
            Main.qml
            MapTile.qml
            MainMenu.qml
            GameScreen.qml
            UnitLayer.qml
            BuildingLayer.qml
            SOURCES



    )
    #soundy a soundtrack
    qt_add_resources(Projekt_CPP "sounds"
    PREFIX "/Projekt_CPP"
    FILES
        sounds/swordsman_buy.wav
        sounds/swordsman_attack.wav
        sounds/archer_buy.wav
        sounds/archer_attack.wav
        sounds/cavalry_buy.wav
        sounds/cavalry_attack.wav
        sounds/bard_buy.wav
        sounds/bard_attack.wav
        sounds/soundtrack.wav
        sounds/Warrior_Shield.wav
        #sounds/heal.wav
        sounds/castle_place.wav
        sounds/barracks_place.wav
        sounds/church_place.wav

    )

    #units
    qt_add_resources(Projekt_CPP "images_units"
    PREFIX "/Projekt_CPP"
    FILES
        #units Idle a Run
        images_units/Warrior_Blue_Idle.png
        images_units/Warrior_Red_Idle.png
        images_units/Warrior_Blue_Run.png
        images_units/Warrior_Red_Run.png

        images_units/Archer_Blue_Idle.png
        images_units/Archer_Blue_Run.png
        images_units/Archer_Red_Idle.png
        images_units/Archer_Red_Run.png

        images_units/Bard_Blue_Idle.png
        images_units/Bard_Blue_Run.png
        images_units/Bard_Red_Idle.png
        images_units/Bard_Red_Run.png

        images_units/Cavalry_Blue_Idle.png
        images_units/Cavalry_Blue_Run.png
        images_units/Cavalry_Red_Idle.png
        images_units/Cavalry_Red_Run.png

        #units attack
        images_units/Warrior_Blue_Attack1.png
        images_units/Warrior_Blue_Attack2.png
        images_units/Warrior_Red_Attack1.png
        images_units/Warrior_Red_Attack2.png

        images_units/Archer_Blue_Attack.png
        images_units/Archer_Red_Attack.png

        images_units/Cavalry_Blue_Attack_Back.png
        images_units/Cavalry_Blue_Attack_BackSide.png
        images_units/Cavalry_Blue_Attack_Front.png
        images_units/Cavalry_Blue_Attack_FrontSide.png
        images_units/Cavalry_Blue_Attack_Side.png

        images_units/Cavalry_Red_Attack_Back.png
        images_units/Cavalry_Red_Attack_BackSide.png
        images_units/Cavalry_Red_Attack_Front.png
        images_units/Cavalry_Red_Attack_FrontSide.png
        images_units/Cavalry_Red_Attack_Side.png

        #Units schopbosti
        images_units/Warrior_Blue_Guard.png
        images_units/Warrior_Red_Guard.png

        images_buildings/Barracks_Blue.png
        images_buildings/Barracks_Red.png
        images_buildings/Church_Red.png
        images_buildings/Church_Blue.png
        images_buildings/Castle_Blue.png
        images_buildings/Castle_Red.png

    )



    #obrázky
    qt_add_resources(Projekt_CPP "images"
    PREFIX "/Projekt_CPP"
    FILES
        #units
        images/swordsman.png
        images/archer.png
        images/cavalry.png
        images/bard.png
        images/mountain.png
        images/water.png

        #grass_cislo pro mapu
        images/grass_3.png
        images/grass_5.png
        images/grass_6.png
        images/grass_7.png
        images/grass_9.png
        images/grass_10.png
        images/grass_11.png
        images/grass_12.png
        images/grass_13.png
        images/grass_14.png
        images/grass_15.png

        #dekorace na travu
        images/rock.png
        images/tree_strip.png
        images/Bushe1.png
        images/Bushe2.png
        images/Bushe3.png
        images/Bushe4.png

        #dekorace na vodu
        images/Clouds_01.png
        images/Clouds_02.png
        images/Clouds_03.png
        images/Clouds_04.png
        images/Clouds_05.png
        images/Clouds_06.png
        images/shadow.png
        images/Water_Rock2.png
        images/Water_Rock1.png
        images/Water_Rock3.png

        #hory
        images/cliff_1.png
        images/cliff_2.png
        images/cliff_3.png
        images/cliff_4.png
        images/cliff_5.png
        images/cliff_6.png
        images/cliff_7.png
        images/cliff_8.png
        images/cliff_9.png
        images/cliff_10.png
        images/cliff_11.png
        images/cliff_12.png
        #schody pro hory
        images/bridge.png
    )

    qt_add_resources(Projekt_CPP "fonts"
    PREFIX "/Projekt_CPP"
    FILES
        fonts/pixelated.ttf
        fonts/pixelated2.ttf
)

    set_target_properties(Projekt_CPP PROPERTIES
        WIN32_EXECUTABLE TRUE
        //MACOSX_BUNDLE TRUE
    )

else()
    message(STATUS "Qt6 not found, building console version")

    add_executable(Projekt_CPP
        main.cpp

        map/Tile.cpp
        map/Map.cpp
        units/Unit.cpp
        units/Swordsman.cpp
        units/Archer.cpp
        units/Cavalry.cpp
        units/Bard.cpp
        player/Player.cpp
        game/TurnManager.cpp
        game/Game.cpp
    )

    target_include_directories(Projekt_CPP PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()
