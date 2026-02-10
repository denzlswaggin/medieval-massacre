#include "GameController.h"
#include <QDebug>
#include <QMap>
#include <cmath>
#include "MapModel.h"
#include "units/Swordsman.h"



GameController::GameController(QObject *parent)
    : QObject(parent), game(nullptr) {
    m_mapModel = new MapModel(this);
    m_unitModel = new UnitModel(this);
    m_buildingModel = new BuildingModel(this);
}

GameController::~GameController() {
    delete game;
}

int GameController::getMapWidth() const {
    return game && game->getMap() ? game->getMap()->getWidth() : 0;
}

int GameController::getMapHeight() const {
    return game && game->getMap() ? game->getMap()->getHeight() : 0;
}

int GameController::getCurrentPlayerIndex() const {
    Player* player = game ? game->getCurrentPlayer() : nullptr;
    return player ? player->getId() : -1;
}

QString GameController::getCurrentPlayerName() const {
    Player* player = game ? game->getCurrentPlayer() : nullptr;
    return player ? QString::fromStdString(player->getName()) : "";
}

int GameController::getCurrentPlayerGold() const {
    Player* player = game ? game->getCurrentPlayer() : nullptr;
    return player ? player->getGold() : 0;
}

int GameController::getCurrentPlayerUnits() const {
    Player* player = game ? game->getCurrentPlayer() : nullptr;
    return player ? player->getLiveUnitCount() : 0;
}

int GameController::getCurrentPlayerBuildings() const {
    Player* player = game ? game->getCurrentPlayer() : nullptr;
    return player ? player->getLiveBuildingCount() : 0;
}

bool GameController::hasSelectedUnit() const {
    return game && game->getSelectedUnit() != nullptr;
}

bool GameController::isGameOver() const {
    return game && game->isGameOver();
}

void GameController::startNewGame(int playerCount) {
    qDebug() << "Starting new game with" << playerCount << "players";

    delete game;

    game = new Game();
    game->initializeGame(playerCount, 32, 24);
    generateIslandMap();
    game->startGame();

    m_mapModel->setGame(game);
    qDebug() << "MapModel rows:" << m_mapModel->rowCount();

    m_mapModel->refreshAll();
    emit turnChanged();
    emit playerStatsChanged();
    emit unitSelectionChanged();
    emit gameOverChanged();

    if (m_unitModel) {
        m_unitModel->setUnits(game->getAllUnits());
    }

    if (m_buildingModel) {
        m_buildingModel->setBuildings(game->getAllBuildings());
    }

    // Herní log zprávy
    emit gameLogMessage("=== NEW GAME STARTED ===");
    emit gameLogMessage(QString("Players: %1").arg(playerCount));
    emit gameLogMessage("Place your Castle to begin!");
}


