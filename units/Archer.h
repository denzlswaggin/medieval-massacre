#ifndef ARCHER_H
#define ARCHER_H

#include "Unit.h"

class Archer : public Unit {
private:
    int bonusDamageAtRange;

public:
    Archer();

    UnitType getType() const override { return UnitType::ARCHER; }
    std::string getDescription() const override;

    void attack(Unit* target) override;
    bool canAttack(Unit* target) const override;
    int calculateDamage(Unit* target) const;
};

#endif