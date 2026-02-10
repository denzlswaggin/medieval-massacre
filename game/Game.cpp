//
// Created by kubdv on 12.12.2025.
//

#include "Game.h"
#include "../units/Swordsman.h"
#include "../units/Archer.h"
#include "../units/Cavalry.h"
#include "../units/Bard.h"
#include "../units/UnitType.h"
#include "../buildings/Castle.h"
#include "../buildings/Barracks.h"
#include "../buildings/Church.h"
#include <iostream>

Game::Game()
    : map(nullptr),
    turnManager(nullptr),
    state(GameState::MENU),
    selectedUnit(nullptr),
    castlesPlaced(0){
}

Game::~Game() {

    for (Unit* unit : allUnits) {
        delete unit;
    }
    allUnits.clear();


    for (Player* player : players) {
        delete player;
    }
    players.clear();

    delete map;
    delete turnManager;
}

void Game::initializeGame(int playerCount, int mapWidth, int mapHeight) {
    map = new Map(mapWidth, mapHeight);
    map->generateRandomMap();

    for (int i = 0; i < playerCount; ++i) {
        Player* player = new Player("Player " + std::to_string(i + 1), i, 300);
        players.push_back(player);
    }

    turnManager = new TurnManager();
    turnManager->setGame(this);  // ✅ KRITICKÉ!

    for (Player* player : players) {
        turnManager->addPlayer(player);
    }

    state = GameState::SETUP;
    castlesPlaced = 0;  // ✅ INICIALIZACE

    std::cout << "Game initialized with " << playerCount << " players.\n";
}

void Game::startGame() {
    if (state != GameState::SETUP) {
        std::cout << "Game must be initialized first!\n";
        return;
    }

    turnManager->startGame();
    state = GameState::CASTLE_PLACEMENT;  // ✅ Začínáme umístěním Castlů
    castlesPlaced = 0;

    std::cout << "Game started!\n";
    std::cout << "=== CASTLE PLACEMENT PHASE ===\n";
    std::cout << "Each player must place their Castle first!\n";
}

bool Game::selectUnit(int x, int y) {
    if (state != GameState::PLAYING) return false;

    Unit* unit = map->getUnitAt(x, y);


    if (unit && unit->getOwner() == getCurrentPlayer() && !unit->isDead()) {
        selectedUnit = unit;
        std::cout << "Selected: " << unit->getName()
                  << " at (" << x << ", " << y << ")\n";
        return true;
    }

    return false;
}

bool Game::moveSelectedUnit(int x, int y) {
    if (!selectedUnit || state != GameState::PLAYING) {
        return false;
    }

    if (!isValidMove(selectedUnit, x, y)) {
        std::cout << "Invalid move!\n";
        return false;
    }


    map->removeUnit(selectedUnit);


    selectedUnit->move(Position(x, y));


    map->placeUnit(selectedUnit, x, y);

    std::cout << selectedUnit->getName() << " moved to (" << x << ", " << y << ")\n";
    return true;


}

bool Game::attackWithSelectedUnit(int x, int y) {
    if (!selectedUnit || state != GameState::PLAYING) {
        return false;
    }

    Unit* target = map->getUnitAt(x, y);

    if (target && isValidAttack(selectedUnit, x, y)) {
        selectedUnit->attack(target);

        std::cout << selectedUnit->getName() << " attacks " << target->getName() << "!\n";

        if (target->isDead()) {
            std::cout << target->getName() << " has been killed!\n";
            map->removeUnit(target);
        }

        update();
        return true;
    }

    Building* buildingTarget = map->getBuildingAt(x, y);
    if (buildingTarget && buildingTarget->getOwner() != selectedUnit->getOwner()) {
        return attackBuilding(x, y);
    }

    std::cout << "Invalid attack!\n";
    return false;
}

bool Game::attackBuilding(int x, int y) {
    if (!selectedUnit || state != GameState::PLAYING) {
        return false;
    }

    Building* target = map->getBuildingAt(x, y);
    if (!target || target->isDestroyed()) return false;
    if (target->getOwner() == selectedUnit->getOwner()) return false;

    // Kontrola dosahu
    Position unitPos = selectedUnit->getPosition();
    int distance = std::abs(x - unitPos.x) + std::abs(y - unitPos.y);
    if (distance > selectedUnit->getAttackRange()) {
        std::cout << "Building too far away!\n";
        return false;
    }

    target->takeDamage(selectedUnit->getDamage());

    std::cout << selectedUnit->getName() << " attacks " << target->getName() << "!\n";

    if (target->isDestroyed()) {
        std::cout << target->getName() << " has been destroyed!\n";
        map->removeBuilding(target);
    }

    selectedUnit->resetTurn();  // nemuze utocit znovu

    update();
    return true;
}

