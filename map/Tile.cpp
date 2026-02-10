//
// Created by daniel on 12/10/2025.
//

#include "Tile.h"

Tile::Tile(int x, int y, TerrainType type)
    : position(x, y), terrain(type), occupyingUnit(nullptr),
    occupyingBuilding(nullptr), stairsType(0) {
}
