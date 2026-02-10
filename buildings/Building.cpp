#include "Building.h"

Building::Building(const std::string& name, int health, int cost)
    : name(name), health(health), maxHealth(health), cost(cost),
    position(0, 0), owner(nullptr) {
}

void Building::takeDamage(int dmg) {
    health -= dmg;
    if (health < 0) {
        health = 0;
    }
    std::cout << name << " takes " << dmg << " damage! (" << health << " HP)\n";
}
