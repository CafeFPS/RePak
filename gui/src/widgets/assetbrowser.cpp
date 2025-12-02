#include "assetbrowser.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QLabel>

#include "core/project.h"
#include "models/assetlistmodel.h"
#include "models/assetfilterproxymodel.h"

AssetBrowser::AssetBrowser(QWidget* parent)
    : QWidget(parent)
    , m_project(nullptr)
    , m_viewMode(ListView)
{
    setupUi();
}

AssetBrowser::~AssetBrowser()
{
}

void AssetBrowser::setupUi()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    // View mode toolbar
    QWidget* toolbar = new QWidget(this);
    QHBoxLayout* toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setContentsMargins(4, 4, 4, 4);
    toolbarLayout->setSpacing(2);

    QLabel* label = new QLabel(tr("Assets"), this);
    label->setStyleSheet("font-weight: bold;");
    toolbarLayout->addWidget(label);

    toolbarLayout->addStretch();

    m_listButton = new QToolButton(this);
    m_listButton->setText("List");
    m_listButton->setCheckable(true);
    m_listButton->setChecked(true);
    m_listButton->setToolTip(tr("List View"));
    connect(m_listButton, &QToolButton::clicked, this, &AssetBrowser::switchToListView);

    m_treeButton = new QToolButton(this);
    m_treeButton->setText("Tree");
    m_treeButton->setCheckable(true);
    m_treeButton->setToolTip(tr("Tree View"));
    connect(m_treeButton, &QToolButton::clicked, this, &AssetBrowser::switchToTreeView);

    m_gridButton = new QToolButton(this);
    m_gridButton->setText("Grid");
    m_gridButton->setCheckable(true);
    m_gridButton->setToolTip(tr("Grid View"));
    connect(m_gridButton, &QToolButton::clicked, this, &AssetBrowser::switchToGridView);

    toolbarLayout->addWidget(m_listButton);
    toolbarLayout->addWidget(m_treeButton);
    toolbarLayout->addWidget(m_gridButton);

    layout->addWidget(toolbar);

    // Create model
    m_model = new AssetListModel(this);
    m_proxyModel = new AssetFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);

    // View stack
    m_viewStack = new QStackedWidget(this);

    // List view
    m_listView = new QListView(this);
    m_listView->setModel(m_proxyModel);
    m_listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_listView->setAlternatingRowColors(true);
    connect(m_listView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &AssetBrowser::onSelectionChanged);
    connect(m_listView, &QListView::doubleClicked, this, &AssetBrowser::onDoubleClicked);
    connect(m_listView, &QListView::customContextMenuRequested, this, &AssetBrowser::onContextMenu);
    m_viewStack->addWidget(m_listView);

    // Tree view
    m_treeView = new QTreeView(this);
    m_treeView->setModel(m_proxyModel);
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->header()->setStretchLastSection(true);
    connect(m_treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &AssetBrowser::onSelectionChanged);
    connect(m_treeView, &QTreeView::doubleClicked, this, &AssetBrowser::onDoubleClicked);
    connect(m_treeView, &QTreeView::customContextMenuRequested, this, &AssetBrowser::onContextMenu);
    m_viewStack->addWidget(m_treeView);

    // Grid view
    m_gridView = new QListView(this);
    m_gridView->setModel(m_proxyModel);
    m_gridView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_gridView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_gridView->setViewMode(QListView::IconMode);
    m_gridView->setIconSize(QSize(64, 64));
    m_gridView->setGridSize(QSize(100, 100));
    m_gridView->setWrapping(true);
    m_gridView->setResizeMode(QListView::Adjust);
    connect(m_gridView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &AssetBrowser::onSelectionChanged);
    connect(m_gridView, &QListView::doubleClicked, this, &AssetBrowser::onDoubleClicked);
    connect(m_gridView, &QListView::customContextMenuRequested, this, &AssetBrowser::onContextMenu);
    m_viewStack->addWidget(m_gridView);

    layout->addWidget(m_viewStack, 1);

    // Status label
    QLabel* statusLabel = new QLabel(this);
    statusLabel->setStyleSheet("color: #888; padding: 4px;");
    statusLabel->setObjectName("assetCountLabel");
    layout->addWidget(statusLabel);
}

void AssetBrowser::setProject(Project* project)
{
    m_project = project;
    m_model->setProject(project);
    refresh();
}

