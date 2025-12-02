#include "fileutils.h"

#include <QDir>
#include <QFileInfo>
#include <QDirIterator>

namespace FileUtils {

QString formatFileSize(qint64 bytes)
{
    if (bytes < 0) {
        return "Unknown";
    }

    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;

    if (bytes < KB) {
        return QString("%1 B").arg(bytes);
    } else if (bytes < MB) {
        return QString("%1 KB").arg(bytes / static_cast<double>(KB), 0, 'f', 1);
    } else if (bytes < GB) {
        return QString("%1 MB").arg(bytes / static_cast<double>(MB), 0, 'f', 1);
    } else {
        return QString("%1 GB").arg(bytes / static_cast<double>(GB), 0, 'f', 2);
    }
}

QString relativePath(const QString& basePath, const QString& targetPath)
{
    QDir baseDir(basePath);
    return baseDir.relativeFilePath(targetPath);
}

QStringList findFiles(const QString& directory, const QStringList& filters, bool recursive)
{
    QStringList result;
    QDir dir(directory);

    if (!dir.exists()) {
        return result;
    }

    QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags;
    if (recursive) {
        flags = QDirIterator::Subdirectories;
    }

    QDirIterator it(directory, filters, QDir::Files, flags);
    while (it.hasNext()) {
        result.append(it.next());
    }

    return result;
}

bool isInsideDirectory(const QString& path, const QString& directory)
{
    QFileInfo fileInfo(path);
    QFileInfo dirInfo(directory);

    QString absPath = fileInfo.absoluteFilePath();
    QString absDir = dirInfo.absoluteFilePath();

    return absPath.startsWith(absDir);
}

bool ensureDirectoryExists(const QString& path)
{
    QDir dir(path);
    if (dir.exists()) {
        return true;
    }
    return dir.mkpath(".");
}

} // namespace FileUtils
