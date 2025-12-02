#ifndef JSONUTILS_H
#define JSONUTILS_H

#include <QString>
#include <QJsonDocument>
#include <QJsonObject>

/**
 * JSON utility functions
 */
namespace JsonUtils {

/**
 * Parse JSON from file with comment support
 */
QJsonDocument parseFromFile(const QString& filePath, QString* errorMsg = nullptr);

/**
 * Remove comments from JSON string
 */
QString removeComments(const QString& json);

/**
 * Pretty-print JSON object
 */
QString prettyPrint(const QJsonObject& obj);

} // namespace JsonUtils

#endif // JSONUTILS_H
