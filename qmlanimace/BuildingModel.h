#ifndef BUILDINGMODEL_H
#define BUILDINGMODEL_H

#include <QAbstractListModel>
#include <vector>
#include "../buildings/Building.h"

class BuildingModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum BuildingRoles {
        TypeRole = Qt::UserRole + 1,
        OwnerRole,
        XRole,
        YRole,
        IdRole,
        HealthRole,
        MaxHealthRole,
        NameRole
    };

    explicit BuildingModel(QObject *parent = nullptr);

    void setBuildings(const std::vector<Building*>& buildings);
    void refresh();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    std::vector<Building*> m_buildings;
};

#endif //BUILDINGMODEL_H
