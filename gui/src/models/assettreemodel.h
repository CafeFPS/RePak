#ifndef ASSETTREEMODEL_H
#define ASSETTREEMODEL_H

#include <QAbstractItemModel>

class Project;

/**
 * Tree model for hierarchical asset display by folder
 * Reserved for future implementation
 */
class AssetTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit AssetTreeModel(QObject* parent = nullptr);
    ~AssetTreeModel() override;

    void setProject(Project* project);
    void refresh();

    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    Project* m_project;
};

#endif // ASSETTREEMODEL_H
