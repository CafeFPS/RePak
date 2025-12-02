#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QStringList>
#include <QSize>
#include <QPoint>
#include <QByteArray>

/**
 * Application-wide settings manager (singleton)
 */
class Settings
{
public:
    static Settings& instance();

    void load();
    void save();
    void reset();

    // Window state
    QSize mainWindowSize() const { return m_mainWindowSize; }
    void setMainWindowSize(const QSize& size);

    QPoint mainWindowPos() const { return m_mainWindowPos; }
    void setMainWindowPos(const QPoint& pos);

    bool mainWindowMaximized() const { return m_mainWindowMaximized; }
    void setMainWindowMaximized(bool maximized);

    QByteArray mainWindowState() const { return m_mainWindowState; }
    void setMainWindowState(const QByteArray& state);

    QByteArray splitterState() const { return m_splitterState; }
    void setSplitterState(const QByteArray& state);

    // Paths
    QString repakPath() const { return m_repakPath; }
    void setRepakPath(const QString& path);

    QString gameDirectory() const { return m_gameDirectory; }
    void setGameDirectory(const QString& path);

    QString defaultProjectsDir() const { return m_defaultProjectsDir; }
    void setDefaultProjectsDir(const QString& path);

    QString lastOpenDir() const { return m_lastOpenDir; }
    void setLastOpenDir(const QString& path);

    QString lastSaveDir() const { return m_lastSaveDir; }
    void setLastSaveDir(const QString& path);

    // Recent files
    QStringList recentProjects() const { return m_recentProjects; }
    void addRecentProject(const QString& path);
    void clearRecentProjects();
    int maxRecentProjects() const { return m_maxRecentProjects; }

    // Build settings
    int defaultCompressLevel() const { return m_defaultCompressLevel; }
    void setDefaultCompressLevel(int level);

    int defaultCompressWorkers() const { return m_defaultCompressWorkers; }
    void setDefaultCompressWorkers(int workers);

    bool autoCompressBuilds() const { return m_autoCompressBuilds; }
    void setAutoCompressBuilds(bool autoCompress);

    bool copyToGameAfterBuild() const { return m_copyToGameAfterBuild; }
    void setCopyToGameAfterBuild(bool copy);

    // Editor settings
    int texturePreviewSize() const { return m_texturePreviewSize; }
    void setTexturePreviewSize(int size);

    bool showAssetPreviews() const { return m_showAssetPreviews; }
    void setShowAssetPreviews(bool show);

    bool enable3DPreview() const { return m_enable3DPreview; }
    void setEnable3DPreview(bool enable);

    // UI settings
    bool darkTheme() const { return m_darkTheme; }
    void setDarkTheme(bool dark);

    bool confirmDialogs() const { return m_confirmDialogs; }
    void setConfirmDialogs(bool confirm);

    int autoSaveInterval() const { return m_autoSaveInterval; }
    void setAutoSaveInterval(int minutes);

    bool rememberLastProject() const { return m_rememberLastProject; }
    void setRememberLastProject(bool remember);

    QString lastProjectPath() const { return m_lastProjectPath; }
    void setLastProjectPath(const QString& path);

private:
    Settings();
    ~Settings();
    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;

    // Window
    QSize m_mainWindowSize;
    QPoint m_mainWindowPos;
    bool m_mainWindowMaximized;
    QByteArray m_mainWindowState;
    QByteArray m_splitterState;

    // Paths
    QString m_repakPath;
    QString m_gameDirectory;
    QString m_defaultProjectsDir;
    QString m_lastOpenDir;
    QString m_lastSaveDir;

    // Recent files
    QStringList m_recentProjects;
    int m_maxRecentProjects;

    // Build
    int m_defaultCompressLevel;
    int m_defaultCompressWorkers;
    bool m_autoCompressBuilds;
    bool m_copyToGameAfterBuild;

    // Editor
    int m_texturePreviewSize;
    bool m_showAssetPreviews;
    bool m_enable3DPreview;

    // UI
    bool m_darkTheme;
    bool m_confirmDialogs;
    int m_autoSaveInterval;
    bool m_rememberLastProject;
    QString m_lastProjectPath;
};

#endif // SETTINGS_H
