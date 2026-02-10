#include "UnitModel.h"
#include "player/Player.h"
#include "units/Swordsman.h"

UnitModel::UnitModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void UnitModel::setUnits(const std::vector<Unit *> &units)
{
    beginResetModel();
    m_units.clear(); // Vyčistíme starý seznam

    for (Unit* unit : units) {
        if (!unit->isDead()) {
            m_units.push_back(unit);
        }
    }
    endResetModel();
}

void UnitModel::refresh()
{
    if (!m_units.empty()) {
        emit dataChanged(index(0), index(m_units.size() - 1));
    }
}

int UnitModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return static_cast<int>(m_units.size());
}

QVariant UnitModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_units.size())
        return QVariant();

    Unit* unit = m_units[index.row()];

    switch (role) {
    case TypeRole: return static_cast<int>(unit->getType());
    case OwnerRole: return unit->getOwner()->getId();
    case XRole: return unit->getPosition().x;
    case YRole: return unit->getPosition().y;
    case IdRole: return (qulonglong)unit;
    case HealthRole: return unit->getHealth();
    case MaxHealthRole: return unit->getMaxHealth();   
    }

    return QVariant();
}

QHash<int, QByteArray> UnitModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TypeRole] = "unitType";
    roles[OwnerRole] = "unitOwner";
    roles[XRole] = "unitX";
    roles[YRole] = "unitY";
    roles[IdRole] = "unitId";
    roles[HealthRole] = "unitHealth";
    roles[MaxHealthRole] = "unitMaxHealth";
    roles[IsDefendingRole] = "unitIsDefending";
    return roles;
}
