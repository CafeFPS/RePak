#ifndef REPAKPROCESS_H
#define REPAKPROCESS_H

#include <QObject>

/**
 * Placeholder for RePak process interaction
 */
class RepakProcess : public QObject
{
    Q_OBJECT

public:
    explicit RepakProcess(QObject* parent = nullptr) : QObject(parent) {}
};

#endif // REPAKPROCESS_H
