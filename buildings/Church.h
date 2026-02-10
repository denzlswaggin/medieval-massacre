#ifndef CHURCH_H
#define CHURCH_H

#include "Building.h"
#include <vector>

class Unit;

class Church : public Building {
private:
    int healAmount;
    int healRange;

public:
    Church();

    BuildingType getType() const override { return BuildingType::CHURCH; }
    std::string getDescription() const override;

    void onTurnStart() override;
    int getHealAmount() const { return healAmount; }
    int getHealRange() const { return healRange; }

    void healNearbyUnits(const std::vector<Unit*>& units);
};

#endif //CHURCH_H
