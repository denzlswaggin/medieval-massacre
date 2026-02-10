//
// Created by daniel on 12/10/2025.
//

#ifndef TILE_H
#define TILE_H
#include "Position.h"
#include "TerrainType.h"
#include "../buildings/Building.h"

class Unit;
class Building;

class Tile {

    Position position;
    TerrainType terrain;
    Unit* occupyingUnit;
    Building* occupyingBuilding;
    int stairsType;

public:
    Tile(int x, int y, TerrainType type = TerrainType::GRASS);

    Position getPosition() const {
        return position;
    }
    TerrainType getTerrain() const {
        return terrain;
    }
    void setTerrain(TerrainType type) {
        terrain = type;
    }

    // unit metody
    Unit* getOccupyingUnit() const {
        return occupyingUnit;
    }
    void setOccupyingUnit(Unit* unit) {
        occupyingUnit = unit;
    }
    bool isOccupied() const {
        return occupyingUnit != nullptr;
    }

    // building metody
    Building* getOccupyingBuilding() const {
        return occupyingBuilding;
    }
    void setOccupyingBuilding(Building* building) {
        occupyingBuilding = building;
    }
    bool hasBuilding() const {
        return occupyingBuilding != nullptr;
    }

    int getStairsType() const { return stairsType; }
    void setStairsType(int type) { stairsType = type; }

};



#endif //TILE_H
