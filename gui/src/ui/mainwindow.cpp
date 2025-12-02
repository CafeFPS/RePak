#include "mainwindow.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QLabel>
#include <QVBoxLayout>
#include <QApplication>
#include <QDirIterator>

#include "core/project.h"
#include "core/buildmanager.h"
#include "core/settings.h"
#include "widgets/assetbrowser.h"
#include "widgets/assetfilterwidget.h"
#include "widgets/buildlogwidget.h"
#include "widgets/buildprogresswidget.h"
#include "widgets/projectdashboard.h"
#include "tools/guidcalculatordialog.h"
#include "tools/uimghashdialog.h"
#include "tools/pakinspectordialog.h"
#include "tools/batchoperationsdialog.h"
#include "tools/validationreportdialog.h"
#include "ui/preferencesdialog.h"
#include "ui/aboutdialog.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_project(new Project(this))
    , m_buildManager(new BuildManager(this))
    , m_autoSaveTimer(new QTimer(this))
{
    setupUi();
    setupMenus();
    setupToolBar();
    setupStatusBar();

    loadSettings();
    updateWindowTitle();
    updateMenuState();

    // Connect project signals
    connect(m_project, &Project::modifiedChanged, this, &MainWindow::onProjectModified);

    // Connect build manager signals
    connect(m_buildManager, &BuildManager::buildStarted, this, &MainWindow::onBuildStarted);
    connect(m_buildManager, &BuildManager::buildProgress, this, &MainWindow::onBuildProgress);
    connect(m_buildManager, &BuildManager::buildFinished, this, &MainWindow::onBuildFinished);
    connect(m_buildManager, &BuildManager::buildOutput, this, &MainWindow::onBuildOutput);
    connect(m_buildManager, &BuildManager::buildError, m_buildLog, &BuildLogWidget::addError);
    connect(m_buildManager, &BuildManager::buildWarning, m_buildLog, &BuildLogWidget::addWarning);

    // Auto-save timer
    connect(m_autoSaveTimer, &QTimer::timeout, this, &MainWindow::autoSave);
    int interval = Settings::instance().autoSaveInterval();
    if (interval > 0) {
        m_autoSaveTimer->start(interval * 60 * 1000);
    }

    // Accept drops
    setAcceptDrops(true);

    // Load last project if configured
    if (Settings::instance().rememberLastProject()) {
        QString lastProject = Settings::instance().lastProjectPath();
        if (!lastProject.isEmpty() && QFile::exists(lastProject)) {
            openProject(lastProject);
        }
    }
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupUi()
{
    setWindowTitle("RePak GUI");
    setMinimumSize(1024, 768);

    // Create central widget with splitters
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Build progress at top
    m_buildProgress = new BuildProgressWidget(this);
    m_buildProgress->setVisible(false);
    layout->addWidget(m_buildProgress);

    // Main splitter (horizontal)
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);

    // Left panel: filter + asset browser
    QWidget* leftPanel = new QWidget(this);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);

    m_filterWidget = new AssetFilterWidget(this);
    m_assetBrowser = new AssetBrowser(this);

    leftLayout->addWidget(m_filterWidget);
    leftLayout->addWidget(m_assetBrowser, 1);

    // Right panel: editor area
    m_editorStack = new QStackedWidget(this);

    // Dashboard (shown when no asset selected)
    m_dashboard = new ProjectDashboard(this);
    m_editorStack->addWidget(m_dashboard);

    // Placeholder for editors
    QLabel* placeholder = new QLabel("Select an asset to edit", this);
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet("color: #888; font-size: 14px;");
    m_editorStack->addWidget(placeholder);

    m_mainSplitter->addWidget(leftPanel);
    m_mainSplitter->addWidget(m_editorStack);
    m_mainSplitter->setStretchFactor(0, 1);
    m_mainSplitter->setStretchFactor(1, 2);
    m_mainSplitter->setSizes({350, 750});

    layout->addWidget(m_mainSplitter, 1);

    // Build log at bottom
    m_buildLog = new BuildLogWidget(this);
    m_buildLog->setMaximumHeight(200);

    m_leftSplitter = new QSplitter(Qt::Vertical, this);
    m_leftSplitter->addWidget(m_mainSplitter);
    m_leftSplitter->addWidget(m_buildLog);
    m_leftSplitter->setStretchFactor(0, 4);
    m_leftSplitter->setStretchFactor(1, 1);

    layout->addWidget(m_leftSplitter);

    setCentralWidget(centralWidget);

    // Connect filter to browser
    connect(m_filterWidget, &AssetFilterWidget::filterChanged,
            m_assetBrowser, &AssetBrowser::setFilter);
    connect(m_filterWidget, &AssetFilterWidget::typeFilterChanged,
            m_assetBrowser, &AssetBrowser::setTypeFilter);
    connect(m_filterWidget, &AssetFilterWidget::searchTextChanged,
            m_assetBrowser, &AssetBrowser::setSearchText);

    // Connect browser to editor
    connect(m_assetBrowser, &AssetBrowser::assetSelected,
            this, [this](int index) {
                if (index >= 0) {
                    showEditor(m_project->assetAt(index));
                } else {
                    m_editorStack->setCurrentIndex(0);
                }
            });

    connect(m_assetBrowser, &AssetBrowser::selectionChanged,
            this, &MainWindow::onAssetSelectionChanged);

    // Set project on browser
    m_assetBrowser->setProject(m_project);
    m_dashboard->setProject(m_project);
}

