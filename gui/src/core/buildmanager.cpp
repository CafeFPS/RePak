#include "buildmanager.h"
#include "project.h"

#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>

BuildManager::BuildManager(QObject* parent)
    : QObject(parent)
    , m_process(new QProcess(this))
    , m_status(BuildStatus::Idle)
    , m_currentAsset(0)
    , m_totalAssets(0)
    , m_errorCount(0)
    , m_warningCount(0)
    , m_compressAfterBuild(false)
    , m_compressLevel(6)
    , m_compressWorkers(16)
{
    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &BuildManager::onProcessReadyRead);
    connect(m_process, &QProcess::readyReadStandardError,
            this, &BuildManager::onProcessReadyRead);
    connect(m_process, &QProcess::errorOccurred,
            this, &BuildManager::onProcessError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &BuildManager::onProcessFinished);
}

BuildManager::~BuildManager()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished();
    }
}

QString BuildManager::statusText() const
{
    switch (m_status) {
        case BuildStatus::Idle: return "Ready";
        case BuildStatus::Building: return "Building...";
        case BuildStatus::Compressing: return "Compressing...";
        case BuildStatus::Copying: return "Copying to game...";
        case BuildStatus::Success: return "Build succeeded";
        case BuildStatus::Failed: return "Build failed";
        case BuildStatus::Cancelled: return "Build cancelled";
    }
    return "Unknown";
}

void BuildManager::build(Project* project, const QString& repakPath)
{
    if (isBuilding()) {
        return;
    }

    if (!project || repakPath.isEmpty()) {
        emit buildError("Invalid project or RePak path");
        setStatus(BuildStatus::Failed);
        return;
    }

    // Save project first to ensure JSON is up to date
    if (project->isModified()) {
        if (!project->save()) {
            emit buildError("Failed to save project before building");
            setStatus(BuildStatus::Failed);
            return;
        }
    }

    m_outputPath = project->outputFilePath();
    m_currentAsset = 0;
    m_totalAssets = project->assetCount();
    m_errorCount = 0;
    m_warningCount = 0;
    m_currentOperation = "Starting build...";
    m_repakPath = repakPath;
    m_compressAfterBuild = project->compressLevel() > 0;
    m_compressLevel = project->compressLevel();
    m_compressWorkers = project->compressWorkers();

    m_timer.start();
    setStatus(BuildStatus::Building);
    emit buildStarted();

    // Run RePak
    QStringList args;
    args << project->filePath();

    m_process->setWorkingDirectory(project->projectDir());
    m_process->start(repakPath, args);
}

void BuildManager::compress(const QString& pakPath, const QString& repakPath, int level, int workers)
{
    if (isBuilding()) {
        return;
    }

    m_outputPath = pakPath;
    m_errorCount = 0;
    m_warningCount = 0;
    m_currentOperation = "Compressing...";
    m_repakPath = repakPath;

    m_timer.start();
    setStatus(BuildStatus::Compressing);
    emit buildStarted();

    QStringList args;
    args << "-compress" << pakPath << QString::number(level) << QString::number(workers);

    m_process->setWorkingDirectory(QFileInfo(pakPath).absolutePath());
    m_process->start(repakPath, args);
}

void BuildManager::decompress(const QString& pakPath, const QString& repakPath)
{
    if (isBuilding()) {
        return;
    }

    m_outputPath = pakPath;
    m_errorCount = 0;
    m_warningCount = 0;
    m_currentOperation = "Decompressing...";
    m_repakPath = repakPath;

    m_timer.start();
    setStatus(BuildStatus::Compressing);
    emit buildStarted();

    QStringList args;
    args << "-decompress" << pakPath;

    m_process->setWorkingDirectory(QFileInfo(pakPath).absolutePath());
    m_process->start(repakPath, args);
}

void BuildManager::cancel()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        setStatus(BuildStatus::Cancelled);
        emit buildFinished(BuildStatus::Cancelled);
    }
}

qint64 BuildManager::elapsedMs() const
{
    return m_timer.isValid() ? m_timer.elapsed() : 0;
}

void BuildManager::onProcessReadyRead()
{
    QByteArray stdOut = m_process->readAllStandardOutput();
    QByteArray stdErr = m_process->readAllStandardError();

    QString output = QString::fromLocal8Bit(stdOut) + QString::fromLocal8Bit(stdErr);
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        if (!trimmed.isEmpty()) {
            parseOutput(trimmed);
            emit buildOutput(trimmed);
        }
    }
}

void BuildManager::onProcessError(QProcess::ProcessError error)
{
    QString errorMsg;
    switch (error) {
        case QProcess::FailedToStart:
            errorMsg = "Failed to start RePak. Check the executable path.";
            break;
        case QProcess::Crashed:
            errorMsg = "RePak crashed during execution.";
            break;
        case QProcess::Timedout:
            errorMsg = "RePak timed out.";
            break;
        case QProcess::WriteError:
            errorMsg = "Failed to write to RePak process.";
            break;
        case QProcess::ReadError:
            errorMsg = "Failed to read from RePak process.";
            break;
        default:
            errorMsg = "Unknown error occurred.";
            break;
    }

    m_errorCount++;
    emit buildError(errorMsg);
    setStatus(BuildStatus::Failed);
    emit buildFinished(BuildStatus::Failed);
}

void BuildManager::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (m_status == BuildStatus::Cancelled) {
        return;
    }

    if (exitStatus == QProcess::CrashExit || exitCode != 0) {
        setStatus(BuildStatus::Failed);
        emit buildFinished(BuildStatus::Failed);
        return;
    }

    // Check if we need to compress after build
    if (m_status == BuildStatus::Building && m_compressAfterBuild) {
        compress(m_outputPath, m_repakPath, m_compressLevel, m_compressWorkers);
        return;
    }

    setStatus(BuildStatus::Success);
    emit buildFinished(BuildStatus::Success);
}

void BuildManager::setStatus(BuildStatus status)
{
    if (m_status != status) {
        m_status = status;
        emit statusChanged(status);
    }
}

void BuildManager::parseOutput(const QString& line)
{
    // Parse progress from RePak output
    // Example: "[INFO] Processing textures (250 assets)"
    // Example: "[WARN] Texture 0x12D775... has no stream layout"
    // Example: "[ERROR] Material references missing shader"

    static QRegularExpression errorRegex(R"(\[ERROR\]\s*(.*))");
    static QRegularExpression warnRegex(R"(\[WARN\]\s*(.*))");
    static QRegularExpression progressRegex(R"(Processing\s+(\w+)\s+\((\d+)\s+assets?\))");
    static QRegularExpression assetRegex(R"((\d+)\s+of\s+(\d+))");

    QRegularExpressionMatch match;

    // Check for errors
    match = errorRegex.match(line);
    if (match.hasMatch()) {
        m_errorCount++;
        emit buildError(match.captured(1));
        return;
    }

    // Check for warnings
    match = warnRegex.match(line);
    if (match.hasMatch()) {
        m_warningCount++;
        emit buildWarning(match.captured(1));
        return;
    }

    // Check for progress
    match = progressRegex.match(line);
    if (match.hasMatch()) {
        m_currentOperation = QString("Processing %1...").arg(match.captured(1));
    }

    // Check for asset count progress
    match = assetRegex.match(line);
    if (match.hasMatch()) {
        m_currentAsset = match.captured(1).toInt();
        m_totalAssets = match.captured(2).toInt();
        emit buildProgress(m_currentAsset, m_totalAssets, m_currentOperation);
    }
}
