#include "jsonutils.h"

#include <QFile>
#include <QJsonParseError>

namespace JsonUtils {

QJsonDocument parseFromFile(const QString& filePath, QString* errorMsg)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMsg) *errorMsg = "Failed to open file";
        return QJsonDocument();
    }

    QString content = QString::fromUtf8(file.readAll());
    file.close();

    // Remove comments
    content = removeComments(content);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        if (errorMsg) *errorMsg = error.errorString();
        return QJsonDocument();
    }

    return doc;
}

QString removeComments(const QString& json)
{
    QStringList lines = json.split('\n');
    QString result;

    for (const QString& line : lines) {
        QString trimmed = line.trimmed();

        // Skip full-line comments
        if (trimmed.startsWith("//")) {
            continue;
        }

        // Remove inline comments (not inside strings)
        int commentPos = line.indexOf("//");
        if (commentPos >= 0) {
            // Count quotes before comment to check if inside string
            int quoteCount = 0;
            for (int i = 0; i < commentPos; ++i) {
                if (line[i] == '"' && (i == 0 || line[i-1] != '\\')) {
                    quoteCount++;
                }
            }

            // If even number of quotes, comment is outside string
            if (quoteCount % 2 == 0) {
                result += line.left(commentPos) + '\n';
                continue;
            }
        }

        result += line + '\n';
    }

    return result;
}

QString prettyPrint(const QJsonObject& obj)
{
    QJsonDocument doc(obj);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
}

} // namespace JsonUtils
