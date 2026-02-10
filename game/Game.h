//
// Created by kubdv on 12.12.2025.
//

#ifndef GAME_H
#define GAME_H

#include "../map/Map.h"
#include "../player/Player.h"
#include "../units/Unit.h"
#include "../units/UnitType.h"
#include "../buildings/BuildingType.h"
#include "../buildings/Building.h"
#include "TurnManager.h"
#include <vector>

enum class GameState {
    MENU,
    SETUP,
    CASTLE_PLACEMENT,
    PLAYING,
    GAME_OVER
};

class Game {
private:
    Map* map;
    TurnManager* turnManager;
    std::vector<Player*> players;
    std::vector<Unit*> allUnits;
    std::vector<Building*> allBuildings;
    GameState state;
    Unit* selectedUnit;
    int castlesPlaced;


public:
    Game();
    ~Game();

    void initializeGame(int playerCount, int mapWidth, int mapHeight);
    void startGame();

    // unit metody
    bool selectUnit(int x, int y);
    bool moveSelectedUnit(int x, int y);
    bool attackWithSelectedUnit(int x, int y);
    void deselectUnit();
    bool buyUnit(UnitType type, int x, int y);
    int getUnitCost(UnitType type) const;
    Player* getCurrentPlayer() const;
    int getEffectiveUnitCost(UnitType type) const; // sleva z barracks

    // building metody
    bool buyBuilding(BuildingType type, int x, int y);
    int getBuildingCost(BuildingType type) const;
    bool attackBuilding(int x, int y); // útok jednotkou na budovu

    void update();
    bool isGameOver() const;
    Player* getWinner() const;
    void endTurn();

    const std::vector<Unit*>& getAllUnits() const { return allUnits; }
    const std::vector<Building*>& getAllBuildings() const { return allBuildings; }
    Map* getMap() const { return map; }
    TurnManager* getTurnManager() const { return turnManager; }
    Unit* getSelectedUnit() const { return selectedUnit; }
    GameState getState() const { return state; }
    std::vector<Player*> getPlayers() const { return players; }


private:
    void cleanupDeadUnits();
    void cleanupDestroyedBuildings();
    bool isValidMove(Unit* unit, int x, int y) const;
    bool isValidAttack(Unit* unit, int targetX, int targetY) const;
    Unit* createUnit(UnitType type);
    Building* createBuilding(BuildingType type);
    void processBuildingEffectsForPlayer(Player* player);

};
#endif
