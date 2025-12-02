#include "asset.h"
#include <QFileInfo>
#include <QDir>
#include <QJsonArray>

Asset::Asset()
    : m_assetType(AssetType::Unknown)
    , m_scope(AssetScope::All)
{
}

Asset::Asset(const QString& type, const QString& path)
    : m_type(type)
    , m_path(path)
{
    m_assetType = typeFromString(type);
    m_scope = getScopeForType(m_assetType);
}

Asset::Asset(const QJsonObject& json)
{
    *this = fromJson(json);
}

QJsonObject Asset::toJson() const
{
    QJsonObject obj;
    obj["_type"] = m_type;
    obj["_path"] = m_path;

    if (!m_guid.isEmpty()) {
        obj["$guid"] = m_guid;
    }

    // Add type-specific properties
    for (auto it = m_properties.begin(); it != m_properties.end(); ++it) {
        const QString& key = it.key();
        const QVariant& value = it.value();

        if (value.typeId() == QMetaType::QString) {
            obj[key] = value.toString();
        } else if (value.typeId() == QMetaType::Int) {
            obj[key] = value.toInt();
        } else if (value.typeId() == QMetaType::Double) {
            obj[key] = value.toDouble();
        } else if (value.typeId() == QMetaType::Bool) {
            obj[key] = value.toBool();
        } else if (value.typeId() == QMetaType::QStringList) {
            QJsonArray arr;
            for (const QString& s : value.toStringList()) {
                arr.append(s);
            }
            obj[key] = arr;
        }
    }

    return obj;
}

Asset Asset::fromJson(const QJsonObject& json)
{
    Asset asset;

    asset.m_type = json["_type"].toString();
    asset.m_path = json["_path"].toString();
    asset.m_guid = json.contains("$guid") ? json["$guid"].toString() :
                   (json.contains("guid") ? json["guid"].toString() : QString());

    asset.m_assetType = typeFromString(asset.m_type);
    asset.m_scope = getScopeForType(asset.m_assetType);

    // Extract additional properties
    for (auto it = json.begin(); it != json.end(); ++it) {
        const QString& key = it.key();
        if (key != "_type" && key != "_path" && key != "$guid" && key != "guid") {
            QJsonValue val = it.value();
            if (val.isString()) {
                asset.m_properties[key] = val.toString();
            } else if (val.isDouble()) {
                asset.m_properties[key] = val.toDouble();
            } else if (val.isBool()) {
                asset.m_properties[key] = val.toBool();
            } else if (val.isArray()) {
                QStringList list;
                for (const QJsonValue& v : val.toArray()) {
                    if (v.isString()) {
                        list.append(v.toString());
                    }
                }
                asset.m_properties[key] = list;
            }
        }
    }

    return asset;
}

AssetType Asset::typeFromString(const QString& typeStr)
{
    static const QMap<QString, AssetType> typeMap = {
        {"txtr", AssetType::Texture},
        {"matl", AssetType::Material},
        {"mt4a", AssetType::MaterialAspect},
        {"mdl_", AssetType::Model},
        {"shdr", AssetType::Shader},
        {"shds", AssetType::ShaderSet},
        {"arig", AssetType::AnimRig},
        {"aseq", AssetType::AnimSequence},
        {"anir", AssetType::AnimRecording},
        {"txan", AssetType::TextureAnim},
        {"txls", AssetType::TextureList},
        {"uimg", AssetType::UIImageAtlas},
        {"dtbl", AssetType::Datatable},
        {"stgs", AssetType::Settings},
        {"stlt", AssetType::SettingsLayout},
        {"rlcd", AssetType::LCDEffect},
        {"ptch", AssetType::Patch}
    };

    return typeMap.value(typeStr.toLower(), AssetType::Unknown);
}

QString Asset::typeToString(AssetType type)
{
    static const QMap<AssetType, QString> typeMap = {
        {AssetType::Texture, "txtr"},
        {AssetType::Material, "matl"},
        {AssetType::MaterialAspect, "mt4a"},
        {AssetType::Model, "mdl_"},
        {AssetType::Shader, "shdr"},
        {AssetType::ShaderSet, "shds"},
        {AssetType::AnimRig, "arig"},
        {AssetType::AnimSequence, "aseq"},
        {AssetType::AnimRecording, "anir"},
        {AssetType::TextureAnim, "txan"},
        {AssetType::TextureList, "txls"},
        {AssetType::UIImageAtlas, "uimg"},
        {AssetType::Datatable, "dtbl"},
        {AssetType::Settings, "stgs"},
        {AssetType::SettingsLayout, "stlt"},
        {AssetType::LCDEffect, "rlcd"},
        {AssetType::Patch, "ptch"}
    };

    return typeMap.value(type, "unknown");
}