QVariantMap GameController::getTileData(int x, int y) {
    QVariantMap tileData;

    if (!game || !game->getMap()) {
        return tileData;
    }

    Map* map = game->getMap();
    Tile* tile = map->getTile(x, y);
    Unit* unit = map->getUnitAt(x, y);
    Building* building = map->getBuildingAt(x, y);

    if (!tile) {
        return tileData;
    }

    tileData["x"] = x;
    tileData["y"] = y;
    tileData["terrain"] = static_cast<int>(tile->getTerrain());

    // VÃ½poÄet bitmask pro grafiku
    int bitmask = 0;

    auto isConnected = [&](int tx, int ty) {
        if (!map->isValidPosition(tx, ty)) return false;
        TerrainType neighborType = map->getTile(tx, ty)->getTerrain();

        if (tile->getTerrain() == TerrainType::WATER) {
            return neighborType == TerrainType::WATER;
        }

        return neighborType != TerrainType::WATER;
    };

    if (isConnected(x, y - 1)) bitmask += 1;
    if (isConnected(x + 1, y)) bitmask += 2;
    if (isConnected(x, y + 1)) bitmask += 4;
    if (isConnected(x - 1, y)) bitmask += 8;

    tileData["bitmask"] = bitmask;

    // Maska pro hory
    if (tile->getTerrain() == TerrainType::MOUNTAIN) {
        int mMask = 0;

        auto isM = [&](int tx, int ty) {
            if (!map->isValidPosition(tx, ty)) return false;
            return map->getTile(tx, ty)->getTerrain() == TerrainType::MOUNTAIN;
        };

        if (isM(x, y - 1)) mMask += 1;
        if (isM(x + 1, y)) mMask += 2;
        if (isM(x, y + 1)) mMask += 4;
        if (isM(x - 1, y)) mMask += 8;

        tileData["mountainMask"] = mMask;
        tileData["stairsType"] = tile->getStairsType();
    }

    // Data o jednotce
    if (unit && !unit->isDead()) {
        tileData["hasUnit"] = true;
        tileData["unitType"] = static_cast<int>(unit->getType());
        tileData["unitOwner"] = unit->getOwner()->getId();
        tileData["unitHealth"] = unit->getHealth();
        tileData["unitMaxHealth"] = unit->getMaxHealth();
        tileData["unitName"] = QString::fromStdString(unit->getName());
    } else {
        tileData["hasUnit"] = false;
    }

    // Data o budovÄ›
    if (building && !building->isDestroyed()) {
        tileData["hasBuilding"] = true;
        tileData["buildingType"] = static_cast<int>(building->getType());
        tileData["buildingOwner"] = building->getOwner()->getId();
        tileData["buildingHealth"] = building->getHealth();
        tileData["buildingMaxHealth"] = building->getMaxHealth();
        tileData["buildingName"] = QString::fromStdString(building->getName());
    } else {
        tileData["hasBuilding"] = false;
    }

    return tileData;
}

bool GameController::selectUnit(int x, int y) {
    if (!game) return false;

    bool success = game->selectUnit(x, y);

    if (success) {
        emit unitSelectionChanged();
        qDebug() << "Unit selected at" << x << "," << y;
    } else {
        emit invalidAction("Cannot select that unit!");
    }

    return success;
}

bool GameController::moveSelectedUnit(int x, int y) {
    if (!game) return false;

    bool success = game->moveSelectedUnit(x, y);

    if (success) {
        game->deselectUnit();

        m_mapModel->refreshAll();

        if (m_unitModel) {
            m_unitModel->refresh();
        }

        emit playerStatsChanged();
        emit unitSelectionChanged();
        qDebug() << "Unit moved to" << x << "," << y;

        endTurn();
    } else {
        emit invalidAction("Cannot move there!");
    }

    return success;
}

