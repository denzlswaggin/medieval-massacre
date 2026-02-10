#include "Player.h"
#include "../buildings/Building.h"
#include "../buildings/Barracks.h"
#include "../buildings/Castle.h"
#include "../buildings/Church.h"

Player::Player(const std::string& name, int id, int startGold)
    : name(name), id(id), gold(startGold) {
}

Player::~Player() {
    units.clear();
    buildings.clear();
}

std::string Player::getName() const {
    return name;
}

int Player::getId() const {
    return id;
}

int Player::getGold() const {
    return gold;
}

std::vector<Unit*> Player::getUnits() const {
    return units;
}

std::vector<Building*> Player::getBuildings() const {
    return buildings;
}

int Player::getUnitCount() const {
    return units.size();
}

int Player::getLiveUnitCount() const {
    int count = 0;
    for (const Unit* unit : units) {
        if (unit && !unit->isDead()) {
            count++;
        }
    }
    return count;
}

int Player::getBuildingCount() const {
    return buildings.size();
}

int Player::getLiveBuildingCount() const {
    int count = 0;
    for (const Building* building : buildings) {
        if (building && !building->isDestroyed()) {
            count++;
        }
    }
    return count;
}

bool Player::canAffordUnit(int cost) const {
    return gold >= cost;
}

void Player::addUnit(Unit* unit) {
    units.push_back(unit);
    unit->setOwner(this);
}

void Player::removeUnit(Unit* unit) {
    auto it = std::remove(units.begin(), units.end(), unit);
    if (it != units.end()) {
        units.erase(it, units.end());
    }
}

bool Player::hasLiveUnits() const {
    for (const Unit* unit : units) {
        if (unit && !unit->isDead()) {
            return true;
        }
    }
    return false;
}

void Player::addBuilding(Building* building) {
    buildings.push_back(building);
    building->setOwner(this);
}

void Player::removeBuilding(Building* building) {
    auto it = std::remove(buildings.begin(), buildings.end(), building);
    if (it != buildings.end()) {
        buildings.erase(it, buildings.end());
    }
}

bool Player::hasLiveBuildings() const {
    for (const Building* building : buildings) {
        if (building && !building->isDestroyed()) {
            return true;
        }
    }
    return false;
}

bool Player::hasBarracks() const {
    for (const Building* building : buildings) {
        if (building && !building->isDestroyed() &&
            building->getType() == BuildingType::BARRACKS) {
            return true;
        }
    }
    return false;
}

int Player::getBarracksDiscount() const {
    for (const Building* building : buildings) {
        if (building && !building->isDestroyed() &&
            building->getType() == BuildingType::BARRACKS) {
            const Barracks* barracks = dynamic_cast<const Barracks*>(building);
            if (barracks) {
                return barracks->getDiscountPercent();
            }
        }
    }
    return 0;
}

bool Player::hasCastle() const {
    for (const Building* building : buildings) {
        if (building && !building->isDestroyed() &&
            building->getType() == BuildingType::CASTLE) {
            return true;
        }
    }
    return false;
}

int Player::getCastleCount() const {
    int count = 0;
    for (const Building* building : buildings) {
        if (building && !building->isDestroyed() &&
            building->getType() == BuildingType::CASTLE) {
            count++;
        }
    }
    return count;
}

void Player::addGold(int amount) {
    gold += amount;
}

bool Player::spendGold(int amount) {
    if (gold >= amount) {
        gold -= amount;
        return true;
    }
    return false;
}

void Player::resetAllUnits() {
    for (Unit* unit : units) {
        if (unit && !unit->isDead()) {
            unit->resetTurn();
        }
    }
}

void Player::processBuildingEffects() {
    for (Building* building : buildings) {
        if (building && !building->isDestroyed()) {
            building->onTurnStart();
        }
    }
}
