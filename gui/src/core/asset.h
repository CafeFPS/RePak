#ifndef ASSET_H
#define ASSET_H

#include <QString>
#include <QJsonObject>
#include <QMap>
#include <QVariant>

/**
 * Asset type enumeration matching RePak's supported types
 */
enum class AssetType {
    Unknown = 0,
    Texture,        // txtr
    Material,       // matl
    MaterialAspect, // mt4a
    Model,          // mdl_
    Shader,         // shdr
    ShaderSet,      // shds
    AnimRig,        // arig
    AnimSequence,   // aseq
    AnimRecording,  // anir
    TextureAnim,    // txan
    TextureList,    // txls
    UIImageAtlas,   // uimg
    Datatable,      // dtbl
    Settings,       // stgs
    SettingsLayout, // stlt
    LCDEffect,      // rlcd
    Patch           // ptch
};

/**
 * Asset scope - matches RePak's PakAssetScope_e
 */
enum class AssetScope {
    All,
    ServerOnly,
    ClientOnly
};

/**
 * Represents a single asset in a RePak project
 */
class Asset
{
public:
    Asset();
    Asset(const QString& type, const QString& path);
    explicit Asset(const QJsonObject& json);

    // Serialization
    QJsonObject toJson() const;
    static Asset fromJson(const QJsonObject& json);

    // Type helpers
    static AssetType typeFromString(const QString& typeStr);
    static QString typeToString(AssetType type);
    static QString typeToDisplayName(AssetType type);
    static QString typeToFourCC(AssetType type);
    static AssetScope getScopeForType(AssetType type);

    // Getters
    QString type() const { return m_type; }
    QString path() const { return m_path; }
    QString guid() const { return m_guid; }
    AssetType assetType() const { return m_assetType; }
    AssetScope scope() const { return m_scope; }
    bool hasCustomGuid() const { return !m_guid.isEmpty(); }

    // Setters
    void setType(const QString& type);
    void setPath(const QString& path) { m_path = path; }
    void setGuid(const QString& guid) { m_guid = guid; }

    // Additional properties (type-specific)
    QVariant property(const QString& key) const;
    void setProperty(const QString& key, const QVariant& value);
    QMap<QString, QVariant> properties() const { return m_properties; }
    void setProperties(const QMap<QString, QVariant>& props) { m_properties = props; }

    // Metadata accessor (alias for properties, used by editors)
    QVariantMap metadata() const { return m_properties; }
    void setMetadata(const QVariantMap& meta) { m_properties = meta; }

    // File info
    QString fileName() const;
    QString absolutePath(const QString& assetsDir) const;
    qint64 fileSize(const QString& assetsDir) const;
    bool exists(const QString& assetsDir) const;

    // Display
    QString displayName() const;
    QString typeDisplayName() const;

    // Validation
    bool isValid() const;
    QStringList validate(const QString& assetsDir) const;

    // Comparison
    bool operator==(const Asset& other) const;
    bool operator!=(const Asset& other) const { return !(*this == other); }

private:
    QString m_type;         // Four-character type code (e.g., "txtr")
    QString m_path;         // Relative path to asset file
    QString m_guid;         // Optional custom GUID
    AssetType m_assetType;  // Parsed asset type
    AssetScope m_scope;     // Asset scope
    QMap<QString, QVariant> m_properties; // Type-specific properties
};

Q_DECLARE_METATYPE(Asset)
Q_DECLARE_METATYPE(AssetType)

#endif // ASSET_H
