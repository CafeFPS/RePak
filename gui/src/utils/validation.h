#ifndef VALIDATION_H
#define VALIDATION_H

#include <QString>
#include <QStringList>

class Project;
class Asset;

/**
 * Validation utilities
 */
namespace Validation {

/**
 * Validation issue severity
 */
enum class Severity {
    Error,      // Build will fail
    Warning,    // Build may succeed but with issues
    Info        // Informational only
};

/**
 * Validation issue
 */
struct Issue {
    Severity severity;
    QString assetPath;
    QString message;
    QString suggestion;
    bool autoFixable;
};

/**
 * Validate entire project
 */
QList<Issue> validateProject(const Project* project);

/**
 * Validate single asset
 */
QList<Issue> validateAsset(const Asset& asset, const Project* project);

/**
 * Check for duplicate GUIDs
 */
QList<Issue> checkDuplicateGuids(const Project* project);

/**
 * Check for missing files
 */
QList<Issue> checkMissingFiles(const Project* project);

/**
 * Check for circular dependencies
 */
QList<Issue> checkCircularDependencies(const Project* project);

} // namespace Validation

#endif // VALIDATION_H
