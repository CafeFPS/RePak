#include "assetfilterproxymodel.h"

#include "assetlistmodel.h"

AssetFilterProxyModel::AssetFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

AssetFilterProxyModel::~AssetFilterProxyModel()
{
}

void AssetFilterProxyModel::setTypeFilter(const QSet<AssetType>& types)
{
    m_typeFilter = types;
    invalidateFilter();
}

bool AssetFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    // Check type filter
    if (!m_typeFilter.isEmpty()) {
        int typeInt = sourceModel()->data(index, AssetListModel::AssetTypeRole).toInt();
        AssetType type = static_cast<AssetType>(typeInt);
        if (!m_typeFilter.contains(type)) {
            return false;
        }
    }

    // Check text filter
    QString filterText = filterRegularExpression().pattern();
    if (!filterText.isEmpty()) {
        QString displayName = sourceModel()->data(index, AssetListModel::DisplayNameRole).toString();
        QString path = sourceModel()->data(index, AssetListModel::PathRole).toString();
        QString guid = sourceModel()->data(index, AssetListModel::GuidRole).toString();

        bool matches = displayName.contains(filterText, Qt::CaseInsensitive) ||
                      path.contains(filterText, Qt::CaseInsensitive) ||
                      guid.contains(filterText, Qt::CaseInsensitive);

        if (!matches) {
            return false;
        }
    }

    return true;
}

bool AssetFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    // Sort by type first, then by name
    int leftType = sourceModel()->data(left, AssetListModel::AssetTypeRole).toInt();
    int rightType = sourceModel()->data(right, AssetListModel::AssetTypeRole).toInt();

    if (leftType != rightType) {
        return leftType < rightType;
    }

    QString leftName = sourceModel()->data(left, AssetListModel::DisplayNameRole).toString();
    QString rightName = sourceModel()->data(right, AssetListModel::DisplayNameRole).toString();

    return QString::localeAwareCompare(leftName, rightName) < 0;
}
