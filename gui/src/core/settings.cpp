#include "settings.h"

#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>

Settings& Settings::instance()
{
    static Settings instance;
    return instance;
}

Settings::Settings()
    : m_mainWindowSize(1400, 900)
    , m_mainWindowPos(-1, -1)
    , m_mainWindowMaximized(false)
    , m_maxRecentProjects(10)
    , m_defaultCompressLevel(6)
    , m_defaultCompressWorkers(16)
    , m_autoCompressBuilds(true)
    , m_copyToGameAfterBuild(false)
    , m_texturePreviewSize(256)
    , m_showAssetPreviews(true)
    , m_enable3DPreview(true)
    , m_darkTheme(true)
    , m_confirmDialogs(true)
    , m_autoSaveInterval(5)
    , m_rememberLastProject(true)
{
    // Set default paths
    m_defaultProjectsDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/RePak Projects";

    // Try to find RePak executable
    QString appDir = QCoreApplication::applicationDirPath();
    QString repakInParent = QDir(appDir).absoluteFilePath("../bin/Release/repak.exe");
    QString repakInSame = QDir(appDir).absoluteFilePath("repak.exe");

    if (QFile::exists(repakInSame)) {
        m_repakPath = repakInSame;
    } else if (QFile::exists(repakInParent)) {
        m_repakPath = repakInParent;
    }
}

Settings::~Settings()
{
    save();
}

void Settings::load()
{
    QSettings settings("RePak", "RePakGUI");

    // Window
    m_mainWindowSize = settings.value("window/size", QSize(1400, 900)).toSize();
    m_mainWindowPos = settings.value("window/pos", QPoint(-1, -1)).toPoint();
    m_mainWindowMaximized = settings.value("window/maximized", false).toBool();
    m_mainWindowState = settings.value("window/state").toByteArray();
    m_splitterState = settings.value("window/splitter").toByteArray();

    // Paths
    m_repakPath = settings.value("paths/repak", m_repakPath).toString();
    m_gameDirectory = settings.value("paths/game").toString();
    m_defaultProjectsDir = settings.value("paths/projects", m_defaultProjectsDir).toString();
    m_lastOpenDir = settings.value("paths/lastOpen").toString();
    m_lastSaveDir = settings.value("paths/lastSave").toString();

    // Recent
    m_recentProjects = settings.value("recent/projects").toStringList();

    // Build
    m_defaultCompressLevel = settings.value("build/compressLevel", 6).toInt();
    m_defaultCompressWorkers = settings.value("build/compressWorkers", 16).toInt();
    m_autoCompressBuilds = settings.value("build/autoCompress", true).toBool();
    m_copyToGameAfterBuild = settings.value("build/copyToGame", false).toBool();

    // Editor
    m_texturePreviewSize = settings.value("editor/texturePreviewSize", 256).toInt();
    m_showAssetPreviews = settings.value("editor/showPreviews", true).toBool();
    m_enable3DPreview = settings.value("editor/3dPreview", true).toBool();

    // UI
    m_darkTheme = settings.value("ui/darkTheme", true).toBool();
    m_confirmDialogs = settings.value("ui/confirmDialogs", true).toBool();
    m_autoSaveInterval = settings.value("ui/autoSaveInterval", 5).toInt();
    m_rememberLastProject = settings.value("ui/rememberLastProject", true).toBool();
    m_lastProjectPath = settings.value("ui/lastProject").toString();
}