bool GameController::attackWithSelectedUnit(int x, int y) {
    if (!game || !game->getSelectedUnit()) return false;

    Unit* target = nullptr;
    for (Unit* u : game->getAllUnits()) {
        if (u->getPosition().x == x && u->getPosition().y == y && !u->isDead()) {
            target = u;
            break;
        }
    }

    if (target) {
        Swordsman* defender = dynamic_cast<Swordsman*>(target);

        if (defender && defender->tryAutoBlock()) {
            qDebug() << "Attack BLOCKED by Swordsman!";
            emit blockAnimationTriggered((qulonglong)defender);
            Unit* attacker = game->getSelectedUnit();
            emit attackAnimationTriggered((qulonglong)attacker, x, y);
            emit floatingBlockTriggered(x, y);
            emit gameLogMessage(QString("%1's attack was BLOCKED!").arg(QString::fromStdString(attacker->getName())));
            game->getSelectedUnit()->resetTurn();
            endTurn();
            return true;
        }
    }

    Position attackerPos = game->getSelectedUnit()->getPosition();
    Unit* attacker = game->getSelectedUnit();
    int attackerType = static_cast<int>(attacker->getType());
    int attackerDamage = attacker->getDamage();

    bool success = game->attackWithSelectedUnit(x, y);

    if (success) {
        emit combatOccurred(attackerPos.x, attackerPos.y, x, y);

        m_mapModel->refreshAll();
        m_unitModel->refresh();
        m_unitModel->setUnits(game->getAllUnits());


        m_buildingModel->setBuildings(game->getAllBuildings());
        m_buildingModel->refresh();

        emit playerStatsChanged();
        emit unitAttacked(attackerType);
        emit damageDealt(x, y, attackerDamage);
        emit floatingDamageTriggered(x, y, attackerDamage);

        emit attackAnimationTriggered((qulonglong)attacker, x, y);

        // Log zprávy
        QString attackerName = QString::fromStdString(attacker->getName());
        Unit* targetAfter = game->getMap()->getUnitAt(x, y);
        if (targetAfter && !targetAfter->isDead()) {
            emit gameLogMessage(QString("%1 deals %2 damage!").arg(attackerName).arg(attackerDamage));
        } else {
            emit gameLogMessage(QString("%1 deals %2 damage - KILL!").arg(attackerName).arg(attackerDamage));
        }

        if (game->isGameOver()) {
            emit gameOverChanged();
            emit gameLogMessage("=== GAME OVER ===");
        }

        qDebug() << "Attack executed, damage: " << attackerDamage;
        endTurn();

    } else {
        emit invalidAction("Cannot attack that target!");
    }

    return success;
}

void GameController::deselectUnit() {
    if (game) {
        game->deselectUnit();
        emit unitSelectionChanged();
    }
}

bool GameController::buyUnit(int unitType, int x, int y) {
    if (!game) return false;

    UnitType type = static_cast<UnitType>(unitType);
    bool success = game->buyUnit(type, x, y);

    if (success) {
        m_mapModel->refreshAll();

        if (m_unitModel) {
            m_unitModel->setUnits(game->getAllUnits());
            m_unitModel->refresh();
        }

        emit playerStatsChanged();
        emit unitBought(unitType);

        // Log zpráva
        QString unitName;
        switch(unitType) {
        case 0: unitName = "Swordsman"; break;
        case 1: unitName = "Archer"; break;
        case 2: unitName = "Cavalry"; break;
        case 3: unitName = "Bard"; break;
        default: unitName = "Unit"; break;
        }
        emit gameLogMessage(QString("%1 recruited %2").arg(getCurrentPlayerName()).arg(unitName));

        qDebug() << "Unit bought at" << x << "," << y;
        endTurn();
    } else {
        emit invalidAction("Cannot buy unit!");
    }

    return success;
}

bool GameController::buyBuilding(int buildingType, int x, int y) {
    if (!game) return false;

    BuildingType type = static_cast<BuildingType>(buildingType);
    bool success = game->buyBuilding(type, x, y);

    if (success) {
        m_mapModel->refreshAll();

        if (m_buildingModel) {
            m_buildingModel->setBuildings(game->getAllBuildings());
            m_buildingModel->refresh();
        }

        emit playerStatsChanged();
        emit buildingBought(buildingType);

        // Log zpráva
        QString buildingName;
        switch(buildingType) {
        case 0: buildingName = "Castle"; break;
        case 1: buildingName = "Barracks"; break;
        case 2: buildingName = "Church"; break;
        default: buildingName = "Building"; break;
        }
        emit gameLogMessage(QString("%1 built %2").arg(getCurrentPlayerName()).arg(buildingName));

        qDebug() << "Building bought at" << x << "," << y;
        endTurn();
    } else {
        emit invalidAction("Cannot buy building!");
    }

    return success;
}

