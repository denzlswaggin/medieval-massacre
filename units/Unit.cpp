//
// Created by daniel on 12/10/2025.
//

#include "Unit.h"

Unit::Unit(const std::string &name, int health, int damage, int movementRange, int attackRange, int cost) : name(name),
health(health),maxHealth(health),damage(damage),movementRange(movementRange),attackRange(attackRange),cost(cost),position(0,0),owner(nullptr),
movedThisTurn(false),attackedThisTurn(false){

}

void Unit::attack(Unit* target) {
    if(!target || !canAttack(target)) {
        return;
    }

    target->takeDamage(damage);
    attackedThisTurn = true;

    std::cout << name << " attacks " << target->getName()
              << " for " << damage << " damage!\n";
}

bool Unit::canAttack(Unit* target) const {
    if(!target || attackedThisTurn) {
        return false;
    }
    //vzdalenost nvm jestli funguje
    int distance = position.x - target->getPosition().x;
    distance = distance < 0 ? -distance : distance;
    int distY = position.y - target->getPosition().y;
    distY = distY < 0 ? -distY : distY;
    distance += distY;

    return distance <= attackRange;
}

void Unit::takeDamage(int dmg) {
    health -= dmg;
    if(health < 0) {
        health = 0;
    }
    std::cout << name << " takes " << dmg << " damage! (" << health  << " HP)\n";
}

bool Unit::canMoveTo(const Position& pos) const {
    if(movedThisTurn) {
        return false;
    }
    //vzdalenost nvm jestli funguje
    int distance = position.x - pos.x;
    distance = distance < 0 ? -distance : distance;
    int distY = position.y - pos.y;
    distY = distY < 0 ? -distY : distY;
    distance += distY;

    return distance <= movementRange;
}

void Unit::move(const Position& newPos) {
    if(!canMoveTo(newPos)) {
        return;
    }

    position = newPos;
    movedThisTurn = true;

    std::cout << name << " moves to (" << newPos.x << ", " << newPos.y << ")\n";
}

void Unit::resetTurn() {
    movedThisTurn = false;
    attackedThisTurn = false;
}
