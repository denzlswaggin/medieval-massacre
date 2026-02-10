//
// Created by kubdv on 12.12.2025.
//

#ifndef TURNMANAGER_H
#define TURNMANAGER_H

#include <vector>
#include "../player/Player.h"

class Game;

class TurnManager {
private:
    std::vector<Player*> players;
    int currentPlayerIndex;
    int turnNumber;
    Game* game;

public:
    TurnManager();
    ~TurnManager();

    void setGame(Game* g) { game = g; }

    void addPlayer(Player* player);
    void startGame();

    void nextTurn();
    void endCurrentTurn();

    Player* getCurrentPlayer() const;
    int getCurrentPlayerIndex() const { return currentPlayerIndex; }
    int getTurnNumber() const { return turnNumber; }
    int getPlayerCount() const { return players.size(); }
    std::vector<Player*> getAllPlayers() const { return players; }

    bool isGameOver() const;
    Player* getWinner() const;
    std::vector<Player*> getAlivePlayers() const;


};
#endif