void GameController::endTurn() {
    if (!game) return;

    if (game->isGameOver()) {
        emit gameOverChanged();
        qDebug() << "Game is already over!";
        return;
    }

    qDebug() << "Ending turn for player" << game->getCurrentPlayer()->getId();

    // Uložíme health jednotek před koncem tahu pro detekci healingu
    QMap<qulonglong, int> healthBefore;
    for (Unit* u : game->getAllUnits()) {
        if (u && !u->isDead()) {
            healthBefore[(qulonglong)u] = u->getHealth();
        }
    }

    game->endTurn();
    qDebug() << "New turn for player" << game->getCurrentPlayer()->getId();

    emit turnChanged();
    emit playerStatsChanged();
    emit unitSelectionChanged();
    m_mapModel->refreshAll();

    // Detekujeme healing od Church a emitujeme floating heal
    for (Unit* u : game->getAllUnits()) {
        if (u && !u->isDead()) {
            qulonglong unitId = (qulonglong)u;
            if (healthBefore.contains(unitId)) {
                int healAmount = u->getHealth() - healthBefore[unitId];
                if (healAmount > 0) {
                    Position pos = u->getPosition();
                    emit floatingHealTriggered(pos.x, pos.y, healAmount);
                    emit gameLogMessage(QString("%1 healed for %2 HP").arg(QString::fromStdString(u->getName())).arg(healAmount));
                }
            }
        }
    }

    if (m_unitModel) {
        m_unitModel->refresh();
    }

    if (game->isGameOver()) {
        emit gameOverChanged();
        emit gameLogMessage("=== GAME OVER ===");
        Player* winner = game->getWinner();
        if (winner) {
            emit gameLogMessage(QString("%1 WINS!").arg(QString::fromStdString(winner->getName())));
        }
        qDebug() << "Game ended!";
    } else {
        emit gameLogMessage(QString("--- %1's Turn ---").arg(getCurrentPlayerName()));
    }

    qDebug() << "Turn ended";
}

QVariantMap GameController::getSelectedUnitInfo() {
    QVariantMap info;

    if (!game || !game->getSelectedUnit()) {
        info["hasUnit"] = false;
        return info;
    }

    Unit* unit = game->getSelectedUnit();
    Position pos = unit->getPosition();

    info["hasUnit"] = true;
    info["name"] = QString::fromStdString(unit->getName());
    info["health"] = unit->getHealth();
    info["maxHealth"] = unit->getMaxHealth();
    info["damage"] = unit->getDamage();
    info["movementRange"] = unit->getMovementRange();
    info["attackRange"] = unit->getAttackRange();
    info["x"] = pos.x;
    info["y"] = pos.y;

    return info;
}

int GameController::getUnitCost(int unitType) {
    if (!game) return 0;
    return game->getUnitCost(static_cast<UnitType>(unitType));
}

int GameController::getEffectiveUnitCost(int unitType) {
    if (!game) return 0;
    return game->getEffectiveUnitCost(static_cast<UnitType>(unitType));
}

int GameController::getBuildingCost(int buildingType) {
    if (!game) return 0;
    return game->getBuildingCost(static_cast<BuildingType>(buildingType));
}

QVariantList GameController::getValidMoves() {
    QVariantList moves;

    if (!game || !game->getSelectedUnit() || !game->getMap()) {
        return moves;
    }

    Unit* unit = game->getSelectedUnit();
    Map* map = game->getMap();
    Position unitPos = unit->getPosition();
    int range = unit->getMovementRange();

    for (int y = 0; y < map->getHeight(); ++y) {
        for (int x = 0; x < map->getWidth(); ++x) {

            if (x == unitPos.x && y == unitPos.y) continue;

            int distance = abs(x - unitPos.x) + abs(y - unitPos.y);

            if (distance <= range && isPosInValidMoves(x, y)) {
                QVariantMap move;
                move["x"] = x;
                move["y"] = y;
                moves.append(move);
            }
        }
    }

    return moves;
}