void AssetBrowser::refresh()
{
    m_model->refresh();

    // Update status
    QLabel* label = findChild<QLabel*>("assetCountLabel");
    if (label && m_project) {
        int total = m_project->assetCount();
        int visible = m_proxyModel->rowCount();
        if (total == visible) {
            label->setText(tr("%1 assets").arg(total));
        } else {
            label->setText(tr("%1 of %2 assets").arg(visible).arg(total));
        }
    }
}

int AssetBrowser::selectedCount() const
{
    QAbstractItemView* currentView = nullptr;
    switch (m_viewMode) {
        case ListView: currentView = m_listView; break;
        case TreeView: currentView = m_treeView; break;
        case GridView: currentView = m_gridView; break;
    }

    if (currentView) {
        return currentView->selectionModel()->selectedRows().size();
    }
    return 0;
}

QVector<int> AssetBrowser::selectedIndices() const
{
    QVector<int> indices;
    QAbstractItemView* currentView = nullptr;

    switch (m_viewMode) {
        case ListView: currentView = m_listView; break;
        case TreeView: currentView = m_treeView; break;
        case GridView: currentView = m_gridView; break;
    }

    if (currentView) {
        QModelIndexList selected = currentView->selectionModel()->selectedRows();
        for (const QModelIndex& proxyIndex : selected) {
            QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
            indices.append(sourceIndex.row());
        }
    }

    return indices;
}

void AssetBrowser::selectAll()
{
    QAbstractItemView* currentView = nullptr;
    switch (m_viewMode) {
        case ListView: currentView = m_listView; break;
        case TreeView: currentView = m_treeView; break;
        case GridView: currentView = m_gridView; break;
    }

    if (currentView) {
        currentView->selectAll();
    }
}

void AssetBrowser::clearSelection()
{
    QAbstractItemView* currentView = nullptr;
    switch (m_viewMode) {
        case ListView: currentView = m_listView; break;
        case TreeView: currentView = m_treeView; break;
        case GridView: currentView = m_gridView; break;
    }

    if (currentView) {
        currentView->clearSelection();
    }
}

void AssetBrowser::setViewMode(ViewMode mode)
{
    if (m_viewMode != mode) {
        m_viewMode = mode;
        updateViewMode();
    }
}

void AssetBrowser::setFilter(const QString& filter)
{
    m_proxyModel->setFilterWildcard(filter);
    refresh();
}

void AssetBrowser::setTypeFilter(const QSet<AssetType>& types)
{
    m_proxyModel->setTypeFilter(types);
    refresh();
}

void AssetBrowser::setSearchText(const QString& text)
{
    m_proxyModel->setFilterFixedString(text);
    refresh();
}

void AssetBrowser::onSelectionChanged()
{
    emit selectionChanged();

    QVector<int> selected = selectedIndices();
    if (selected.size() == 1) {
        emit assetSelected(selected.first());
    } else {
        emit assetSelected(-1);
    }
}

void AssetBrowser::onDoubleClicked(const QModelIndex& index)
{
    QModelIndex sourceIndex = m_proxyModel->mapToSource(index);
    emit assetDoubleClicked(sourceIndex.row());
}

void AssetBrowser::onContextMenu(const QPoint& pos)
{
    emit contextMenuRequested(pos);
}

void AssetBrowser::switchToListView()
{
    setViewMode(ListView);
    m_listButton->setChecked(true);
    m_treeButton->setChecked(false);
    m_gridButton->setChecked(false);
}

void AssetBrowser::switchToTreeView()
{
    setViewMode(TreeView);
    m_listButton->setChecked(false);
    m_treeButton->setChecked(true);
    m_gridButton->setChecked(false);
}

void AssetBrowser::switchToGridView()
{
    setViewMode(GridView);
    m_listButton->setChecked(false);
    m_treeButton->setChecked(false);
    m_gridButton->setChecked(true);
}

void AssetBrowser::updateViewMode()
{
    switch (m_viewMode) {
        case ListView:
            m_viewStack->setCurrentWidget(m_listView);
            break;
        case TreeView:
            m_viewStack->setCurrentWidget(m_treeView);
            break;
        case GridView:
            m_viewStack->setCurrentWidget(m_gridView);
            break;
    }
}
