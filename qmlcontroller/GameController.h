#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include "../game/Game.h"
#include "MapModel.h"
#include "qmlanimace/UnitModel.h"
#include "qmlanimace/BuildingModel.h"

class GameController : public QObject {
    Q_OBJECT

    Q_PROPERTY(int mapWidth READ getMapWidth NOTIFY mapChanged)
    Q_PROPERTY(int mapHeight READ getMapHeight NOTIFY mapChanged)
    Q_PROPERTY(int currentPlayerIndex READ getCurrentPlayerIndex NOTIFY turnChanged)
    Q_PROPERTY(QString currentPlayerName READ getCurrentPlayerName NOTIFY turnChanged)
    Q_PROPERTY(int currentPlayerGold READ getCurrentPlayerGold NOTIFY playerStatsChanged)
    Q_PROPERTY(int currentPlayerUnits READ getCurrentPlayerUnits NOTIFY playerStatsChanged)
    Q_PROPERTY(int currentPlayerBuildings READ getCurrentPlayerBuildings NOTIFY playerStatsChanged)
    Q_PROPERTY(bool hasSelectedUnit READ hasSelectedUnit NOTIFY unitSelectionChanged)
    Q_PROPERTY(bool isGameOver READ isGameOver NOTIFY gameOverChanged)
    Q_PROPERTY(MapModel* mapModel READ getMapModel CONSTANT)
    Q_PROPERTY(UnitModel* unitModel READ getUnitModel CONSTANT)
    Q_PROPERTY(BuildingModel* buildingModel READ getBuildingModel CONSTANT)
    Q_PROPERTY(bool isGameOver READ isGameOver NOTIFY gameOverChanged)

private:
    Game* game;
    MapModel* m_mapModel;
    UnitModel* m_unitModel;
    BuildingModel* m_buildingModel;

public:
    explicit GameController(QObject *parent = nullptr);
    ~GameController();

    int getMapWidth() const;
    int getMapHeight() const;
    int getCurrentPlayerIndex() const;
    QString getCurrentPlayerName() const;
    int getCurrentPlayerGold() const;
    int getCurrentPlayerUnits() const;
    int getCurrentPlayerBuildings() const;
    bool hasSelectedUnit() const;
    bool isGameOver() const;
    void generateIslandMap();
    MapModel* getMapModel() const { return m_mapModel; }
    UnitModel* getUnitModel() const { return m_unitModel; }
    BuildingModel* getBuildingModel() const { return m_buildingModel; }

    Q_INVOKABLE void startNewGame(int playerCount);
    Q_INVOKABLE QVariantMap getTileData(int x, int y);
    Q_INVOKABLE bool selectUnit(int x, int y);
    Q_INVOKABLE bool moveSelectedUnit(int x, int y);
    Q_INVOKABLE bool attackWithSelectedUnit(int x, int y);
    Q_INVOKABLE void deselectUnit();
    Q_INVOKABLE bool buyUnit(int unitType, int x, int y);
    Q_INVOKABLE bool buyBuilding(int buildingType, int x, int y);
    Q_INVOKABLE void endTurn();

    Q_INVOKABLE QVariantMap getSelectedUnitInfo();
    Q_INVOKABLE int getUnitCost(int unitType);
    Q_INVOKABLE int getEffectiveUnitCost(int unitType);
    Q_INVOKABLE int getBuildingCost(int buildingType);
    Q_INVOKABLE QString getWinnerName();

    Q_INVOKABLE bool isPosInValidMoves(int x, int y);
    Q_INVOKABLE bool isPosInValidAttacks(int x, int y);

    Q_INVOKABLE QVariantList getValidMoves();
    Q_INVOKABLE QVariantList getValidAttacks();
    Q_INVOKABLE bool canMoveToPosition(int x, int y);
    Q_INVOKABLE bool canAttackPosition(int x, int y);
    Q_INVOKABLE bool canBuyAt(int x, int y);
    Q_INVOKABLE bool canBuildAt(int x, int y);


signals:
    void mapChanged();
    void turnChanged();
    void playerStatsChanged();
    void unitSelectionChanged();
    void combatOccurred(int attackerX, int attackerY, int targetX, int targetY);
    void gameOverChanged();
    void unitBought(int unitType);
    void buildingBought(int buildingType);
    void unitAttacked(int attackerType);
    void damageDealt(int targetX, int targetY, int damage);
    void invalidAction(QString message);
    void attackAnimationTriggered(qulonglong attackerId, int targetX, int targetY);
    void blockAnimationTriggered(qulonglong unitId);

    void gameLogMessage(QString message);
    void floatingDamageTriggered(int gridX, int gridY, int damage);
    void floatingHealTriggered(int gridX, int gridY, int healAmount);
    void floatingBlockTriggered(int gridX, int gridY);
};

#endif
