//
// Created by daniel on 12/10/2025.
//

#ifndef MAP_H
#define MAP_H
#include "Tile.h"
#include <vector>
#include "../units/Unit.h"
#include "../buildings/Building.h"
#include <random>


class Map {
private:
    int width;
    int height;
    std::vector<std::vector<Tile*>> tiles;

public:
    Map(int w, int h);
    ~Map();

    Tile* getTile(int x, int y);
    Tile* getTile(int x, int y) const;
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isValidPosition(int x, int y) const;

    void generateRandomMap();
    void printMap() const;

    // Unit metody na mapě
    bool placeUnit(Unit* unit, int x, int y);
    bool placeUnit(Unit* unit, const Position& pos);
    bool canPlaceUnitAt(int x, int y) const;
    void removeUnit(Unit* unit);
    Unit* getUnitAt(int x, int y) const;
    Unit* getUnitAt(const Position& pos) const;

    // Building metody na mapě
    bool placeBuilding(Building* building, int x, int y);
    bool placeBuilding(Building* building, const Position& pos);
    bool canPlaceBuildingAt(int x, int y) const;
    void removeBuilding(Building* building);
    Building* getBuildingAt(int x, int y) const;
    Building* getBuildingAt(const Position& pos) const;

};

#endif //MAP_H
