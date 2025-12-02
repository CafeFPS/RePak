#ifndef ASSETFILTERPROXYMODEL_H
#define ASSETFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QSet>

#include "core/asset.h"

/**
 * Filter proxy model for asset filtering by type and search
 */
class AssetFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit AssetFilterProxyModel(QObject* parent = nullptr);
    ~AssetFilterProxyModel() override;

    void setTypeFilter(const QSet<AssetType>& types);
    QSet<AssetType> typeFilter() const { return m_typeFilter; }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

private:
    QSet<AssetType> m_typeFilter;
};

#endif // ASSETFILTERPROXYMODEL_H