QString Asset::typeToDisplayName(AssetType type)
{
    static const QMap<AssetType, QString> nameMap = {
        {AssetType::Unknown, "Unknown"},
        {AssetType::Texture, "Texture"},
        {AssetType::Material, "Material"},
        {AssetType::MaterialAspect, "Material (Aspect)"},
        {AssetType::Model, "Model"},
        {AssetType::Shader, "Shader"},
        {AssetType::ShaderSet, "Shader Set"},
        {AssetType::AnimRig, "Animation Rig"},
        {AssetType::AnimSequence, "Animation Sequence"},
        {AssetType::AnimRecording, "Animation Recording"},
        {AssetType::TextureAnim, "Texture Animation"},
        {AssetType::TextureList, "Texture List"},
        {AssetType::UIImageAtlas, "UI Image Atlas"},
        {AssetType::Datatable, "Datatable"},
        {AssetType::Settings, "Settings"},
        {AssetType::SettingsLayout, "Settings Layout"},
        {AssetType::LCDEffect, "LCD Effect"},
        {AssetType::Patch, "Patch"}
    };

    return nameMap.value(type, "Unknown");
}

QString Asset::typeToFourCC(AssetType type)
{
    return typeToString(type).toUpper();
}

AssetScope Asset::getScopeForType(AssetType type)
{
    switch (type) {
        case AssetType::Texture:
        case AssetType::Material:
        case AssetType::MaterialAspect:
        case AssetType::Shader:
        case AssetType::ShaderSet:
        case AssetType::TextureAnim:
        case AssetType::UIImageAtlas:
        case AssetType::LCDEffect:
            return AssetScope::ClientOnly;
        case AssetType::AnimRecording:
            return AssetScope::ServerOnly;
        default:
            return AssetScope::All;
    }
}

QVariant Asset::property(const QString& key) const
{
    return m_properties.value(key);
}

void Asset::setProperty(const QString& key, const QVariant& value)
{
    m_properties[key] = value;
}

void Asset::setType(const QString& type)
{
    m_type = type;
    m_assetType = typeFromString(type);
    m_scope = getScopeForType(m_assetType);
}

QString Asset::fileName() const
{
    return QFileInfo(m_path).fileName();
}

QString Asset::absolutePath(const QString& assetsDir) const
{
    if (m_path.isEmpty()) return QString();

    if (QDir::isAbsolutePath(m_path)) {
        return m_path;
    }

    return QDir(assetsDir).absoluteFilePath(m_path);
}

qint64 Asset::fileSize(const QString& assetsDir) const
{
    QFileInfo fi(absolutePath(assetsDir));
    return fi.exists() ? fi.size() : -1;
}

bool Asset::exists(const QString& assetsDir) const
{
    return QFile::exists(absolutePath(assetsDir));
}

QString Asset::displayName() const
{
    if (m_path.isEmpty()) return "Unnamed Asset";

    QFileInfo fi(m_path);
    QString name = fi.completeBaseName();

    // If name looks like a GUID, show it as such
    if (name.startsWith("0x") && name.length() > 10) {
        return name;
    }

    return name;
}

QString Asset::typeDisplayName() const
{
    return typeToDisplayName(m_assetType);
}

bool Asset::isValid() const
{
    return !m_type.isEmpty() && !m_path.isEmpty() && m_assetType != AssetType::Unknown;
}

QStringList Asset::validate(const QString& assetsDir) const
{
    QStringList errors;

    if (m_type.isEmpty()) {
        errors << "Asset type is not specified";
    }

    if (m_path.isEmpty()) {
        errors << "Asset path is not specified";
    }

    if (m_assetType == AssetType::Unknown) {
        errors << QString("Unknown asset type: %1").arg(m_type);
    }

    if (!assetsDir.isEmpty() && !exists(assetsDir)) {
        errors << QString("Asset file not found: %1").arg(absolutePath(assetsDir));
    }

    // GUID validation
    if (!m_guid.isEmpty()) {
        if (!m_guid.startsWith("0x") && !m_guid.startsWith("0X")) {
            errors << "GUID should start with '0x'";
        }
    }

    return errors;
}

bool Asset::operator==(const Asset& other) const
{
    return m_type == other.m_type &&
           m_path == other.m_path &&
           m_guid == other.m_guid;
}
