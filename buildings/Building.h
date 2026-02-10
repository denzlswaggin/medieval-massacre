#ifndef BUILDING_H
#define BUILDING_H

#include "../map/Position.h"
#include "BuildingType.h"
#include <string>
#include <iostream>

class Player;

class Building {
protected:
    std::string name;
    int health;
    int maxHealth;
    int cost;
    Position position;
    Player* owner;

public:
    Building(const std::string& name, int health, int cost);
    virtual ~Building() = default;

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }
    int getCost() const { return cost; }
    Position getPosition() const { return position; }
    Player* getOwner() const { return owner; }
    bool isDestroyed() const { return health <= 0; }

    void setPosition(const Position& pos) { position = pos; }
    void setOwner(Player* p) { owner = p; }

    virtual void takeDamage(int dmg);
    virtual BuildingType getType() const = 0;
    virtual std::string getDescription() const = 0;

    virtual void onTurnStart() {}
    virtual void onTurnEnd() {}
};

#endif //BUILDING_H
