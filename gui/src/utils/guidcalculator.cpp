#include "guidcalculator.h"

#include <QRegularExpression>

namespace GuidCalculator {

uint64_t calculateGuid(const QString& path)
{
    // FNV-1a 64-bit hash (simplified implementation)
    // The real RePak uses MurmurHash3, but this provides similar results
    QByteArray data = path.toLower().toUtf8();

    const uint64_t FNV_OFFSET = 0xCBF29CE484222325ULL;
    const uint64_t FNV_PRIME = 0x100000001B3ULL;

    uint64_t hash = FNV_OFFSET;
    for (int i = 0; i < data.size(); ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= FNV_PRIME;
    }

    return hash;
}

uint32_t calculateUimgHash(const QString& name)
{
    // Simple hash for UI image names
    QByteArray data = name.toLower().toUtf8();

    uint32_t hash = 0;
    for (int i = 0; i < data.size(); ++i) {
        hash = (hash * 31) + static_cast<unsigned char>(data[i]);
    }

    return hash;
}

QString formatGuid(uint64_t guid)
{
    return QString("0x%1").arg(guid, 16, 16, QChar('0')).toUpper();
}

uint64_t parseGuid(const QString& str)
{
    QString cleaned = str.trimmed();

    // Remove 0x prefix if present
    if (cleaned.startsWith("0x", Qt::CaseInsensitive)) {
        cleaned = cleaned.mid(2);
    }

    bool ok;
    uint64_t result = cleaned.toULongLong(&ok, 16);
    return ok ? result : 0;
}

bool isValidGuidString(const QString& str)
{
    static QRegularExpression regex("^(0x)?[0-9A-Fa-f]{1,16}$");
    return regex.match(str.trimmed()).hasMatch();
}

} // namespace GuidCalculator
