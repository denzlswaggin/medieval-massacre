#ifndef BARRACKS_H
#define BARRACKS_H

#include "Building.h"

class Barracks : public Building {
private:
    int discountPercent;

public:
    Barracks();

    BuildingType getType() const override { return BuildingType::BARRACKS; }
    std::string getDescription() const override;

    int getDiscountPercent() const { return discountPercent; }
    int applyDiscount(int originalCost) const;
};

#endif //BARRACKS_H
