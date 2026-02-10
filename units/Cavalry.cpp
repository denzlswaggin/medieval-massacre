#include "Cavalry.h"

Cavalry::Cavalry()
    : Unit("Cavalry", 120, 30, 4, 4, 100),
      hasCharged(false) {
}

std::string Cavalry::getDescription() const {
    return "Fast and powerful. Can charge (move + attack in one turn).";
}

void Cavalry::attack(Unit* target) {
    if(!target || !canAttack(target)) {
        return;
    }

    // Bonus damage kdyz chargne
    int totalDamage = damage;
    if(!movedThisTurn) {
        totalDamage += 15;
        std::cout << name << " charges!\n";
    }

    target->takeDamage(totalDamage);
    attackedThisTurn = true;
}

void Cavalry::move(const Position& newPos) {
    Unit::move(newPos);
    //toto ze se muze hybat i potom kdyz da charge
}

void Cavalry::charge(Unit* target) {
    if(!target || hasCharged) {
        return;
    }

    //tady by se mel pohnout k cili
    Position targetPos = target->getPosition();


    int dx = targetPos.x - position.x;
    int dy = targetPos.y - position.y;


    if(dx != 0) dx = dx / abs(dx);
    if(dy != 0) dy = dy / abs(dy);

    Position newPos(position.x + dx, position.y + dy);
    position = newPos;


    attack(target);
    hasCharged = true;
    movedThisTurn = true;

    std::cout << name << " charges at " << target->getName() << "!\n";
}

void Cavalry::resetTurn() {
    Unit::resetTurn();
    hasCharged = false;
}
