#ifndef ASSETBROWSER_H
#define ASSETBROWSER_H

#include <QWidget>
#include <QTreeView>
#include <QListView>
#include <QStackedWidget>
#include <QToolButton>
#include <QSet>

#include "core/asset.h"

class Project;
class AssetListModel;
class AssetFilterProxyModel;

/**
 * Asset browser widget with list/tree/grid views
 */
class AssetBrowser : public QWidget
{
    Q_OBJECT

public:
    enum ViewMode {
        ListView,
        TreeView,
        GridView
    };

    explicit AssetBrowser(QWidget* parent = nullptr);
    ~AssetBrowser() override;

    void setProject(Project* project);
    Project* project() const { return m_project; }

    void refresh();

    // Selection
    int selectedCount() const;
    QVector<int> selectedIndices() const;
    void selectAll();
    void clearSelection();

    // View mode
    ViewMode viewMode() const { return m_viewMode; }
    void setViewMode(ViewMode mode);

public slots:
    void setFilter(const QString& filter);
    void setTypeFilter(const QSet<AssetType>& types);
    void setSearchText(const QString& text);

signals:
    void assetSelected(int index);
    void assetDoubleClicked(int index);
    void selectionChanged();
    void contextMenuRequested(const QPoint& pos);

private slots:
    void onSelectionChanged();
    void onDoubleClicked(const QModelIndex& index);
    void onContextMenu(const QPoint& pos);
    void switchToListView();
    void switchToTreeView();
    void switchToGridView();

private:
    void setupUi();
    void updateViewMode();

private:
    Project* m_project;
    ViewMode m_viewMode;

    // Views
    QStackedWidget* m_viewStack;
    QListView* m_listView;
    QTreeView* m_treeView;
    QListView* m_gridView;

    // Model
    AssetListModel* m_model;
    AssetFilterProxyModel* m_proxyModel;

    // View mode buttons
    QToolButton* m_listButton;
    QToolButton* m_treeButton;
    QToolButton* m_gridButton;
};

#endif // ASSETBROWSER_H