void MainWindow::setupMenus()
{
    // File menu
    m_fileMenu = menuBar()->addMenu(tr("&File"));

    m_newAction = m_fileMenu->addAction(tr("&New Project"), this, &MainWindow::newProject);
    m_newAction->setShortcut(QKeySequence::New);

    m_openAction = m_fileMenu->addAction(tr("&Open Project..."), this, &MainWindow::openProjectDialog);
    m_openAction->setShortcut(QKeySequence::Open);

    m_recentProjectsMenu = m_fileMenu->addMenu(tr("Recent Projects"));
    updateRecentProjectsMenu();

    m_fileMenu->addSeparator();

    m_saveAction = m_fileMenu->addAction(tr("&Save"), this, &MainWindow::saveProject);
    m_saveAction->setShortcut(QKeySequence::Save);

    m_saveAsAction = m_fileMenu->addAction(tr("Save &As..."), this, &MainWindow::saveProjectAs);
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);

    m_fileMenu->addSeparator();

    m_closeAction = m_fileMenu->addAction(tr("&Close Project"), this, &MainWindow::closeProject);
    m_closeAction->setShortcut(QKeySequence::Close);

    m_fileMenu->addSeparator();

    m_fileMenu->addAction(tr("E&xit"), this, &QMainWindow::close);

    // Edit menu
    m_editMenu = menuBar()->addMenu(tr("&Edit"));

    m_addAssetsAction = m_editMenu->addAction(tr("&Add Assets..."), this, &MainWindow::addAssets);
    m_addAssetsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_A));

    m_addFolderAction = m_editMenu->addAction(tr("Add Assets from &Folder..."), this, &MainWindow::addAssetsFromFolder);

    m_editMenu->addSeparator();

    m_removeAssetsAction = m_editMenu->addAction(tr("&Remove Selected"), this, &MainWindow::removeSelectedAssets);
    m_removeAssetsAction->setShortcut(QKeySequence::Delete);

    m_editMenu->addSeparator();

    m_selectAllAction = m_editMenu->addAction(tr("Select &All"), this, &MainWindow::selectAllAssets);
    m_selectAllAction->setShortcut(QKeySequence::SelectAll);

    m_editMenu->addAction(tr("&Deselect All"), this, &MainWindow::deselectAllAssets);

    // View menu
    m_viewMenu = menuBar()->addMenu(tr("&View"));

    QAction* toggleBrowser = m_viewMenu->addAction(tr("Asset &Browser"));
    toggleBrowser->setCheckable(true);
    toggleBrowser->setChecked(true);
    connect(toggleBrowser, &QAction::toggled, this, &MainWindow::toggleAssetBrowser);

    QAction* toggleLog = m_viewMenu->addAction(tr("Build &Log"));
    toggleLog->setCheckable(true);
    toggleLog->setChecked(true);
    connect(toggleLog, &QAction::toggled, this, &MainWindow::toggleBuildLog);

    m_viewMenu->addSeparator();

    QAction* toggleStatus = m_viewMenu->addAction(tr("&Status Bar"));
    toggleStatus->setCheckable(true);
    toggleStatus->setChecked(true);
    connect(toggleStatus, &QAction::toggled, this, &MainWindow::toggleStatusBar);

    // Build menu
    m_buildMenu = menuBar()->addMenu(tr("&Build"));

    m_buildAction = m_buildMenu->addAction(tr("&Build"), this, &MainWindow::buildProject);
    m_buildAction->setShortcut(QKeySequence(Qt::Key_F7));

    m_buildCompressAction = m_buildMenu->addAction(tr("Build && &Compress"), this, &MainWindow::buildAndCompress);
    m_buildCompressAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F7));

    m_quickBuildAction = m_buildMenu->addAction(tr("&Quick Build (No Compress)"), this, &MainWindow::quickBuild);
    m_quickBuildAction->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F7));

    m_buildMenu->addSeparator();

    m_cancelBuildAction = m_buildMenu->addAction(tr("C&ancel Build"), this, &MainWindow::cancelBuild);
    m_cancelBuildAction->setEnabled(false);

    m_buildMenu->addSeparator();

    m_buildMenu->addAction(tr("Compress PA&K..."), this, &MainWindow::compressPak);
    m_buildMenu->addAction(tr("&Decompress PAK..."), this, &MainWindow::decompressPak);

    // Tools menu
    m_toolsMenu = menuBar()->addMenu(tr("&Tools"));

    m_guidCalcAction = m_toolsMenu->addAction(tr("&GUID Calculator..."), this, &MainWindow::showGuidCalculator);
    m_uimgHashAction = m_toolsMenu->addAction(tr("&UI Image Hash Calculator..."), this, &MainWindow::showUimgHashCalculator);

    m_toolsMenu->addSeparator();

    m_pakInspectorAction = m_toolsMenu->addAction(tr("PAK &Inspector..."), this, &MainWindow::showPakInspector);

    m_toolsMenu->addSeparator();

    m_batchOpsAction = m_toolsMenu->addAction(tr("&Batch Operations..."), this, &MainWindow::showBatchOperations);
    m_validationAction = m_toolsMenu->addAction(tr("&Validation Report..."), this, &MainWindow::showValidationReport);

    // Help menu
    m_helpMenu = menuBar()->addMenu(tr("&Help"));

    m_helpMenu->addAction(tr("&Documentation"), this, &MainWindow::showDocumentation);

    m_helpMenu->addSeparator();

    m_helpMenu->addAction(tr("&Preferences..."), this, &MainWindow::showPreferences);

    m_helpMenu->addSeparator();

    m_helpMenu->addAction(tr("&About RePak GUI..."), this, &MainWindow::showAbout);
}

