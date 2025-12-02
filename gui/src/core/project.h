#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QDateTime>
#include <QObject>

#include "asset.h"

/**
 * PAK version enumeration
 */
enum class PakVersion {
    Version7 = 7,   // Titanfall 2
    Version8 = 8    // Apex Legends
};

/**
 * Represents a RePak project configuration
 * Maps directly to the JSON build map format
 */
class Project : public QObject
{
    Q_OBJECT

public:
    explicit Project(QObject* parent = nullptr);
    ~Project() override;

    // File operations
    bool load(const QString& filePath);
    bool save(const QString& filePath = QString());
    bool saveAs(const QString& filePath);
    void clear();

    // State
    bool isModified() const { return m_modified; }
    bool hasFile() const { return !m_filePath.isEmpty(); }
    QString filePath() const { return m_filePath; }
    QString fileName() const;
    QString projectDir() const;

    // Project settings
    QString name() const { return m_name; }
    void setName(const QString& name);

    PakVersion version() const { return m_version; }
    void setVersion(PakVersion version);

    QString outputDir() const { return m_outputDir; }
    void setOutputDir(const QString& dir);

    QString assetsDir() const { return m_assetsDir; }
    void setAssetsDir(const QString& dir);

    // Build flags
    bool keepDevOnly() const { return m_keepDevOnly; }
    void setKeepDevOnly(bool keep);

    bool keepServerOnly() const { return m_keepServerOnly; }
    void setKeepServerOnly(bool keep);

    bool keepClientOnly() const { return m_keepClientOnly; }
    void setKeepClientOnly(bool keep);

    bool showDebugInfo() const { return m_showDebugInfo; }
    void setShowDebugInfo(bool show);

    // Compression settings
    int compressLevel() const { return m_compressLevel; }
    void setCompressLevel(int level);

    int compressWorkers() const { return m_compressWorkers; }
    void setCompressWorkers(int workers);

    // Streaming files
    QString streamFileMandatory() const { return m_streamFileMandatory; }
    void setStreamFileMandatory(const QString& path);

    QString streamFileOptional() const { return m_streamFileOptional; }
    void setStreamFileOptional(const QString& path);

    QString streamCache() const { return m_streamCache; }
    void setStreamCache(const QString& path);

    QStringList streamCacheFilter() const { return m_streamCacheFilter; }
    void setStreamCacheFilter(const QStringList& filter);

    // Asset management
    const QVector<Asset>& assets() const { return m_assets; }
    int assetCount() const { return m_assets.size(); }

    void addAsset(const Asset& asset);
    void addAssets(const QVector<Asset>& assets);
    void removeAsset(int index);
    void removeAssets(const QVector<int>& indices);
    void updateAsset(int index, const Asset& asset);
    void moveAsset(int from, int to);
    void clearAssets();

    Asset assetAt(int index) const;
    int findAssetByPath(const QString& path) const;
    int findAssetByGuid(const QString& guid) const;
    QVector<Asset> assetsByType(AssetType type) const;

    // Statistics
    int assetCountByType(AssetType type) const;
    QMap<AssetType, int> assetCountsByType() const;
    qint64 totalAssetSize() const;

    // Validation
    QStringList validate() const;
    bool hasValidationErrors() const;

    // Serialization
    QJsonObject toJson() const;
    static Project* fromJson(const QJsonObject& json, QObject* parent = nullptr);

    // Output path helpers
    QString outputFilePath() const;
    QString absoluteOutputDir() const;
    QString absoluteAssetsDir() const;

signals:
    void modifiedChanged(bool modified);
    void nameChanged(const QString& name);
    void assetAdded(int index);
    void assetRemoved(int index);
    void assetUpdated(int index);
    void assetMoved(int from, int to);
    void assetsCleared();
    void projectLoaded();
    void projectSaved();

private:
    void setModified(bool modified);
    void parseJson(const QJsonObject& json);

private:
    // File info
    QString m_filePath;
    bool m_modified;

    // Project settings
    QString m_name;
    PakVersion m_version;
    QString m_outputDir;
    QString m_assetsDir;

    // Build flags
    bool m_keepDevOnly;
    bool m_keepServerOnly;
    bool m_keepClientOnly;
    bool m_showDebugInfo;

    // Compression
    int m_compressLevel;
    int m_compressWorkers;

    // Streaming
    QString m_streamFileMandatory;
    QString m_streamFileOptional;
    QString m_streamCache;
    QStringList m_streamCacheFilter;

    // Assets
    QVector<Asset> m_assets;
};

#endif // PROJECT_H
