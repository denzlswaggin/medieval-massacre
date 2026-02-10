//
// Created by daniel on 12/10/2025.
//

#include "Map.h"
#include <iostream>
#include <random>
#include <ctime>
#include "../units/Unit.h"
#include "../buildings/Building.h"

Map::Map(int w, int h) : width(w), height(h) {
    tiles.resize(height);
    for(int y = 0; y < height; ++y) {
        tiles[y].resize(width);
        for(int x = 0; x < width; ++x) {
            tiles[y][x] = new Tile(x, y);
        }
    }
}

Map::~Map() {
    for(auto& row : tiles) {
        for(auto tile : row) {
            delete tile;
        }
    }
}

Tile* Map::getTile(int x, int y) {
    if(!isValidPosition(x, y)) {
        return nullptr;
    }
    return tiles[y][x];
}

Tile* Map::getTile(int x, int y) const {
    if(!isValidPosition(x, y)) {
        return nullptr;
    }
    return tiles[y][x];
}

bool Map::isValidPosition(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

void Map::generateRandomMap() {
    std::srand(std::time(nullptr));

    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            int random = std::rand() % 100;
            TerrainType type;

            if(random < 60) {
                type = TerrainType::GRASS;
            } else if(random < 80) {
                type = TerrainType::FOREST;
            } else if(random < 90) {
                type = TerrainType::MOUNTAIN;
            } else {
                type = TerrainType::WATER;
            }

            tiles[y][x]->setTerrain(type);
        }
    }
}

void Map::printMap() const {
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            TerrainType type = tiles[y][x]->getTerrain();
            char symbol;

            switch(type) {
            case TerrainType::GRASS:    symbol = '.'; break;
            case TerrainType::FOREST:   symbol = 'T'; break;
            case TerrainType::MOUNTAIN: symbol = '^'; break;
            case TerrainType::WATER:    symbol = '~'; break;
            default:                    symbol = '?'; break;
            }

            std::cout << symbol << ' ';
        }
        std::cout << '\n';
    }
}

bool Map::canPlaceUnitAt(int x, int y) const {
    if(!isValidPosition(x, y)) {
        return false;
    }

    Tile* tile = getTile(x, y);

    if(tile->isOccupied()) {
        return false;
    }

    if(tile->getTerrain() == TerrainType::WATER) {
        return false;
    }
    if (tile->getTerrain() == TerrainType::MOUNTAIN) {

        if (tile->getStairsType() > 0) {
            return true;
        }

        Tile* southTile = getTile(x, y + 1);
        if (southTile && southTile->getTerrain() != TerrainType::MOUNTAIN) {
            return false;
        }
        return true;
    }

    return true;
}

bool Map::placeUnit(Unit* unit, int x, int y) {
    if(!canPlaceUnitAt(x, y)) {
        return false;
    }

    Tile* tile = getTile(x, y);
    tile->setOccupyingUnit(unit);
    unit->setPosition(Position(x, y));

    return true;
}

bool Map::placeUnit(Unit* unit, const Position& pos) {
    return placeUnit(unit, pos.x, pos.y);
}

void Map::removeUnit(Unit* unit) {
    if(!unit) return;

    Position pos = unit->getPosition();
    Tile* tile = getTile(pos.x, pos.y);

    if(tile && tile->getOccupyingUnit() == unit) {
        tile->setOccupyingUnit(nullptr);
    }
}

Unit* Map::getUnitAt(int x, int y) const {
    Tile* tile = getTile(x, y);
    return tile ? tile->getOccupyingUnit() : nullptr;
}

Unit* Map::getUnitAt(const Position& pos) const {
    return getUnitAt(pos.x, pos.y);
}


bool Map::canPlaceBuildingAt(int x, int y) const {
    if(!isValidPosition(x, y)) {
        return false;
    }

    Tile* tile = getTile(x, y);

    // Nelze stavět na obsazené dlaždici (jednotkou nebo budovou)
    if(tile->isOccupied() || tile->hasBuilding()) {
        return false;
    }

    // Nelze stavět na vodě
    if(tile->getTerrain() == TerrainType::WATER) {
        return false;
    }

    // Nelze stavět na horách
    if(tile->getTerrain() == TerrainType::MOUNTAIN) {
        return false;
    }

    // Nelze stavět pod útesem
    if (isValidPosition(x, y - 1)) {
        Tile* aboveTile = getTile(x, y - 1);
        if (aboveTile && aboveTile->getTerrain() == TerrainType::MOUNTAIN) {
            return false;
        }
    }

    return true;
}

bool Map::placeBuilding(Building* building, int x, int y) {
    if(!canPlaceBuildingAt(x, y)) {
        return false;
    }

    Tile* tile = getTile(x, y);
    tile->setOccupyingBuilding(building);
    building->setPosition(Position(x, y));

    return true;
}

bool Map::placeBuilding(Building* building, const Position& pos) {
    return placeBuilding(building, pos.x, pos.y);
}

void Map::removeBuilding(Building* building) {
    if(!building) return;

    Position pos = building->getPosition();
    Tile* tile = getTile(pos.x, pos.y);

    if(tile && tile->getOccupyingBuilding() == building) {
        tile->setOccupyingBuilding(nullptr);
    }
}

Building* Map::getBuildingAt(int x, int y) const {
    Tile* tile = getTile(x, y);
    return tile ? tile->getOccupyingBuilding() : nullptr;
}

Building* Map::getBuildingAt(const Position& pos) const {
    return getBuildingAt(pos.x, pos.y);
}
