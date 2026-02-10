//
// Created by daniel on 12/10/2025.
//

#ifndef UNIT_H
#define UNIT_H

#include "../map/Position.h"
#include "UnitType.h"
#include <string>
#include <iostream>

class Player;

class Unit {
protected:
    std::string name;
    int health;
    int maxHealth;
    int damage;
    int attackRange;
    int movementRange;
    int cost;
    Position position;
    Player* owner;
    bool movedThisTurn;
    bool attackedThisTurn;

public:
    Unit(const std::string& name, int health, int damage, int movementRange, int attackRange, int cost);
    virtual ~Unit() = default;
    std::string getName() const {
        return name;
    }
    int getHealth() const {
        return health;
    }
    int getMaxHealth() const {
        return maxHealth;
    }
    int getDamage() const {
        return damage;
    }
    int getMovementRange() const {
        return movementRange;
    }
    int getAttackRange() const {
        return attackRange;
    }
    Position getPosition() const {
        return position;
    }
    Player* getOwner() const {
        return owner;
    }
    int getCost() const {
        return cost;
    }
    bool isDead() const {
        return health <= 0;
    }
    void setPosition(const Position& pos) {
        position = pos;
    }
    void setOwner(Player* p) {
        owner = p;
    }

    void setHealth(int h) {
        health = h;
        if (health > maxHealth) health = maxHealth;
        if (health < 0) health = 0;
    }

    void heal(int amount) {
        health += amount;
        if (health > maxHealth) {
            health = maxHealth;
        }
    }
    virtual void attack(Unit* target);

    virtual bool canAttack(Unit* target) const;

    virtual void takeDamage(int dmg);

    virtual bool canMoveTo(const Position& pos) const;

    virtual void move(const Position& newPos);

    virtual UnitType getType() const = 0;

    virtual std::string getDescription() const = 0;
    void resetTurn();
};



#endif //UNIT_H