void Settings::save()
{
    QSettings settings("RePak", "RePakGUI");

    // Window
    settings.setValue("window/size", m_mainWindowSize);
    settings.setValue("window/pos", m_mainWindowPos);
    settings.setValue("window/maximized", m_mainWindowMaximized);
    settings.setValue("window/state", m_mainWindowState);
    settings.setValue("window/splitter", m_splitterState);

    // Paths
    settings.setValue("paths/repak", m_repakPath);
    settings.setValue("paths/game", m_gameDirectory);
    settings.setValue("paths/projects", m_defaultProjectsDir);
    settings.setValue("paths/lastOpen", m_lastOpenDir);
    settings.setValue("paths/lastSave", m_lastSaveDir);

    // Recent
    settings.setValue("recent/projects", m_recentProjects);

    // Build
    settings.setValue("build/compressLevel", m_defaultCompressLevel);
    settings.setValue("build/compressWorkers", m_defaultCompressWorkers);
    settings.setValue("build/autoCompress", m_autoCompressBuilds);
    settings.setValue("build/copyToGame", m_copyToGameAfterBuild);

    // Editor
    settings.setValue("editor/texturePreviewSize", m_texturePreviewSize);
    settings.setValue("editor/showPreviews", m_showAssetPreviews);
    settings.setValue("editor/3dPreview", m_enable3DPreview);

    // UI
    settings.setValue("ui/darkTheme", m_darkTheme);
    settings.setValue("ui/confirmDialogs", m_confirmDialogs);
    settings.setValue("ui/autoSaveInterval", m_autoSaveInterval);
    settings.setValue("ui/rememberLastProject", m_rememberLastProject);
    settings.setValue("ui/lastProject", m_lastProjectPath);
}

void Settings::reset()
{
    QSettings settings("RePak", "RePakGUI");
    settings.clear();
    // Reset to defaults
    m_mainWindowSize = QSize(1280, 800);
    m_mainWindowPos = QPoint(100, 100);
    m_mainWindowMaximized = false;
    m_mainWindowState.clear();
    m_splitterState.clear();
    m_repakPath.clear();
    m_gameDirectory.clear();
    m_defaultProjectsDir.clear();
    m_lastOpenDir.clear();
    m_recentProjects.clear();
    m_darkTheme = true;
    m_confirmDialogs = true;
    m_autoSaveInterval = 300;
    m_rememberLastProject = true;
    m_lastProjectPath.clear();
}

void Settings::setMainWindowSize(const QSize& size)
{
    m_mainWindowSize = size;
}

void Settings::setMainWindowPos(const QPoint& pos)
{
    m_mainWindowPos = pos;
}

void Settings::setMainWindowMaximized(bool maximized)
{
    m_mainWindowMaximized = maximized;
}

void Settings::setMainWindowState(const QByteArray& state)
{
    m_mainWindowState = state;
}

void Settings::setSplitterState(const QByteArray& state)
{
    m_splitterState = state;
}

void Settings::setRepakPath(const QString& path)
{
    m_repakPath = path;
}

void Settings::setGameDirectory(const QString& path)
{
    m_gameDirectory = path;
}

void Settings::setDefaultProjectsDir(const QString& path)
{
    m_defaultProjectsDir = path;
}

void Settings::setLastOpenDir(const QString& path)
{
    m_lastOpenDir = path;
}

void Settings::setLastSaveDir(const QString& path)
{
    m_lastSaveDir = path;
}

void Settings::addRecentProject(const QString& path)
{
    m_recentProjects.removeAll(path);
    m_recentProjects.prepend(path);

    while (m_recentProjects.size() > m_maxRecentProjects) {
        m_recentProjects.removeLast();
    }
}

void Settings::clearRecentProjects()
{
    m_recentProjects.clear();
}

void Settings::setDefaultCompressLevel(int level)
{
    m_defaultCompressLevel = qBound(-5, level, 22);
}

void Settings::setDefaultCompressWorkers(int workers)
{
    m_defaultCompressWorkers = qBound(1, workers, 64);
}

void Settings::setAutoCompressBuilds(bool autoCompress)
{
    m_autoCompressBuilds = autoCompress;
}

void Settings::setCopyToGameAfterBuild(bool copy)
{
    m_copyToGameAfterBuild = copy;
}

void Settings::setTexturePreviewSize(int size)
{
    m_texturePreviewSize = qBound(64, size, 512);
}

void Settings::setShowAssetPreviews(bool show)
{
    m_showAssetPreviews = show;
}

void Settings::setEnable3DPreview(bool enable)
{
    m_enable3DPreview = enable;
}

void Settings::setDarkTheme(bool dark)
{
    m_darkTheme = dark;
}

void Settings::setConfirmDialogs(bool confirm)
{
    m_confirmDialogs = confirm;
}

void Settings::setAutoSaveInterval(int minutes)
{
    m_autoSaveInterval = qBound(1, minutes, 60);
}

void Settings::setRememberLastProject(bool remember)
{
    m_rememberLastProject = remember;
}

void Settings::setLastProjectPath(const QString& path)
{
    m_lastProjectPath = path;
}
