#include "assetlistmodel.h"

#include "core/project.h"

AssetListModel::AssetListModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_project(nullptr)
{
}

AssetListModel::~AssetListModel()
{
}

void AssetListModel::setProject(Project* project)
{
    beginResetModel();
    m_project = project;
    endResetModel();
}

void AssetListModel::refresh()
{
    beginResetModel();
    endResetModel();
}

int AssetListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() || !m_project) {
        return 0;
    }
    return m_project->assetCount();
}

QVariant AssetListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !m_project) {
        return QVariant();
    }

    int row = index.row();
    if (row < 0 || row >= m_project->assetCount()) {
        return QVariant();
    }

    const Asset asset = m_project->assetAt(row);

    switch (role) {
        case Qt::DisplayRole:
        case DisplayNameRole:
            return asset.displayName();

        case Qt::ToolTipRole:
            return QString("%1\nType: %2\nPath: %3")
                   .arg(asset.displayName())
                   .arg(asset.typeDisplayName())
                   .arg(asset.path());

        case TypeRole:
            return asset.type();

        case PathRole:
            return asset.path();

        case GuidRole:
            return asset.guid();

        case AssetTypeRole:
            return static_cast<int>(asset.assetType());

        default:
            return QVariant();
    }
}

QVariant AssetListModel::headerData(int section, Qt::Orientation orientation, int role) const
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

Qt::ItemFlags AssetListModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