QVariantList GameController::getValidAttacks() {
    QVariantList attacks;

    if (!game || !game->getSelectedUnit() || !game->getMap()) {
        return attacks;
    }

    Unit* unit = game->getSelectedUnit();
    Map* map = game->getMap();
    Position unitPos = unit->getPosition();

    for (int y = 0; y < map->getHeight(); ++y) {
        for (int x = 0; x < map->getWidth(); ++x) {
            Unit* target = map->getUnitAt(x, y);

            if (!target || !unit->canAttack(target)) {
                continue;
            }

            if (target->getOwner() != unit->getOwner() && !target->isDead()) {
                QVariantMap attack;
                attack["x"] = x;
                attack["y"] = y;
                attacks.append(attack);
            }
        }
    }

    return attacks;
}

bool GameController::isPosInValidMoves(int x, int y) {

    if (!game || !game->getSelectedUnit() || !game->getMap()) {
        return false;
    }

    Unit* unit = game->getSelectedUnit();
    Position unitPos = unit->getPosition();

    if (x == unitPos.x && y == unitPos.y) return false;

    int distance = std::abs(x - unitPos.x) + std::abs(y - unitPos.y);
    int range = unit->getMovementRange();
    if (distance > range) return false;

    Map* map = game->getMap();
    Tile* targetTile = map->getTile(x, y);
    Tile* startTile = map->getTile(unitPos.x, unitPos.y);

    if (!targetTile || !startTile) return false;

    if (map->isValidPosition(x, y - 1)) {
        Tile* aboveTarget = map->getTile(x, y - 1);
        if (aboveTarget && aboveTarget->getTerrain() == TerrainType::MOUNTAIN) {
            if (targetTile->getTerrain() != TerrainType::MOUNTAIN) {
                return false;
            }
        }
    }

    if (targetTile->getTerrain() == TerrainType::MOUNTAIN &&
        startTile->getTerrain() != TerrainType::MOUNTAIN) {

        int stairs = targetTile->getStairsType();

        if (stairs == 1 && unitPos.x == x - 1 && unitPos.y == y + 1) {
            return map->canPlaceUnitAt(x, y);
        }
        else if (stairs == 2 && unitPos.x == x + 1 && unitPos.y == y + 1) {
            return map->canPlaceUnitAt(x, y);
        }

        return false;
    }

    if (startTile->getTerrain() == TerrainType::MOUNTAIN &&
        targetTile->getTerrain() != TerrainType::MOUNTAIN) {

        int stairs = startTile->getStairsType();

        if (stairs == 1 && x == unitPos.x - 1 && y == unitPos.y + 1) {
            return map->canPlaceUnitAt(x, y);
        }
        else if (stairs == 2 && x == unitPos.x + 1 && y == unitPos.y + 1) {
            return map->canPlaceUnitAt(x, y);
        }

        return false;
    }

    if (startTile->getTerrain() == TerrainType::MOUNTAIN &&
        targetTile->getTerrain() == TerrainType::MOUNTAIN) {
        return map->canPlaceUnitAt(x, y);
    }

    return map->canPlaceUnitAt(x, y) && unit->canMoveTo(Position(x, y));
}


bool GameController::isPosInValidAttacks(int x, int y) {

    if (!game || !game->getSelectedUnit() || !game->getMap()) {
        return false;
    }

    Unit* unit = game->getSelectedUnit();
    Unit* target = game->getMap()->getUnitAt(x, y);

    if (!target || target->isDead()) return false;

    if (target->getOwner() == unit->getOwner()) return false;
    return unit->canAttack(target);
}



bool GameController::canMoveToPosition(int x, int y) {
    if (!game || !game->getSelectedUnit() || !game->getMap()) {
        return false;
    }

    if (game->getMap()->isValidPosition(x, y - 1)) {
        if (game->getMap()->getTile(x, y - 1)->getTerrain() == TerrainType::MOUNTAIN) {
            return false;
        }
    }

    Unit* unit = game->getSelectedUnit();
    Map* map = game->getMap();

    return unit->canMoveTo(Position(x, y)) && map->canPlaceUnitAt(x, y);
}

