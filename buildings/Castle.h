#ifndef CASTLE_H
#define CASTLE_H

#include "Building.h"

class Castle : public Building {
private:
    int goldPerTurn;

public:
    Castle();

    BuildingType getType() const override { return BuildingType::CASTLE; }
    std::string getDescription() const override;

    void onTurnStart() override;
    int getGoldPerTurn() const { return goldPerTurn; }
};

#endif //CASTLE_H
