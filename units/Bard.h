//
// Created by daniel on 12/10/2025.
//

#ifndef BARD_H
#define BARD_H
#include "Unit.h"
#include <vector>

class Bard : public Unit{
    int healAmount;
public:
    Bard();
    UnitType getType() const override {
        return UnitType::BARD;
    }
    std::string getDescription() const override;

    void heal(Unit* target);
    bool canHeal(Unit* target) const;
};



#endif //BARD_H