bool GameController::canAttackPosition(int x, int y) {
    if (!game || !game->getSelectedUnit() || !game->getMap()) {
        return false;
    }

    Unit* unit = game->getSelectedUnit();
    Unit* target = game->getMap()->getUnitAt(x, y);

    if (!target) return false;

    return unit->canAttack(target) && target->getOwner() != unit->getOwner();
}

QString GameController::getWinnerName() {
    if (!game || !game->isGameOver()) {
        return "";
    }

    Player* winner = game->getWinner();
    return winner ? QString::fromStdString(winner->getName()) : "Draw";
}

bool GameController::canBuyAt(int x, int y) {
    if (!game || !game->getMap()) return false;

    Map* map = game->getMap();

    if (!map->canPlaceUnitAt(x, y)) return false;

    Tile* tile = map->getTile(x, y);
    if (tile && tile->getTerrain() == TerrainType::MOUNTAIN) {
        return false;
    }

    if (map->isValidPosition(x, y - 1)) {
        if (map->getTile(x, y - 1)->getTerrain() == TerrainType::MOUNTAIN) {
            return false;
        }
    }

    int width = map->getWidth();
    int splitLine = width / 2;
    int playerId = game->getCurrentPlayer()->getId();

    if (playerId == 0 && x >= splitLine) return false;
    if (playerId == 1 && x < splitLine) return false;

    return true;
}

bool GameController::canBuildAt(int x, int y) {
    if (!game || !game->getMap()) return false;

    Map* map = game->getMap();

    if (!map->canPlaceBuildingAt(x, y)) return false;

    int width = map->getWidth();
    int splitLine = width / 2;
    int playerId = game->getCurrentPlayer()->getId();

    if (playerId == 0 && x >= splitLine) return false;
    if (playerId == 1 && x < splitLine) return false;

    return true;
}




