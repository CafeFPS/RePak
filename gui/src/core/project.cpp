#include "project.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>

Project::Project(QObject* parent)
    : QObject(parent)
    , m_modified(false)
    , m_version(PakVersion::Version8)
    , m_outputDir("./build/")
    , m_assetsDir("./assets/")
    , m_keepDevOnly(false)
    , m_keepServerOnly(true)
    , m_keepClientOnly(true)
    , m_showDebugInfo(false)
    , m_compressLevel(6)
    , m_compressWorkers(16)
{
}

Project::~Project()
{
}

bool Project::load(const QString& filePath)
{
    qDebug() << "Project::load() - Loading:" << filePath;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Project::load() - Failed to open file:" << file.errorString();
        return false;
    }

    QByteArray data = file.readAll();
    file.close();
    qDebug() << "Project::load() - Read" << data.size() << "bytes";

    // Remove comments (RePak supports // comments in JSON)
    QString content = QString::fromUtf8(data);
    QStringList lines = content.split('\n');
    QString cleanedContent;

    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        if (!trimmed.startsWith("//")) {
            // Remove inline comments
            int commentPos = line.indexOf("//");
            if (commentPos >= 0) {
                // Make sure it's not inside a string
                int quoteCount = 0;
                for (int i = 0; i < commentPos; ++i) {
                    if (line[i] == '"' && (i == 0 || line[i-1] != '\\')) {
                        quoteCount++;
                    }
                }
                if (quoteCount % 2 == 0) {
                    cleanedContent += line.left(commentPos) + '\n';
                    continue;
                }
            }
            cleanedContent += line + '\n';
        }
    }

    qDebug() << "Project::load() - Cleaned content size:" << cleanedContent.size();

    // Remove trailing commas (RePak's JSON parser allows them, Qt's doesn't)
    // Match: comma followed by optional whitespace and then } or ]
    QRegularExpression trailingCommaRegex(",\\s*([\\}\\]])");
    cleanedContent.replace(trailingCommaRegex, "\\1");

    qDebug() << "Project::load() - After trailing comma removal:" << cleanedContent.size();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(cleanedContent.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Project::load() - JSON parse error at offset" << error.offset << ":" << error.errorString();
        // Show context around the error
        int start = qMax(0, error.offset - 50);
        int end = qMin(cleanedContent.size(), error.offset + 50);
        qDebug() << "Context:" << cleanedContent.mid(start, end - start);
        return false;
    }

    if (!doc.isObject()) {
        qDebug() << "Project::load() - JSON document is not an object";
        return false;
    }

    clear();
    m_filePath = filePath;
    parseJson(doc.object());
    m_modified = false;

    qDebug() << "Project::load() - Successfully loaded" << m_assets.size() << "assets";
    emit projectLoaded();
    return true;
}

