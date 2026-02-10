#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <algorithm>
#include "../units/Unit.h"

class Building;

class Player {
private:
    std::string name;
    int id;
    int gold;
    std::vector<Unit*> units;
    std::vector<Building*> buildings;

public:
    Player(const std::string& name,int id, int startGold);
    ~Player();

    std::string getName() const;
    int getId() const;
    int getGold() const;
    std::vector<Unit*> getUnits() const;
    int getUnitCount() const;
    int getLiveUnitCount() const;
    bool canAffordUnit(int cost) const;

    //unit metody
    void addUnit(Unit* unit);
    void removeUnit(Unit* unit);
    bool hasLiveUnits() const;

    //building metody
    std::vector<Building*> getBuildings() const;
    int getBuildingCount() const;
    int getLiveBuildingCount() const;
    void addBuilding(Building* building);
    void removeBuilding(Building* building);
    bool hasLiveBuildings() const;
    void processBuildingEffects();
    bool hasCastle() const;
    int getCastleCount() const;

    // specificke metody pro budovy
    bool hasBarracks() const;
    int getBarracksDiscount() const;

    void addGold(int amount);
    bool spendGold(int amount);
    void resetAllUnits();

};

#endif // PLAYER_H
