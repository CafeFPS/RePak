#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>
#include <QStringList>

/**
 * File utility functions
 */
namespace FileUtils {

/**
 * Format file size as human-readable string
 */
QString formatFileSize(qint64 bytes);

/**
 * Get relative path from base to target
 */
QString relativePath(const QString& basePath, const QString& targetPath);

/**
 * Find files matching pattern recursively
 */
QStringList findFiles(const QString& directory, const QStringList& filters, bool recursive = true);

/**
 * Check if path is inside directory
 */
bool isInsideDirectory(const QString& path, const QString& directory);

/**
 * Ensure directory exists
 */
bool ensureDirectoryExists(const QString& path);

} // namespace FileUtils

#endif // FILEUTILS_H