void MainWindow::setupToolBar()
{
    m_mainToolBar = addToolBar(tr("Main Toolbar"));
    m_mainToolBar->setMovable(false);

    m_mainToolBar->addAction(m_newAction);
    m_mainToolBar->addAction(m_openAction);
    m_mainToolBar->addAction(m_saveAction);

    m_mainToolBar->addSeparator();

    m_mainToolBar->addAction(m_addAssetsAction);
    m_mainToolBar->addAction(m_removeAssetsAction);

    m_mainToolBar->addSeparator();

    m_mainToolBar->addAction(m_buildAction);
    m_mainToolBar->addAction(m_cancelBuildAction);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::setupDockWidgets()
{
    // Reserved for future dock widgets
}

void MainWindow::loadSettings()
{
    Settings& settings = Settings::instance();

    if (settings.mainWindowMaximized()) {
        showMaximized();
    } else {
        QSize size = settings.mainWindowSize();
        if (size.isValid()) {
            resize(size);
        }
        QPoint pos = settings.mainWindowPos();
        if (pos.x() >= 0 && pos.y() >= 0) {
            move(pos);
        }
    }

    if (!settings.mainWindowState().isEmpty()) {
        restoreState(settings.mainWindowState());
    }

    if (!settings.splitterState().isEmpty()) {
        m_mainSplitter->restoreState(settings.splitterState());
    }
}

void MainWindow::saveSettings()
{
    Settings& settings = Settings::instance();

    settings.setMainWindowMaximized(isMaximized());
    if (!isMaximized()) {
        settings.setMainWindowSize(size());
        settings.setMainWindowPos(pos());
    }
    settings.setMainWindowState(saveState());
    settings.setSplitterState(m_mainSplitter->saveState());

    if (m_project->hasFile()) {
        settings.setLastProjectPath(m_project->filePath());
    }

    settings.save();
}

void MainWindow::updateWindowTitle()
{
    QString title = "RePak GUI";

    if (m_project->hasFile()) {
        title = m_project->fileName() + " - " + title;
    } else if (m_project->assetCount() > 0) {
        title = "Untitled - " + title;
    }

    if (m_project->isModified()) {
        title = "* " + title;
    }

    setWindowTitle(title);
}

void MainWindow::updateRecentProjectsMenu()
{
    m_recentProjectsMenu->clear();

    QStringList recent = Settings::instance().recentProjects();
    for (const QString& path : recent) {
        QAction* action = m_recentProjectsMenu->addAction(path);
        connect(action, &QAction::triggered, this, &MainWindow::openRecentProject);
    }

    if (!recent.isEmpty()) {
        m_recentProjectsMenu->addSeparator();
        m_recentProjectsMenu->addAction(tr("Clear Recent"), this, &MainWindow::clearRecentProjects);
    }

    m_recentProjectsMenu->setEnabled(!recent.isEmpty());
}

void MainWindow::updateMenuState()
{
    bool hasProject = m_project->assetCount() > 0 || m_project->hasFile();
    bool hasSelection = m_assetBrowser && m_assetBrowser->selectedCount() > 0;
    bool isBuilding = m_buildManager->isBuilding();

    m_saveAction->setEnabled(hasProject);
    m_saveAsAction->setEnabled(hasProject);
    m_closeAction->setEnabled(hasProject);

    m_addAssetsAction->setEnabled(!isBuilding);
    m_addFolderAction->setEnabled(!isBuilding);
    m_removeAssetsAction->setEnabled(hasSelection && !isBuilding);
    m_selectAllAction->setEnabled(hasProject);

    m_buildAction->setEnabled(hasProject && !isBuilding);
    m_buildCompressAction->setEnabled(hasProject && !isBuilding);
    m_quickBuildAction->setEnabled(hasProject && !isBuilding);
    m_cancelBuildAction->setEnabled(isBuilding);

    m_validationAction->setEnabled(hasProject);
}

bool MainWindow::confirmClose()
{
    if (m_project->isModified()) {
        return confirmDiscardChanges();
    }
    return true;
}

bool MainWindow::confirmDiscardChanges()
{
    QMessageBox::StandardButton result = QMessageBox::question(
        this,
        tr("Unsaved Changes"),
        tr("The project has been modified. Do you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save
    );

    if (result == QMessageBox::Save) {
        saveProject();
        return !m_project->isModified();
    } else if (result == QMessageBox::Discard) {
        return true;
    }

    return false;
}

void MainWindow::showEditor(const Asset& asset)
{
    // For now, just show placeholder
    // TODO: Implement actual editors
    m_editorStack->setCurrentIndex(1);
}

void MainWindow::clearEditor()
{
    m_editorStack->setCurrentIndex(0);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (m_buildManager->isBuilding()) {
        QMessageBox::warning(this, tr("Build in Progress"),
                            tr("Cannot close while a build is in progress. Please cancel the build first."));
        event->ignore();
        return;
    }

    if (confirmClose()) {
        saveSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();

        for (const QUrl& url : urls) {
            QString filePath = url.toLocalFile();

            if (filePath.endsWith(".json", Qt::CaseInsensitive)) {
                openProject(filePath);
                break;
            }
        }
    }
}

void MainWindow::openProject(const QString& filePath)
{
    if (!confirmClose()) {
        return;
    }

    if (m_project->load(filePath)) {
        Settings::instance().addRecentProject(filePath);
        Settings::instance().setLastOpenDir(QFileInfo(filePath).absolutePath());
        updateRecentProjectsMenu();
        updateWindowTitle();
        updateMenuState();
        m_assetBrowser->refresh();
        m_dashboard->refresh();
        m_buildLog->clear();
        statusBar()->showMessage(tr("Project loaded: %1").arg(filePath), 3000);
    } else {
        QMessageBox::critical(this, tr("Error"),
                             tr("Failed to load project: %1").arg(filePath));
    }
}

// File menu slots
void MainWindow::newProject()
{
    if (!confirmClose()) {
        return;
    }

    m_project->clear();
    updateWindowTitle();
    updateMenuState();
    m_assetBrowser->refresh();
    m_dashboard->refresh();
    m_buildLog->clear();
}

void MainWindow::openProjectDialog()
{
    QString dir = Settings::instance().lastOpenDir();
    if (dir.isEmpty()) {
        dir = Settings::instance().defaultProjectsDir();
    }

    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open Project"),
        dir,
        tr("RePak Project (*.json);;All Files (*)")
    );

    if (!filePath.isEmpty()) {
        openProject(filePath);
    }
}

void MainWindow::openRecentProject()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        openProject(action->text());
    }
}

