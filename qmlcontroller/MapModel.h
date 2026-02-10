#ifndef MAPMODEL_H
#define MAPMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include "../game/Game.h"

class MapModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum TileRoles {
        XRole = Qt::UserRole + 1,
        YRole,
        TerrainRole,
        BitmaskRole,
        MountainMaskRole,
        HasUnitRole,
        UnitTypeRole,
        UnitOwnerRole,
        UnitHealthRole,
        UnitMaxHealthRole,
        UnitNameRole,
        StairsTypeRole,
        // Building roles
        HasBuildingRole,
        BuildingTypeRole,
        BuildingOwnerRole,
        BuildingHealthRole,
        BuildingMaxHealthRole,
        BuildingNameRole
    };

    explicit MapModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setGame(Game* game);
    void refreshAll();
    void refreshTile(int x, int y);
    void refreshTiles(const QList<QPair<int,int>>& positions);

private:
    Game* m_game;
    int m_width;
    int m_height;

    int indexFromPos(int x, int y) const { return y * m_width + x; }
};

#endif
