#ifndef UNITMODEL_H
#define UNITMODEL_H

#include <QAbstractListModel>
#include <vector>
#include "units/Unit.h"

class UnitModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum UnitRoles {
        TypeRole = Qt::UserRole + 1,
        OwnerRole,
        XRole,
        YRole,
        IdRole, // Unikátní ID pro animaci
        HealthRole,
        MaxHealthRole,
        IsDefendingRole,

    };

    explicit UnitModel(QObject *parent = nullptr);

    // Funkce pro aktualizaci seznamu jednotek
    void setUnits(const std::vector<Unit*>& units);
    void refresh(); // Donutí QML překreslit data

    // Povinné funkce QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    std::vector<Unit*> m_units;
};

#endif // UNITMODEL_H