void MainWindow::clearRecentProjects()
{
    Settings::instance().clearRecentProjects();
    updateRecentProjectsMenu();
}

void MainWindow::saveProject()
{
    if (m_project->hasFile()) {
        if (m_project->save()) {
            updateWindowTitle();
            statusBar()->showMessage(tr("Project saved"), 3000);
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to save project"));
        }
    } else {
        saveProjectAs();
    }
}

void MainWindow::saveProjectAs()
{
    QString dir = Settings::instance().lastSaveDir();
    if (dir.isEmpty()) {
        dir = Settings::instance().defaultProjectsDir();
    }

    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Save Project As"),
        dir,
        tr("RePak Project (*.json);;All Files (*)")
    );

    if (!filePath.isEmpty()) {
        if (!filePath.endsWith(".json", Qt::CaseInsensitive)) {
            filePath += ".json";
        }

        if (m_project->saveAs(filePath)) {
            Settings::instance().addRecentProject(filePath);
            Settings::instance().setLastSaveDir(QFileInfo(filePath).absolutePath());
            updateRecentProjectsMenu();
            updateWindowTitle();
            statusBar()->showMessage(tr("Project saved: %1").arg(filePath), 3000);
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to save project"));
        }
    }
}

void MainWindow::closeProject()
{
    if (confirmClose()) {
        m_project->clear();
        updateWindowTitle();
        updateMenuState();
        m_assetBrowser->refresh();
        m_dashboard->refresh();
        m_buildLog->clear();
        clearEditor();
    }
}

