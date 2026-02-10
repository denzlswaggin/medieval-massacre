//
// Created by daniel on 12/10/2025.
//

#ifndef SWORDSMAN_H
#define SWORDSMAN_H

#include "Unit.h"
#include <cstdlib>

class Swordsman : public Unit {
private:
    bool isDefending;

public:
    Swordsman();

    UnitType getType() const override { return UnitType::SWORDSMAN; }
    std::string getDescription() const override;

    void takeDamage(int dmg) override;
    bool tryAutoBlock() const {
        return (rand() % 100) < 30;
    }
};


#endif //SWORDSMAN_H
