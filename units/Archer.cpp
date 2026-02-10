#include "Archer.h"

Archer::Archer() 
    : Unit("Archer", 70, 20, 2, 3, 60),
      bonusDamageAtRange(10) {
}

std::string Archer::getDescription() const {
    return "Ranged attacker with long range. Deals bonus damage from distance.";
}

void Archer::attack(Unit* target) {
    if(!target || !canAttack(target)) {
        return;
    }

    int totalDamage = calculateDamage(target);
    target->takeDamage(totalDamage);
    attackedThisTurn = true;

    std::cout << name << " shoots " << target->getName() 
              << " for " << totalDamage << " damage!\n";
}

bool Archer::canAttack(Unit* target) const {
    if(!target || attackedThisTurn) {
        return false;
    }

    int distance = position.x - target->getPosition().x;
    distance = distance < 0 ? -distance : distance;
    int distY = position.y - target->getPosition().y;
    distY = distY < 0 ? -distY : distY;
    distance += distY;


    return distance >= 2 && distance <= attackRange;
}

int Archer::calculateDamage(Unit* target) const {
    int distance = position.x - target->getPosition().x;
    distance = distance < 0 ? -distance : distance;
    int distY = position.y - target->getPosition().y;
    distY = distY < 0 ? -distY : distY;
    distance += distY;


    if(distance >= 3) {
        return damage + bonusDamageAtRange;
    }

    return damage;
}