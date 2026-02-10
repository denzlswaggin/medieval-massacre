//
// Created by kubdv on 12.12.2025.
//

#include "TurnManager.h"
#include "Game.h"
#include <iostream>

TurnManager::TurnManager()
    : currentPlayerIndex(0), turnNumber(1), game(nullptr) {
}

TurnManager::~TurnManager() {
    players.clear();
}

void TurnManager::nextTurn() {
    if (players.empty()) return;

    std::cout << "DEBUG: nextTurn() called, currentPlayerIndex = " << currentPlayerIndex << "\n";

    // ✅ Během CASTLE_PLACEMENT prostě rotujeme hráče
    if (game && game->getState() == GameState::CASTLE_PLACEMENT) {
        std::cout << "DEBUG: In CASTLE_PLACEMENT mode\n";
        currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
        std::cout << "DEBUG: New player = " << currentPlayerIndex << "\n";

        if (currentPlayerIndex == 0) {
            turnNumber++;
        }

        Player* player = getCurrentPlayer();
        if (player) {
            player->resetAllUnits();
            std::cout << "\n=== Castle Placement - Turn " << turnNumber << " ===\n";
            std::cout << player->getName() << "'s turn to place Castle\n";
        }
        return;
    }

    std::cout << "DEBUG: In normal game mode\n";

    // ✅ Normální hra
    int attempts = 0;
    int maxAttempts = players.size();

    do {
        currentPlayerIndex = (currentPlayerIndex + 1) % players.size();

        if (currentPlayerIndex == 0) {
            turnNumber++;
        }

        attempts++;

        Player* player = getCurrentPlayer();

        if (player && player->hasCastle()) {
            player->resetAllUnits();
            std::cout << "\n=== Turn " << turnNumber << " ===\n";
            std::cout << player->getName() << "'s turn\n";
            return;
        }

        if (attempts >= maxAttempts) {
            std::cout << "\n=== NO PLAYERS WITH CASTLE - GAME OVER ===\n";
            return;
        }

    } while (true);
}

void TurnManager::addPlayer(Player* player) {
    if (player) {
        players.push_back(player);
    }
}

void TurnManager::startGame() {
    if (players.empty()) {
        std::cout << "No players to start game!\n";
        return;
    }

    currentPlayerIndex = 0;
    turnNumber = 1;

    std::cout << "=== Game Started ===\n";
    std::cout << "Players: " << players.size() << "\n";
    std::cout << getCurrentPlayer()->getName() << "'s turn\n";
}

void TurnManager::endCurrentTurn() {
    std::cout << getCurrentPlayer()->getName() << " ends their turn.\n";
    nextTurn();
}

Player* TurnManager::getCurrentPlayer() const {
    if (players.empty()) return nullptr;
    return players[currentPlayerIndex];
}

bool TurnManager::isGameOver() const {
    int playersWithCastle = 0;

    if (game && game->getState() == GameState::CASTLE_PLACEMENT) {
        return false;
    }

    for (Player* player : players) {
        if (player && player->hasCastle()) {
            playersWithCastle++;
            std::cout << player->getName() << " still has a Castle!\n";
        } else if (player) {
            std::cout << player->getName() << " has NO Castle!\n";
        }
    }

    bool gameOver = playersWithCastle <= 1;
    if (gameOver) {
        std::cout << "=== GAME OVER: Only " << playersWithCastle << " castle(s) remain! ===\n";
    }

    return playersWithCastle < 2;
}

Player* TurnManager::getWinner() const {
    if (!isGameOver()) return nullptr;

    for (Player* player : players) {
        if (player && player->hasCastle()) { // vitez je ten kdo ma castle
            return player;
        }
    }

    return nullptr; // remize, oba dva ho ztratili soucasne ( nemelo by se stat )
}

std::vector<Player*> TurnManager::getAlivePlayers() const {
    std::vector<Player*> alive;

    for (Player* player : players) {
        if (player && player->hasLiveUnits()) {
            alive.push_back(player);
        }
    }

    return alive;
}
