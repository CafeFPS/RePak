#include "validation.h"

#include "core/project.h"
#include "core/asset.h"

namespace Validation {

QList<Issue> validateProject(const Project* project)
{
    QList<Issue> issues;

    if (!project) {
        issues.append({Severity::Error, "", "No project loaded", "", false});
        return issues;
    }

    // Check project settings
    if (project->outputDir().isEmpty()) {
        issues.append({Severity::Error, "", "Output directory not specified", "Set output directory in project settings", true});
    }

    // Check for duplicates
    issues.append(checkDuplicateGuids(project));

    // Check for missing files
    issues.append(checkMissingFiles(project));

    // Validate each asset
    const auto& assets = project->assets();
    for (int i = 0; i < assets.size(); ++i) {
        QList<Issue> assetIssues = validateAsset(assets[i], project);
        issues.append(assetIssues);
    }

    return issues;
}

QList<Issue> validateAsset(const Asset& asset, const Project* project)
{
    QList<Issue> issues;

    if (asset.type().isEmpty()) {
        issues.append({Severity::Error, asset.path(), "Asset type not specified", "Specify asset type", false});
    }

    if (asset.path().isEmpty()) {
        issues.append({Severity::Error, asset.path(), "Asset path not specified", "Specify asset path", false});
    }

    if (asset.assetType() == AssetType::Unknown) {
        issues.append({Severity::Error, asset.path(), QString("Unknown asset type: %1").arg(asset.type()), "Use a valid asset type (txtr, matl, mdl_, etc.)", false});
    }

    // Check GUID format
    if (!asset.guid().isEmpty()) {
        if (!asset.guid().startsWith("0x", Qt::CaseInsensitive)) {
            issues.append({Severity::Warning, asset.path(), "GUID should start with '0x'", "Add '0x' prefix to GUID", true});
        }
    }

    // Check file existence
    if (project) {
        QString assetsDir = project->absoluteAssetsDir();
        if (!asset.exists(assetsDir)) {
            issues.append({Severity::Error, asset.path(), "Asset file not found", "Check file path or remove asset", false});
        }
    }

    return issues;
}

QList<Issue> checkDuplicateGuids(const Project* project)
{
    QList<Issue> issues;

    if (!project) {
        return issues;
    }

    QMap<QString, QStringList> guidUsage;
    const auto& assets = project->assets();

    for (const Asset& asset : assets) {
        QString guid = asset.guid();
        if (!guid.isEmpty()) {
            guidUsage[guid.toLower()].append(asset.path());
        }
    }

    for (auto it = guidUsage.begin(); it != guidUsage.end(); ++it) {
        if (it.value().size() > 1) {
            QString paths = it.value().join(", ");
            issues.append({
                Severity::Error,
                it.value().first(),
                QString("Duplicate GUID %1 used by: %2").arg(it.key()).arg(paths),
                "Regenerate GUID for duplicate assets",
                true
            });
        }
    }

    return issues;
}

QList<Issue> checkMissingFiles(const Project* project)
{
    QList<Issue> issues;

    if (!project) {
        return issues;
    }

    QString assetsDir = project->absoluteAssetsDir();
    const auto& assets = project->assets();

    for (const Asset& asset : assets) {
        if (!asset.exists(assetsDir)) {
            issues.append({
                Severity::Error,
                asset.path(),
                QString("File not found: %1").arg(asset.absolutePath(assetsDir)),
                "Check file path or remove asset from project",
                false
            });
        }
    }

    return issues;
}

QList<Issue> checkCircularDependencies(const Project* project)
{
    QList<Issue> issues;
    // TODO: Implement circular dependency detection
    return issues;
}

} // namespace Validation
