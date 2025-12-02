#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QStackedWidget>
#include <QTimer>
#include <QMap>

#include "core/asset.h"
#include "core/buildmanager.h"

class Project;
class AssetBrowser;
class BuildLogWidget;
class BuildProgressWidget;
class ProjectDashboard;
class AssetEditorBase;
class AssetFilterWidget;

/**
 * Main application window
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    void openProject(const QString& filePath);

protected:
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    // File menu
    void newProject();
    void openProjectDialog();
    void openRecentProject();
    void clearRecentProjects();
    void saveProject();
    void saveProjectAs();
    void closeProject();

    // Edit menu
    void addAssets();
    void addAssetsFromFolder();
    void removeSelectedAssets();
    void selectAllAssets();
    void deselectAllAssets();

    // Build menu
    void buildProject();
    void buildAndCompress();
    void quickBuild();
    void cancelBuild();
    void compressPak();
    void decompressPak();

    // Tools menu
    void showGuidCalculator();
    void showUimgHashCalculator();
    void showPakInspector();
    void showBatchOperations();
    void showValidationReport();

    // View menu
    void toggleAssetBrowser();
    void toggleBuildLog();
    void toggleStatusBar();

    // Help menu
    void showPreferences();
    void showAbout();
    void showDocumentation();

    // Project signals
    void onProjectModified(bool modified);
    void onAssetSelectionChanged();

    // Build signals
    void onBuildStarted();
    void onBuildProgress(int current, int total, const QString& operation);
    void onBuildFinished(BuildStatus status);
    void onBuildOutput(const QString& text);

    // Auto-save
    void autoSave();

private:
    void setupUi();
    void setupMenus();
    void setupToolBar();
    void setupStatusBar();
    void setupCentralWidget();
    void setupDockWidgets();

    void loadSettings();
    void saveSettings();

    void updateWindowTitle();
    void updateRecentProjectsMenu();
    void updateMenuState();

    bool confirmClose();
    bool confirmDiscardChanges();

    void showEditor(const Asset& asset);
    void clearEditor();

private:
    // Core objects
    Project* m_project;
    BuildManager* m_buildManager;

    // Widgets
    QSplitter* m_mainSplitter;
    QSplitter* m_leftSplitter;
    QSplitter* m_rightSplitter;

    AssetFilterWidget* m_filterWidget;
    AssetBrowser* m_assetBrowser;
    QStackedWidget* m_editorStack;
    BuildLogWidget* m_buildLog;
    BuildProgressWidget* m_buildProgress;
    ProjectDashboard* m_dashboard;

    // Editors (lazy-loaded)
    QMap<AssetType, AssetEditorBase*> m_editors;

    // Menus
    QMenu* m_fileMenu;
    QMenu* m_recentProjectsMenu;
    QMenu* m_editMenu;
    QMenu* m_viewMenu;
    QMenu* m_buildMenu;
    QMenu* m_toolsMenu;
    QMenu* m_helpMenu;

    // Actions
    QAction* m_newAction;
    QAction* m_openAction;
    QAction* m_saveAction;
    QAction* m_saveAsAction;
    QAction* m_closeAction;

    QAction* m_addAssetsAction;
    QAction* m_addFolderAction;
    QAction* m_removeAssetsAction;
    QAction* m_selectAllAction;

    QAction* m_buildAction;
    QAction* m_buildCompressAction;
    QAction* m_quickBuildAction;
    QAction* m_cancelBuildAction;

    QAction* m_guidCalcAction;
    QAction* m_uimgHashAction;
    QAction* m_pakInspectorAction;
    QAction* m_batchOpsAction;
    QAction* m_validationAction;

    // Toolbar
    QToolBar* m_mainToolBar;

    // Timer
    QTimer* m_autoSaveTimer;
};

#endif // MAINWINDOW_H