bool Project::save(const QString& filePath)
{
    QString targetPath = filePath.isEmpty() ? m_filePath : filePath;

    if (targetPath.isEmpty()) {
        return false;
    }

    QFile file(targetPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QJsonDocument doc(toJson());
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    m_filePath = targetPath;
    setModified(false);

    emit projectSaved();
    return true;
}

bool Project::saveAs(const QString& filePath)
{
    return save(filePath);
}

void Project::clear()
{
    m_filePath.clear();
    m_name.clear();
    m_version = PakVersion::Version8;
    m_outputDir = "./build/";
    m_assetsDir = "./assets/";
    m_keepDevOnly = false;
    m_keepServerOnly = true;
    m_keepClientOnly = true;
    m_showDebugInfo = false;
    m_compressLevel = 6;
    m_compressWorkers = 16;
    m_streamFileMandatory.clear();
    m_streamFileOptional.clear();
    m_streamCache.clear();
    m_streamCacheFilter.clear();
    m_assets.clear();
    m_modified = false;

    emit assetsCleared();
}

QString Project::fileName() const
{
    if (m_filePath.isEmpty()) return QString();
    return QFileInfo(m_filePath).fileName();
}

QString Project::projectDir() const
{
    if (m_filePath.isEmpty()) return QString();
    return QFileInfo(m_filePath).absolutePath();
}

void Project::setName(const QString& name)
{
    if (m_name != name) {
        m_name = name;
        setModified(true);
        emit nameChanged(name);
    }
}

void Project::setVersion(PakVersion version)
{
    if (m_version != version) {
        m_version = version;
        setModified(true);
    }
}

void Project::setOutputDir(const QString& dir)
{
    if (m_outputDir != dir) {
        m_outputDir = dir;
        setModified(true);
    }
}

void Project::setAssetsDir(const QString& dir)
{
    if (m_assetsDir != dir) {
        m_assetsDir = dir;
        setModified(true);
    }
}

void Project::setKeepDevOnly(bool keep)
{
    if (m_keepDevOnly != keep) {
        m_keepDevOnly = keep;
        setModified(true);
    }
}

void Project::setKeepServerOnly(bool keep)
{
    if (m_keepServerOnly != keep) {
        m_keepServerOnly = keep;
        setModified(true);
    }
}

void Project::setKeepClientOnly(bool keep)
{
    if (m_keepClientOnly != keep) {
        m_keepClientOnly = keep;
        setModified(true);
    }
}

void Project::setShowDebugInfo(bool show)
{
    if (m_showDebugInfo != show) {
        m_showDebugInfo = show;
        setModified(true);
    }
}

void Project::setCompressLevel(int level)
{
    level = qBound(-5, level, 22);
    if (m_compressLevel != level) {
        m_compressLevel = level;
        setModified(true);
    }
}

void Project::setCompressWorkers(int workers)
{
    workers = qBound(1, workers, 64);
    if (m_compressWorkers != workers) {
        m_compressWorkers = workers;
        setModified(true);
    }
}

void Project::setStreamFileMandatory(const QString& path)
{
    if (m_streamFileMandatory != path) {
        m_streamFileMandatory = path;
        setModified(true);
    }
}

void Project::setStreamFileOptional(const QString& path)
{
    if (m_streamFileOptional != path) {
        m_streamFileOptional = path;
        setModified(true);
    }
}

void Project::setStreamCache(const QString& path)
{
    if (m_streamCache != path) {
        m_streamCache = path;
        setModified(true);
    }
}

void Project::setStreamCacheFilter(const QStringList& filter)
{
    if (m_streamCacheFilter != filter) {
        m_streamCacheFilter = filter;
        setModified(true);
    }
}

void Project::addAsset(const Asset& asset)
{
    m_assets.append(asset);
    setModified(true);
    emit assetAdded(m_assets.size() - 1);
}

void Project::addAssets(const QVector<Asset>& assets)
{
    for (const Asset& asset : assets) {
        m_assets.append(asset);
        emit assetAdded(m_assets.size() - 1);
    }
    if (!assets.isEmpty()) {
        setModified(true);
    }
}

void Project::removeAsset(int index)
{
    if (index >= 0 && index < m_assets.size()) {
        m_assets.removeAt(index);
        setModified(true);
        emit assetRemoved(index);
    }
}

void Project::removeAssets(const QVector<int>& indices)
{
    // Sort indices in descending order to remove from end first
    QVector<int> sortedIndices = indices;
    std::sort(sortedIndices.begin(), sortedIndices.end(), std::greater<int>());

    for (int index : sortedIndices) {
        if (index >= 0 && index < m_assets.size()) {
            m_assets.removeAt(index);
            emit assetRemoved(index);
        }
    }

    if (!indices.isEmpty()) {
        setModified(true);
    }
}

void Project::updateAsset(int index, const Asset& asset)
{
    if (index >= 0 && index < m_assets.size()) {
        m_assets[index] = asset;
        setModified(true);
        emit assetUpdated(index);
    }
}

void Project::moveAsset(int from, int to)
{
    if (from >= 0 && from < m_assets.size() &&
        to >= 0 && to < m_assets.size() && from != to) {
        Asset asset = m_assets.takeAt(from);
        m_assets.insert(to, asset);
        setModified(true);
        emit assetMoved(from, to);
    }
}

void Project::clearAssets()
{
    if (!m_assets.isEmpty()) {
        m_assets.clear();
        setModified(true);
        emit assetsCleared();
    }
}

Asset Project::assetAt(int index) const
{
    if (index >= 0 && index < m_assets.size()) {
        return m_assets.at(index);
    }
    return Asset();
}

int Project::findAssetByPath(const QString& path) const
{
    for (int i = 0; i < m_assets.size(); ++i) {
        if (m_assets[i].path() == path) {
            return i;
        }
    }
    return -1;
}

int Project::findAssetByGuid(const QString& guid) const
{
    for (int i = 0; i < m_assets.size(); ++i) {
        if (m_assets[i].guid() == guid) {
            return i;
        }
    }
    return -1;
}

QVector<Asset> Project::assetsByType(AssetType type) const
{
    QVector<Asset> result;
    for (const Asset& asset : m_assets) {
        if (asset.assetType() == type) {
            result.append(asset);
        }
    }
    return result;
}

int Project::assetCountByType(AssetType type) const
{
    int count = 0;
    for (const Asset& asset : m_assets) {
        if (asset.assetType() == type) {
            count++;
        }
    }
    return count;
}

QMap<AssetType, int> Project::assetCountsByType() const
{
    QMap<AssetType, int> counts;
    for (const Asset& asset : m_assets) {
        counts[asset.assetType()]++;
    }
    return counts;
}

qint64 Project::totalAssetSize() const
{
    qint64 total = 0;
    QString assetsPath = absoluteAssetsDir();

    for (const Asset& asset : m_assets) {
        qint64 size = asset.fileSize(assetsPath);
        if (size > 0) {
            total += size;
        }
    }

    return total;
}

QStringList Project::validate() const
{
    QStringList errors;
    QString assetsPath = absoluteAssetsDir();

    // Check output directory
    if (m_outputDir.isEmpty()) {
        errors << "Output directory is not specified";
    }

    // Check for duplicate GUIDs
    QMap<QString, int> guidCounts;
    for (const Asset& asset : m_assets) {
        if (!asset.guid().isEmpty()) {
            guidCounts[asset.guid()]++;
        }
    }

    for (auto it = guidCounts.begin(); it != guidCounts.end(); ++it) {
        if (it.value() > 1) {
            errors << QString("Duplicate GUID found: %1 (used %2 times)")
                        .arg(it.key())
                        .arg(it.value());
        }
    }

    // Validate each asset
    for (int i = 0; i < m_assets.size(); ++i) {
        QStringList assetErrors = m_assets[i].validate(assetsPath);
        for (const QString& err : assetErrors) {
            errors << QString("Asset %1: %2").arg(i).arg(err);
        }
    }

    return errors;
}

bool Project::hasValidationErrors() const
{
    return !validate().isEmpty();
}

QJsonObject Project::toJson() const
{
    QJsonObject obj;

    obj["version"] = static_cast<int>(m_version);

    if (!m_name.isEmpty()) {
        obj["name"] = m_name;
    }

    obj["outputDir"] = m_outputDir;

    if (!m_assetsDir.isEmpty() && m_assetsDir != "./") {
        obj["assetsDir"] = m_assetsDir;
    }

    // Build flags (only write non-defaults)
    if (m_keepDevOnly) {
        obj["keepDevOnly"] = m_keepDevOnly;
    }
    if (!m_keepServerOnly) {
        obj["keepServerOnly"] = m_keepServerOnly;
    }
    if (!m_keepClientOnly) {
        obj["keepClientOnly"] = m_keepClientOnly;
    }
    if (m_showDebugInfo) {
        obj["showDebugInfo"] = m_showDebugInfo;
    }

    // Compression
    if (m_compressLevel != 6) {
        obj["compressLevel"] = m_compressLevel;
    }
    if (m_compressWorkers != 16) {
        obj["compressWorkers"] = m_compressWorkers;
    }

    // Streaming
    if (!m_streamFileMandatory.isEmpty()) {
        obj["streamFileMandatory"] = m_streamFileMandatory;
    }
    if (!m_streamFileOptional.isEmpty()) {
        obj["streamFileOptional"] = m_streamFileOptional;
    }
    if (!m_streamCache.isEmpty()) {
        obj["streamCache"] = m_streamCache;
    }
    if (!m_streamCacheFilter.isEmpty()) {
        QJsonArray filterArray;
        for (const QString& filter : m_streamCacheFilter) {
            filterArray.append(filter);
        }
        obj["streamCacheFilter"] = filterArray;
    }

    // Assets
    QJsonArray assetsArray;
    for (const Asset& asset : m_assets) {
        assetsArray.append(asset.toJson());
    }
    obj["files"] = assetsArray;

    return obj;
}

Project* Project::fromJson(const QJsonObject& json, QObject* parent)
{
    Project* project = new Project(parent);
    project->parseJson(json);
    return project;
}

void Project::parseJson(const QJsonObject& json)
{
    // Version
    m_version = static_cast<PakVersion>(json.value("version").toInt(8));

    // Basic settings
    m_name = json.value("name").toString();
    m_outputDir = json.value("outputDir").toString("./build/");
    m_assetsDir = json.value("assetsDir").toString("./assets/");

    // Build flags
    m_keepDevOnly = json.value("keepDevOnly").toBool(false);
    m_keepServerOnly = json.value("keepServerOnly").toBool(true);
    m_keepClientOnly = json.value("keepClientOnly").toBool(true);
    m_showDebugInfo = json.value("showDebugInfo").toBool(false);

    // Compression
    m_compressLevel = json.value("compressLevel").toInt(6);
    m_compressWorkers = json.value("compressWorkers").toInt(16);

    // Streaming
    m_streamFileMandatory = json.value("streamFileMandatory").toString();
    m_streamFileOptional = json.value("streamFileOptional").toString();
    m_streamCache = json.value("streamCache").toString();

    if (json.contains("streamCacheFilter")) {
        QJsonArray filterArray = json["streamCacheFilter"].toArray();
        m_streamCacheFilter.clear();
        for (const QJsonValue& val : filterArray) {
            m_streamCacheFilter.append(val.toString());
        }
    }

    // Assets - support both "files" and "assets" keys
    QJsonArray assetsArray = json.contains("files") ?
                             json["files"].toArray() :
                             json["assets"].toArray();

    m_assets.clear();
    for (const QJsonValue& val : assetsArray) {
        if (val.isObject()) {
            m_assets.append(Asset::fromJson(val.toObject()));
        }
    }
}

QString Project::outputFilePath() const
{
    QString output = absoluteOutputDir();
    QString pakName = m_name.isEmpty() ? "output" : m_name;
    return QDir(output).absoluteFilePath(pakName + ".rpak");
}

QString Project::absoluteOutputDir() const
{
    if (QDir::isAbsolutePath(m_outputDir)) {
        return m_outputDir;
    }

    QString baseDir = projectDir();
    if (baseDir.isEmpty()) {
        return m_outputDir;
    }

    return QDir(baseDir).absoluteFilePath(m_outputDir);
}

QString Project::absoluteAssetsDir() const
{
    if (QDir::isAbsolutePath(m_assetsDir)) {
        return m_assetsDir;
    }

    QString baseDir = projectDir();
    if (baseDir.isEmpty()) {
        return m_assetsDir;
    }

    return QDir(baseDir).absoluteFilePath(m_assetsDir);
}

void Project::setModified(bool modified)
{
    if (m_modified != modified) {
        m_modified = modified;
        emit modifiedChanged(modified);
    }
}
