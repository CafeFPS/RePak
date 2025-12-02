#ifndef ASSETLISTMODEL_H
#define ASSETLISTMODEL_H

#include <QAbstractListModel>

#include "core/asset.h"

class Project;

/**
 * Model for displaying assets in a list view
 */
class AssetListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        TypeRole = Qt::UserRole + 1,
        PathRole,
        GuidRole,
        AssetTypeRole,
        DisplayNameRole
    };

    explicit AssetListModel(QObject* parent = nullptr);
    ~AssetListModel() override;

    void setProject(Project* project);
    Project* project() const { return m_project; }

    void refresh();

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    Project* m_project;
};

#endif // ASSETLISTMODEL_H