void Game::deselectUnit() {
    selectedUnit = nullptr;
}

bool Game::buyUnit(UnitType type, int x, int y) {
    if (state != GameState::PLAYING) return false;

    Player* player = getCurrentPlayer();
    int cost = getUnitCost(type);


    if (!player->canAffordUnit(cost)) {
        std::cout << "Not enough gold!\n";
        return false;
    }


    if (!map->canPlaceUnitAt(x, y)) {
        std::cout << "Cannot place unit there!\n";
        return false;
    }

    // Nelze kupovat na horách - na hory se vstupuje pouze přes schody
    Tile* tile = map->getTile(x, y);
    if (tile && tile->getTerrain() == TerrainType::MOUNTAIN) {
        std::cout << "Cannot buy units on mountains!\n";
        return false;
    }

    // Nelze kupovat pod útesem
    if (map->isValidPosition(x, y - 1)) {
        if (map->getTile(x, y - 1)->getTerrain() == TerrainType::MOUNTAIN) {
            std::cout << "Cannot buy units under cliffs!\n";
            return false;
        }
    }

    int mapWidth = map->getWidth();
    int splitLine = mapWidth / 2;
    int playerId = player->getId();

    if (playerId == 0 && x >= splitLine) {
        std::cout << "Player 1 can only build on the LEFT side!\n";
        return false;
    }

    if (playerId == 1 && x < splitLine) {
        std::cout << "Player 2 can only build on the RIGHT side!\n";
        return false;
    }

    Unit* unit = createUnit(type);
    if (!unit) return false;

    if (player->spendGold(cost)) {
        player->addUnit(unit);
        allUnits.push_back(unit);
        map->placeUnit(unit, x, y);

        std::cout << player->getName() << " bought " << unit->getName() << "!\n";
        return true;
    }

    delete unit;
    return false;
}

int Game::getEffectiveUnitCost(UnitType type) const {
    int baseCost = getUnitCost(type);
    Player* player = getCurrentPlayer();

    if (player && player->hasBarracks()) {
        int discount = player->getBarracksDiscount();
        return baseCost - (baseCost * discount / 100);
    }

    return baseCost;
}

bool Game::buyBuilding(BuildingType type, int x, int y) {
    if (state != GameState::PLAYING && state != GameState::CASTLE_PLACEMENT) {
        return false;
    }

    Player* player = getCurrentPlayer();
    int cost = getBuildingCost(type);

    if (state == GameState::CASTLE_PLACEMENT) {
        if (type != BuildingType::CASTLE) {
            std::cout << "You must place your Castle first!\n";
            return false;
        }

        cost = 0;
    } else {
        if (type == BuildingType::CASTLE) {
            if (player->getCastleCount() >= 1) {
                std::cout << "You can only have ONE Castle!\n";
                return false;
            }
        }
    }

    if (!player->canAffordUnit(cost)) {
        std::cout << "Not enough gold!\n";
        return false;
    }

    if (!map->canPlaceBuildingAt(x, y)) {
        std::cout << "Cannot place building there!\n";
        return false;
    }

    int mapWidth = map->getWidth();
    int splitLine = mapWidth / 2;
    int playerId = player->getId();

    if (playerId == 0 && x >= splitLine) {
        std::cout << "Player 1 can only build on the LEFT side!\n";
        return false;
    }

    if (playerId == 1 && x < splitLine) {
        std::cout << "Player 2 can only build on the RIGHT side!\n";
        return false;
    }

    Building* building = createBuilding(type);
    if (!building) return false;

    if (player->spendGold(cost)) {
        player->addBuilding(building);
        allBuildings.push_back(building);
        map->placeBuilding(building, x, y);

        std::cout << player->getName() << " built " << building->getName() << "!\n";

        if (state == GameState::CASTLE_PLACEMENT && type == BuildingType::CASTLE) {
            castlesPlaced++;
            std::cout << "Castles placed: " << castlesPlaced << "/2\n";

            if (castlesPlaced >= 2) {
                state = GameState::PLAYING;
                std::cout << "\n=== ALL CASTLES PLACED - GAME BEGINS! ===\n";
            }
        }

        return true;
    }

    delete building;
    return false;
}

int Game::getUnitCost(UnitType type) const {
    switch (type) {
    case UnitType::SWORDSMAN: return 50;
    case UnitType::ARCHER: return 60;
    case UnitType::CAVALRY: return 80;
    case UnitType::BARD: return 70;
    default: return 0;
    }
}

int Game::getBuildingCost(BuildingType type) const {
    switch (type) {
    case BuildingType::CASTLE: return 150;
    case BuildingType::BARRACKS: return 100;
    case BuildingType::CHURCH: return 120;
    default: return 0;
    }
}

