//
// Created by daniel on 12/10/2025.
//

#include "Bard.h"

Bard::Bard() : Unit("Bard",50,10,2,2,50), healAmount(20) {

}

std::string Bard::getDescription() const {
    return "Support character, can heal allies";
}
void Bard::heal(Unit* target) {
    if(!canHeal(target)) {
        return;
    }

    int newHealth = target->getHealth() + healAmount;
    if(newHealth > target->getMaxHealth()) {
        newHealth = target->getMaxHealth();
    }

    int healed = newHealth - target->getHealth();

    std::cout << getName() << " heals " << target->getName()
              << " for " << healed << " HP!\n";

    movedThisTurn = true;
}

bool Bard::canHeal(Unit* target) const {
    if(!target || movedThisTurn) {
        return false;
    }
    //kontrola vzdalenosti, asi predelam
    int distance = position.x - target->getPosition().x;
    distance = distance < 0 ? -distance : distance;
    int distY = position.y - target->getPosition().y;
    distY = distY < 0 ? -distY : distY;
    distance += distY;

    return distance <= 2 && target->getHealth() < target->getMaxHealth();
}
