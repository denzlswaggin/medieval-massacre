#include "MapModel.h"

MapModel::MapModel(QObject *parent)
    : QAbstractListModel(parent), m_game(nullptr), m_width(0), m_height(0) {
}

int MapModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid() || !m_game || !m_game->getMap())
        return 0;
    return m_width * m_height;
}

QHash<int, QByteArray> MapModel::roleNames() const {
    return {
        {XRole, "x"},
        {YRole, "y"},
        {TerrainRole, "terrain"},
        {BitmaskRole, "bitmask"},
        {MountainMaskRole, "mountainMask"},
        {HasUnitRole, "hasUnit"},
        {UnitTypeRole, "unitType"},
        {UnitOwnerRole, "unitOwner"},
        {UnitHealthRole, "unitHealth"},
        {UnitMaxHealthRole, "unitMaxHealth"},
        {UnitNameRole, "unitName"},
        {StairsTypeRole, "stairsType"},
        // Building roles
        {HasBuildingRole, "hasBuilding"},
        {BuildingTypeRole, "buildingType"},
        {BuildingOwnerRole, "buildingOwner"},
        {BuildingHealthRole, "buildingHealth"},
        {BuildingMaxHealthRole, "buildingMaxHealth"},
        {BuildingNameRole, "buildingName"}
    };
}

void MapModel::setGame(Game* game) {
    beginResetModel();
    m_game = game;
    if (m_game && m_game->getMap()) {
        m_width = m_game->getMap()->getWidth();
        m_height = m_game->getMap()->getHeight();
    } else {
        m_width = 0;
        m_height = 0;
    }
    endResetModel();
}

QVariant MapModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || !m_game || !m_game->getMap())
        return QVariant();

    int idx = index.row();
    int x = idx % m_width;
    int y = idx / m_width;

    Map* map = m_game->getMap();
    Tile* tile = map->getTile(x, y);
    if (!tile) return QVariant();

    Unit* unit = map->getUnitAt(x, y);
    Building* building = map->getBuildingAt(x, y);

    switch (role) {
    case XRole: return x;
    case YRole: return y;
    case TerrainRole: return static_cast<int>(tile->getTerrain());
    case BitmaskRole: {
        int bitmask = 0;
        auto isConnected = [&](int tx, int ty) {
            if (!map->isValidPosition(tx, ty)) return false;
            TerrainType neighborType = map->getTile(tx, ty)->getTerrain();
            if (tile->getTerrain() == TerrainType::WATER)
                return neighborType == TerrainType::WATER;
            return neighborType != TerrainType::WATER;
        };
        if (isConnected(x, y - 1)) bitmask += 1;
        if (isConnected(x + 1, y)) bitmask += 2;
        if (isConnected(x, y + 1)) bitmask += 4;
        if (isConnected(x - 1, y)) bitmask += 8;
        return bitmask;
    }
    case MountainMaskRole: {
        if (tile->getTerrain() != TerrainType::MOUNTAIN) return 0;
        int mMask = 0;
        auto isM = [&](int tx, int ty) {
            if (!map->isValidPosition(tx, ty)) return false;
            return map->getTile(tx, ty)->getTerrain() == TerrainType::MOUNTAIN;
        };
        if (isM(x, y - 1)) mMask += 1;
        if (isM(x + 1, y)) mMask += 2;
        if (isM(x, y + 1)) mMask += 4;
        if (isM(x - 1, y)) mMask += 8;
        return mMask;
    }
    case HasUnitRole: return unit && !unit->isDead();
    case UnitTypeRole: return unit ? static_cast<int>(unit->getType()) : -1;
    case UnitOwnerRole: return unit && unit->getOwner() ? unit->getOwner()->getId() : -1;
    case UnitHealthRole: return unit ? unit->getHealth() : 0;
    case UnitMaxHealthRole: return unit ? unit->getMaxHealth() : 0;
    case UnitNameRole: return unit ? QString::fromStdString(unit->getName()) : "";
    case StairsTypeRole: return tile->getStairsType();
    // Building data
    case HasBuildingRole: return building && !building->isDestroyed();
    case BuildingTypeRole: return building ? static_cast<int>(building->getType()) : -1;
    case BuildingOwnerRole: return building && building->getOwner() ? building->getOwner()->getId() : -1;
    case BuildingHealthRole: return building ? building->getHealth() : 0;
    case BuildingMaxHealthRole: return building ? building->getMaxHealth() : 0;
    case BuildingNameRole: return building ? QString::fromStdString(building->getName()) : "";
    }
    return QVariant();
}

void MapModel::refreshAll() {
    emit dataChanged(index(0), index(rowCount() - 1));
}

void MapModel::refreshTile(int x, int y) {
    int idx = indexFromPos(x, y);
    emit dataChanged(index(idx), index(idx));
}

void MapModel::refreshTiles(const QList<QPair<int,int>>& positions) {
    for (const auto& pos : positions) {
        refreshTile(pos.first, pos.second);
    }
}