// Edit menu slots
void MainWindow::addAssets()
{
    QString dir = m_project->hasFile() ? m_project->absoluteAssetsDir() :
                                         Settings::instance().lastOpenDir();

    QStringList files = QFileDialog::getOpenFileNames(
        this,
        tr("Add Assets"),
        dir,
        tr("All Supported Files (*.rpak *.dds *.vtf *.rmdl *.csv *.json *.cso *.msw);;All Files (*)")
    );

    if (files.isEmpty()) {
        return;
    }

    for (const QString& file : files) {
        QFileInfo fi(file);
        QString ext = fi.suffix().toLower();
        QString type;

        // Determine type from extension
        if (ext == "dds" || ext == "vtf") {
            type = "txtr";
        } else if (ext == "rmdl") {
            type = "mdl_";
        } else if (ext == "csv") {
            type = "dtbl";
        } else if (ext == "cso") {
            type = "shdr";
        } else if (ext == "msw") {
            type = "shds";
        } else if (ext == "rpak") {
            // Need to detect from path
            QString path = file.toLower();
            if (path.contains("texture")) type = "txtr";
            else if (path.contains("material")) type = "matl";
            else if (path.contains("shader") && path.contains("set")) type = "shds";
            else if (path.contains("shader")) type = "shdr";
            else if (path.contains("mdl")) type = "mdl_";
            else type = "txtr"; // Default
        }

        if (!type.isEmpty()) {
            QString relativePath = file;
            if (m_project->hasFile()) {
                QString assetsDir = m_project->absoluteAssetsDir();
                if (file.startsWith(assetsDir)) {
                    relativePath = file.mid(assetsDir.length());
                    if (relativePath.startsWith('/') || relativePath.startsWith('\\')) {
                        relativePath = relativePath.mid(1);
                    }
                }
            }

            Asset asset(type, relativePath);
            m_project->addAsset(asset);
        }
    }

    m_assetBrowser->refresh();
    m_dashboard->refresh();
    updateMenuState();
}

