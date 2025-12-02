#ifndef GUIDCALCULATOR_H
#define GUIDCALCULATOR_H

#include <QString>
#include <cstdint>

/**
 * GUID calculation utilities (matching RePak's RTech implementation)
 */
namespace GuidCalculator {

/**
 * Calculate asset GUID from path string
 */
uint64_t calculateGuid(const QString& path);

/**
 * Calculate UI image hash from name
 */
uint32_t calculateUimgHash(const QString& name);

/**
 * Format GUID as hex string
 */
QString formatGuid(uint64_t guid);

/**
 * Parse GUID from hex string
 */
uint64_t parseGuid(const QString& str);

/**
 * Validate GUID string format
 */
bool isValidGuidString(const QString& str);

} // namespace GuidCalculator

#endif // GUIDCALCULATOR_H
