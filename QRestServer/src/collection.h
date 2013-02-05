#ifndef QRESTSERVER_COLLECTION_H
#define QRESTSERVER_COLLECTION_H

#include <QDataSuite/Error.h>

class QObject;
class QString;
class QStringList;

namespace QRestServer {

class Collection
{
public:
    virtual QString name() const = 0;
    virtual QStringList keys() const = 0;
    virtual QObject *object(const QString &key) const = 0;
    virtual QObject *createObjectInstance() = 0;
    virtual void insertObject(QObject *object) = 0;
    virtual void removeObject(QObject *object) = 0;
    virtual void updateObject(QObject *object) = 0;

    QDataSuite::Error lastError() const;
    void setLastError(const QDataSuite::Error &error) const;
    void resetLastError() const;

private:
    mutable QDataSuite::Error m_lastError;
};

} // namespace QRestServer

#endif // QRESTSERVER_COLLECTION_H