void MainWindow::addAssetsFromFolder()
{
    QString dir = m_project->hasFile() ? m_project->absoluteAssetsDir() :
                                         Settings::instance().lastOpenDir();

    QString folder = QFileDialog::getExistingDirectory(
        this,
        tr("Add Assets from Folder"),
        dir
    );

    if (folder.isEmpty()) {
        return;
    }

    // Recursively find assets
    QDir directory(folder);
    QStringList filters = {"*.rpak", "*.dds", "*.vtf", "*.rmdl", "*.csv", "*.json", "*.cso", "*.msw"};
    QStringList files = directory.entryList(filters, QDir::Files, QDir::Name);

    QDirIterator it(folder, filters, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString file = it.next();
        // Same logic as addAssets for each file
        QFileInfo fi(file);
        QString ext = fi.suffix().toLower();
        QString type;

        if (ext == "dds" || ext == "vtf") type = "txtr";
        else if (ext == "rmdl") type = "mdl_";
        else if (ext == "csv") type = "dtbl";
        else if (ext == "cso") type = "shdr";
        else if (ext == "msw") type = "shds";
        else if (ext == "rpak") {
            QString path = file.toLower();
            if (path.contains("texture")) type = "txtr";
            else if (path.contains("material")) type = "matl";
            else if (path.contains("shader") && path.contains("set")) type = "shds";
            else if (path.contains("shader")) type = "shdr";
            else if (path.contains("mdl")) type = "mdl_";
            else continue;
        }

        if (!type.isEmpty()) {
            QString relativePath = file;
            if (m_project->hasFile()) {
                QString assetsDir = m_project->absoluteAssetsDir();
                if (file.startsWith(assetsDir)) {
                    relativePath = file.mid(assetsDir.length());
                    if (relativePath.startsWith('/') || relativePath.startsWith('\\')) {
                        relativePath = relativePath.mid(1);
                    }
                }
            }

            Asset asset(type, relativePath);
            m_project->addAsset(asset);
        }
    }

    m_assetBrowser->refresh();
    m_dashboard->refresh();
    updateMenuState();
}

void MainWindow::removeSelectedAssets()
{
    QVector<int> selected = m_assetBrowser->selectedIndices();
    if (selected.isEmpty()) {
        return;
    }

    if (Settings::instance().confirmDialogs()) {
        QMessageBox::StandardButton result = QMessageBox::question(
            this,
            tr("Remove Assets"),
            tr("Are you sure you want to remove %1 asset(s)?").arg(selected.size()),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

        if (result != QMessageBox::Yes) {
            return;
        }
    }

    m_project->removeAssets(selected);
    m_assetBrowser->refresh();
    m_dashboard->refresh();
    updateMenuState();
    clearEditor();
}

void MainWindow::selectAllAssets()
{
    m_assetBrowser->selectAll();
}

void MainWindow::deselectAllAssets()
{
    m_assetBrowser->clearSelection();
}

// Build menu slots
void MainWindow::buildProject()
{
    QString repakPath = Settings::instance().repakPath();
    if (repakPath.isEmpty() || !QFile::exists(repakPath)) {
        QMessageBox::warning(this, tr("RePak Not Found"),
                            tr("Please configure the RePak executable path in Preferences."));
        showPreferences();
        return;
    }

    m_buildManager->build(m_project, repakPath);
}

void MainWindow::buildAndCompress()
{
    m_project->setCompressLevel(Settings::instance().defaultCompressLevel());
    m_project->setCompressWorkers(Settings::instance().defaultCompressWorkers());
    buildProject();
}

void MainWindow::quickBuild()
{
    int savedLevel = m_project->compressLevel();
    m_project->setCompressLevel(0);
    buildProject();
    m_project->setCompressLevel(savedLevel);
}

void MainWindow::cancelBuild()
{
    m_buildManager->cancel();
}

void MainWindow::compressPak()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Select PAK to Compress"),
        Settings::instance().lastOpenDir(),
        tr("RPak Files (*.rpak);;All Files (*)")
    );

    if (filePath.isEmpty()) {
        return;
    }

    QString repakPath = Settings::instance().repakPath();
    if (repakPath.isEmpty() || !QFile::exists(repakPath)) {
        QMessageBox::warning(this, tr("RePak Not Found"),
                            tr("Please configure the RePak executable path in Preferences."));
        return;
    }

    m_buildManager->compress(filePath, repakPath,
                            Settings::instance().defaultCompressLevel(),
                            Settings::instance().defaultCompressWorkers());
}

