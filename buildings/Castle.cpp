#include "Castle.h"
#include "../player/Player.h"

Castle::Castle()
    : Building("Castle", 200, 150), goldPerTurn(5) {
}

std::string Castle::getDescription() const {
    return "Main fortress. Generates " + std::to_string(goldPerTurn) + " gold per turn.";
}

void Castle::onTurnStart() {
    if (owner && !isDestroyed()) {
        owner->addGold(goldPerTurn);
        std::cout << name << " generates " << goldPerTurn << " gold for "
                  << owner->getName() << "!\n";
    }
}
