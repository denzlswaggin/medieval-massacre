#ifndef CAVALRY_H
#define CAVALRY_H

#include "Unit.h"

class Cavalry : public Unit {
private:
    bool hasCharged;

public:
    Cavalry();

    UnitType getType() const override { return UnitType::CAVALRY; }
    std::string getDescription() const override;

    void attack(Unit* target) override;
    void move(const Position& newPos) override;
    void charge(Unit* target); // Pohyb + útok v jednom tahu
    bool canCharge() const { return !hasCharged; }
    void resetTurn();
};

#endif