void GameController::generateIslandMap() {
    if (!game || !game->getMap()) return;

    Map* map = game->getMap();
    int width = map->getWidth();
    int height = map->getHeight();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Tile* tile = map->getTile(x, y);
            if (!tile) continue;

            bool isGrass = (rand() % 100) < 56;
            tile->setTerrain(isGrass ? TerrainType::GRASS : TerrainType::WATER);
        }
    }


    for (int i = 0; i < 6; ++i) {
        std::vector<TerrainType> newTerrain(width * height);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {

                int waterNeighbors = 0;

                for (int ny = y - 1; ny <= y + 1; ++ny) {
                    for (int nx = x - 1; nx <= x + 1; ++nx) {
                        if (nx == x && ny == y) continue;

                        if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
                            waterNeighbors++;
                        } else {
                            Tile* neighbor = map->getTile(nx, ny);
                            if (neighbor && neighbor->getTerrain() == TerrainType::WATER) {
                                waterNeighbors++;
                            }
                        }
                    }
                }

                Tile* currentTile = map->getTile(x, y);
                int index = y * width + x;

                if (waterNeighbors > 4) {
                    newTerrain[index] = TerrainType::WATER;
                } else if (waterNeighbors < 4) {
                    newTerrain[index] = TerrainType::GRASS;
                } else {
                    newTerrain[index] = currentTile->getTerrain();
                }
            }
        }

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                Tile* tile = map->getTile(x, y);
                if (tile) {
                    tile->setTerrain(newTerrain[y * width + x]);
                }
            }
        }
    }

    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            Tile* t = map->getTile(x, y);
            if (t->getTerrain() == TerrainType::GRASS) {
                int grassNeighbors = 0;
                if (map->getTile(x+1, y)->getTerrain() == TerrainType::GRASS) grassNeighbors++;
                if (map->getTile(x-1, y)->getTerrain() == TerrainType::GRASS) grassNeighbors++;
                if (map->getTile(x, y+1)->getTerrain() == TerrainType::GRASS) grassNeighbors++;
                if (map->getTile(x, y-1)->getTerrain() == TerrainType::GRASS) grassNeighbors++;

                if (grassNeighbors < 2) {
                    t->setTerrain(TerrainType::WATER);
                }
            }
        }
    }

    for (int x = 0; x < width; x++) {
        if(map->getTile(x, 0)) map->getTile(x, 0)->setTerrain(TerrainType::WATER);
        if(map->getTile(x, height - 1)) map->getTile(x, height - 1)->setTerrain(TerrainType::WATER);
    }
    for (int y = 0; y < height; y++) {
        if(map->getTile(0, y)) map->getTile(0, y)->setTerrain(TerrainType::WATER);
        if(map->getTile(width - 1, y)) map->getTile(width - 1, y)->setTerrain(TerrainType::WATER);
    }

    auto SAND = TerrainType::PLAINS;
    std::vector<TerrainType> finalTerrain(width * height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Tile* t = map->getTile(x, y);
            finalTerrain[y * width + x] = t->getTerrain();

            if (t->getTerrain() == TerrainType::GRASS) {
                bool touchesWater = false;
                for (int ny = y - 1; ny <= y + 1; ++ny) {
                    for (int nx = x - 1; nx <= x + 1; ++nx) {
                        if (map->isValidPosition(nx, ny)) {
                            if (map->getTile(nx, ny)->getTerrain() == TerrainType::WATER) {
                                touchesWater = true;
                                break;
                            }
                        }
                    }
                    if (touchesWater) break;
                }

                if (touchesWater) {
                    finalTerrain[y * width + x] = SAND;
                }
            }
        }
    }
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            map->getTile(x, y)->setTerrain(finalTerrain[y * width + x]);
        }
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Tile* t = map->getTile(x, y);

            if (t->getTerrain() == TerrainType::GRASS) {
                int randVal = rand() % 100;

                if (randVal < 5) {
                    t->setTerrain(TerrainType::FOREST);
                }
            }
        }
    }

    int mountainWidth = 3;
    int mountainHeight = 3;
    int attempts = 3;

    for (int i = 0; i < attempts; i++) {
        int startX = (rand() % (width - mountainWidth - 2)) + 1;
        int startY = (rand() % (height - mountainHeight - 2)) + 1;

        bool canPlace = true;

        for (int y = startY; y <= startY + mountainHeight; y++) {
            for (int x = startX; x < startX + mountainWidth; x++) {

                if (!map->isValidPosition(x, y)) {
                    canPlace = false; break;
                }

                Tile* t = map->getTile(x, y);

                if (y < startY + mountainHeight) {
                    if (t->getTerrain() == TerrainType::WATER || t->getTerrain() == TerrainType::MOUNTAIN) {
                        canPlace = false;
                    }
                }
                else {
                    if (t->getTerrain() == TerrainType::WATER) {
                        canPlace = false;
                    }
                }
            }
            if (!canPlace) break;
        }

        if (!canPlace) continue;

        int cliffY = startY + mountainHeight - 1;

        Tile* leftStep = map->getTile(startX - 1, cliffY);
        Tile* rightStep = map->getTile(startX + mountainWidth, cliffY);

        bool canLeft = leftStep && leftStep->getTerrain() == TerrainType::GRASS;
        bool canRight = rightStep && rightStep->getTerrain() == TerrainType::GRASS;

        if (!canLeft && !canRight) continue;

        for (int y = startY; y < startY + mountainHeight; y++) {
            for (int x = startX; x < startX + mountainWidth; x++) {
                Tile* t = map->getTile(x, y);
                t->setTerrain(TerrainType::MOUNTAIN);
                t->setStairsType(0);
            }
        }

        int chosenSide = 0;
        if (canLeft && canRight) chosenSide = (rand() % 2) + 1;
        else if (canLeft) chosenSide = 1;
        else chosenSide = 2;

        if (chosenSide == 1) {
            map->getTile(startX, cliffY)->setStairsType(1);
        } else {
            map->getTile(startX + mountainWidth - 1, cliffY)->setStairsType(2);
        }
    }

    m_mapModel->refreshAll();
}
