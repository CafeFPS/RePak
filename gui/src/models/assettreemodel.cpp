#include "assettreemodel.h"

#include "core/project.h"

AssetTreeModel::AssetTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
    , m_project(nullptr)
{
}

AssetTreeModel::~AssetTreeModel()
{
}

void AssetTreeModel::setProject(Project* project)
{
    beginResetModel();
    m_project = project;
    endResetModel();
}

void AssetTreeModel::refresh()
{
    beginResetModel();
    endResetModel();
}

QModelIndex AssetTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    // Simplified implementation - treat as flat list for now
    if (!m_project || row < 0 || row >= m_project->assetCount() || column < 0 || column > 3) {
        return QModelIndex();
    }
    return createIndex(row, column, nullptr);
}

QModelIndex AssetTreeModel::parent(const QModelIndex& index) const
{
    Q_UNUSED(index)
    return QModelIndex(); // Flat list for now
}

int AssetTreeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() || !m_project) {
        return 0;
    }
    return m_project->assetCount();
}

int AssetTreeModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 4; // Name, Type, Path, GUID
}

QVariant AssetTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !m_project) {
        return QVariant();
    }

    int row = index.row();
    if (row < 0 || row >= m_project->assetCount()) {
        return QVariant();
    }

    const Asset asset = m_project->assetAt(row);

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return asset.displayName();
            case 1: return asset.typeDisplayName();
            case 2: return asset.path();
            case 3: return asset.guid();
        }
    }

    return QVariant();
}

QVariant AssetTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0: return tr("Name");
            case 1: return tr("Type");
            case 2: return tr("Path");
            case 3: return tr("GUID");
        }
    }
    return QVariant();
}