void Game::endTurn() {
    if (state != GameState::PLAYING && state != GameState::CASTLE_PLACEMENT) {
        return;
    }

    deselectUnit();
    turnManager->endCurrentTurn();
    Player* newPlayer = getCurrentPlayer();

    if (state == GameState::PLAYING && newPlayer) {
        processBuildingEffectsForPlayer(newPlayer);
    }

    update();
}

void Game::processBuildingEffectsForPlayer(Player* player) {
    if (!player) return;

    player->processBuildingEffects();

    for (Building* building : allBuildings) {
        if (building && !building->isDestroyed() &&
            building->getOwner() == player &&
            building->getType() == BuildingType::CHURCH) {
            Church* church = dynamic_cast<Church*>(building);
            if (church) {
                church->healNearbyUnits(allUnits);
            }
        }
    }
}

Player* Game::getCurrentPlayer() const {
    if (!turnManager) return nullptr;
    return turnManager->getCurrentPlayer();
}

void Game::update() {

    if (isGameOver()) {
        state = GameState::GAME_OVER;
        Player* winner = getWinner();

        if (winner) {
            std::cout << "\n=== GAME OVER ===\n";
            std::cout << winner->getName() << " wins!\n";
        } else {
            std::cout << "\n=== DRAW ===\n";
        }
    }
}

bool Game::isGameOver() const {
    if (!turnManager) return false;
    return turnManager->isGameOver();
}

Player* Game::getWinner() const {
    if (!turnManager) return nullptr;
    return turnManager->getWinner();
}

bool Game::isValidMove(Unit* unit, int x, int y) const {
    if (!unit || !map) return false;

    Position target(x, y);
    Position unitPos = unit->getPosition();

    if (!unit->canMoveTo(target)) {
        return false;
    }

    if (!map->canPlaceUnitAt(x, y)) {
        return false;
    }

    Tile* targetTile = map->getTile(x, y);
    Tile* startTile = map->getTile(unitPos.x, unitPos.y);

    if (!targetTile || !startTile) return false;

    // nemuzeme se pohybovat na polickach pod horou
    if (map->isValidPosition(x, y - 1)) {
        Tile* aboveTarget = map->getTile(x, y - 1);
        if (aboveTarget && aboveTarget->getTerrain() == TerrainType::MOUNTAIN) {
            if (targetTile->getTerrain() != TerrainType::MOUNTAIN) {
                return false;
            }
        }
    }

    // vstup na horu
    if (targetTile->getTerrain() == TerrainType::MOUNTAIN &&
        startTile->getTerrain() != TerrainType::MOUNTAIN) {

        int stairs = targetTile->getStairsType();

        // leve schody pristup
        if (stairs == 1 && unitPos.x == x - 1 && unitPos.y == y + 1) {
            return true;
        }
        // prave schody pristup
        else if (stairs == 2 && unitPos.x == x + 1 && unitPos.y == y + 1) {
            return true;
        }

        return false;
    }

    // VÝSTUP Z HORY (z hory na ne-horu)
    if (startTile->getTerrain() == TerrainType::MOUNTAIN &&
        targetTile->getTerrain() != TerrainType::MOUNTAIN) {

        int stairs = startTile->getStairsType();

        // Schody vlevo (stairsType=1): výstup na levé dolní políčko
        if (stairs == 1 && x == unitPos.x - 1 && y == unitPos.y + 1) {
            return true;
        }
        // Schody vpravo (stairsType=2): výstup na pravé dolní políčko
        else if (stairs == 2 && x == unitPos.x + 1 && y == unitPos.y + 1) {
            return true;
        }

        return false;
    }

    // pohyb kdyz uz jsme na hore
    if (startTile->getTerrain() == TerrainType::MOUNTAIN &&
        targetTile->getTerrain() == TerrainType::MOUNTAIN) {
        return true;
    }

    return true;
}

bool Game::isValidAttack(Unit* unit, int targetX, int targetY) const {
    if (!unit || !map) return false;

    Unit* target = map->getUnitAt(targetX, targetY);

    if (!target) return false;
    if (target->getOwner() == unit->getOwner()) return false;
    if (target->isDead()) return false;

    return unit->canAttack(target);
}

Unit* Game::createUnit(UnitType type) {
    switch (type) {
    case UnitType::SWORDSMAN: return new Swordsman();
    case UnitType::ARCHER: return new Archer();
    case UnitType::CAVALRY: return new Cavalry();
    case UnitType::BARD: return new Bard();
    default: return nullptr;
    }
}

Building* Game::createBuilding(BuildingType type) {
    switch (type) {
    case BuildingType::CASTLE: return new Castle();
    case BuildingType::BARRACKS: return new Barracks();
    case BuildingType::CHURCH: return new Church();
    default: return nullptr;
    }
}
