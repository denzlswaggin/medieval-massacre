#include "Church.h"
#include "../units/Unit.h"
#include <cmath>

Church::Church()
    : Building("Church", 120, 120), healAmount(10), healRange(2) {
}

std::string Church::getDescription() const {
    return "Holy sanctuary. Heals friendly units within range " +
           std::to_string(healRange) + " for " + std::to_string(healAmount) + " HP per turn.";
}

void Church::onTurnStart() {

}

void Church::healNearbyUnits(const std::vector<Unit*>& units) {
    if (isDestroyed() || !owner) return;

    for (Unit* unit : units) {
        if (!unit || unit->isDead()) continue;
        if (unit->getOwner() != owner) continue;

        int dx = std::abs(unit->getPosition().x - position.x);
        int dy = std::abs(unit->getPosition().y - position.y);
        int distance = dx + dy;

        if (distance <= healRange) {
            int currentHealth = unit->getHealth();
            int maxHealth = unit->getMaxHealth();

            if (currentHealth < maxHealth) {
                int healed = std::min(healAmount, maxHealth - currentHealth);
                unit->heal(healed);

                std::cout << "Church heals " << unit->getName()
                          << " for " << healed << " HP!\n";
            }
        }
    }
}
