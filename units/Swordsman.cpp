#include "Swordsman.h"

Swordsman::Swordsman()
    : Unit("Swordsman", 100, 25, 2, 2, 50),
      isDefending(false) {
}

std::string Swordsman::getDescription() const {
    return "Melee fighter with balanced stats. Can defend for reduced damage.";
}

void Swordsman::takeDamage(int dmg) {
    if(isDefending) {
        dmg = dmg / 2;
        std::cout << name << " is defending! Damage reduced.\n";
    }

    Unit::takeDamage(dmg);
    isDefending = false;
}