void MainWindow::decompressPak()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Select PAK to Decompress"),
        Settings::instance().lastOpenDir(),
        tr("RPak Files (*.rpak);;All Files (*)")
    );

    if (filePath.isEmpty()) {
        return;
    }

    QString repakPath = Settings::instance().repakPath();
    if (repakPath.isEmpty() || !QFile::exists(repakPath)) {
        QMessageBox::warning(this, tr("RePak Not Found"),
                            tr("Please configure the RePak executable path in Preferences."));
        return;
    }

    m_buildManager->decompress(filePath, repakPath);
}

// Tool menu slots
void MainWindow::showGuidCalculator()
{
    GuidCalculatorDialog dialog(this);
    dialog.exec();
}

void MainWindow::showUimgHashCalculator()
{
    UimgHashDialog dialog(this);
    dialog.exec();
}

void MainWindow::showPakInspector()
{
    PakInspectorDialog dialog(this);
    dialog.exec();
}

void MainWindow::showBatchOperations()
{
    BatchOperationsDialog dialog(this);
    dialog.exec();
}

void MainWindow::showValidationReport()
{
    ValidationReportDialog dialog(m_project, this);
    dialog.exec();
}

// View menu slots
void MainWindow::toggleAssetBrowser()
{
    // Toggle visibility of asset browser panel
}

void MainWindow::toggleBuildLog()
{
    m_buildLog->setVisible(!m_buildLog->isVisible());
}

void MainWindow::toggleStatusBar()
{
    statusBar()->setVisible(!statusBar()->isVisible());
}

// Help menu slots
void MainWindow::showPreferences()
{
    PreferencesDialog dialog(this);
    dialog.exec();
}

void MainWindow::showAbout()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::showDocumentation()
{
    // TODO: Open documentation
}

// Project signals
void MainWindow::onProjectModified(bool modified)
{
    Q_UNUSED(modified)
    updateWindowTitle();
    updateMenuState();
}

void MainWindow::onAssetSelectionChanged()
{
    updateMenuState();
}

// Build signals
void MainWindow::onBuildStarted()
{
    m_buildProgress->setVisible(true);
    m_buildProgress->reset();
    m_buildLog->clear();
    m_buildLog->addInfo("Build started...");
    updateMenuState();
    statusBar()->showMessage(tr("Building..."));
}

void MainWindow::onBuildProgress(int current, int total, const QString& operation)
{
    m_buildProgress->setProgress(current, total);
    m_buildProgress->setOperation(operation);
}

void MainWindow::onBuildFinished(BuildStatus status)
{
    m_buildProgress->setVisible(false);
    updateMenuState();

    QString message;
    switch (status) {
        case BuildStatus::Success:
            message = tr("Build succeeded");
            m_buildLog->addInfo("Build completed successfully");
            break;
        case BuildStatus::Failed:
            message = tr("Build failed");
            m_buildLog->addError("Build failed");
            break;
        case BuildStatus::Cancelled:
            message = tr("Build cancelled");
            m_buildLog->addWarning("Build cancelled by user");
            break;
        default:
            message = tr("Build finished");
            break;
    }

    statusBar()->showMessage(message, 5000);
}

void MainWindow::onBuildOutput(const QString& text)
{
    m_buildLog->addInfo(text);
}

void MainWindow::autoSave()
{
    if (m_project->isModified() && m_project->hasFile()) {
        m_project->save();
        statusBar()->showMessage(tr("Auto-saved"), 2000);
    }
}
