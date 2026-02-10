#include "Barracks.h"

Barracks::Barracks()
    : Building("Barracks", 150, 100), discountPercent(20) {
}

std::string Barracks::getDescription() const {
    return "Military training facility. Reduces unit costs by " +
           std::to_string(discountPercent) + "%.";
}

int Barracks::applyDiscount(int originalCost) const {
    if (isDestroyed()) {
        return originalCost;
    }
    return originalCost - (originalCost * discountPercent / 100);
}
