#ifndef BUILDMANAGER_H
#define BUILDMANAGER_H

#include <QObject>
#include <QProcess>
#include <QElapsedTimer>

class Project;

/**
 * Build status enumeration
 */
enum class BuildStatus {
    Idle,
    Building,
    Compressing,
    Copying,
    Success,
    Failed,
    Cancelled
};

/**
 * Manages the RePak build process
 */
class BuildManager : public QObject
{
    Q_OBJECT

public:
    explicit BuildManager(QObject* parent = nullptr);
    ~BuildManager() override;

    // State
    bool isBuilding() const { return m_status == BuildStatus::Building ||
                                    m_status == BuildStatus::Compressing ||
                                    m_status == BuildStatus::Copying; }
    BuildStatus status() const { return m_status; }
    QString statusText() const;

    // Build operations
    void build(Project* project, const QString& repakPath);
    void compress(const QString& pakPath, const QString& repakPath, int level, int workers);
    void decompress(const QString& pakPath, const QString& repakPath);
    void cancel();

    // Progress info
    int currentAsset() const { return m_currentAsset; }
    int totalAssets() const { return m_totalAssets; }
    qint64 elapsedMs() const;
    QString currentOperation() const { return m_currentOperation; }

    // Results
    QString outputPath() const { return m_outputPath; }
    int errorCount() const { return m_errorCount; }
    int warningCount() const { return m_warningCount; }

signals:
    void buildStarted();
    void buildProgress(int current, int total, const QString& assetName);
    void buildOutput(const QString& text);
    void buildError(const QString& error);
    void buildWarning(const QString& warning);
    void buildFinished(BuildStatus status);
    void statusChanged(BuildStatus status);

private slots:
    void onProcessReadyRead();
    void onProcessError(QProcess::ProcessError error);
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    void setStatus(BuildStatus status);
    void parseOutput(const QString& line);

private:
    QProcess* m_process;
    BuildStatus m_status;
    QElapsedTimer m_timer;

    QString m_outputPath;
    QString m_currentOperation;
    int m_currentAsset;
    int m_totalAssets;
    int m_errorCount;
    int m_warningCount;

    bool m_compressAfterBuild;
    QString m_repakPath;
    int m_compressLevel;
    int m_compressWorkers;
};

#endif // BUILDMANAGER_H
