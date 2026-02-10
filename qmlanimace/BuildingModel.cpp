#include "BuildingModel.h"
#include "../player/Player.h"

BuildingModel::BuildingModel(QObject *parent)
    : QAbstractListModel(parent) {
}

void BuildingModel::setBuildings(const std::vector<Building*>& buildings) {
    beginResetModel();
    m_buildings.clear();

    for (Building* building : buildings) {
        if (!building->isDestroyed()) {
            m_buildings.push_back(building);
        }
    }
    endResetModel();
}

void BuildingModel::refresh() {
    if (!m_buildings.empty()) {
        emit dataChanged(index(0), index(m_buildings.size() - 1));
    }
}

int BuildingModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_buildings.size());
}

QVariant BuildingModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(m_buildings.size()))
        return QVariant();

    Building* building = m_buildings[index.row()];

    switch (role) {
    case TypeRole: return static_cast<int>(building->getType());
    case OwnerRole: return building->getOwner() ? building->getOwner()->getId() : -1;
    case XRole: return building->getPosition().x;
    case YRole: return building->getPosition().y;
    case IdRole: return (qulonglong)building;
    case HealthRole:
        // debug
        qDebug() << "Building health:" << building->getHealth();
        return building->getHealth();
    case MaxHealthRole:
        // debug
        qDebug() << "Building maxHealth:" << building->getMaxHealth();
        return building->getMaxHealth();
    case NameRole: return QString::fromStdString(building->getName());
    }

    return QVariant();
}
QHash<int, QByteArray> BuildingModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TypeRole] = "buildingType";
    roles[OwnerRole] = "buildingOwner";
    roles[XRole] = "buildingX";
    roles[YRole] = "buildingY";
    roles[IdRole] = "buildingId";
    roles[HealthRole] = "buildingHealth";
    roles[MaxHealthRole] = "buildingMaxHealth";
    roles[NameRole] = "buildingName";
    return roles;
}
