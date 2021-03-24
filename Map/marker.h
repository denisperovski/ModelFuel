#ifndef MARKER_H
#define MARKER_H

#include <QAbstractListModel>
#include <QGeoCoordinate>

class MarkerModel : public QAbstractListModel//класс для добавления меток на карту, по сути просто заносит координаты в массив для сохранения
        //меток на карте
{
    Q_OBJECT

public:
    using QAbstractListModel::QAbstractListModel;
    enum MarkerRoles{positionRole = Qt::UserRole + 1};

    Q_INVOKABLE void addMarker(const QGeoCoordinate &coordinate)
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_coordinates.append(coordinate);
        endInsertRows();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent)
        return m_coordinates.count();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (index.row() < 0 || index.row() >= m_coordinates.count())
            return QVariant();
        if(role== MarkerModel::positionRole)
            return QVariant::fromValue(m_coordinates[index.row()]);
        return QVariant();
    }

    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[positionRole] = "position";
        return roles;
    }

    QList<QGeoCoordinate> m_coordinates;
};

#endif // MARKER_H
