#ifndef QDATASUITE_ABSTRACTDATAACCESSOBJECT_H
#define QDATASUITE_ABSTRACTDATAACCESSOBJECT_H

#include <QtCore/QObject>

#include <QtCore/QSharedDataPointer>

uint qHash(const QVariant & var);

namespace QDataSuite {

class Error;

class AbstractDataAccessObjectPrivate;
class AbstractDataAccessObject : public QObject
{
    Q_OBJECT
public:
    ~AbstractDataAccessObject();

    virtual QList<QVariant> allKeys() const = 0;
    virtual QList<QObject *> readAllObjects() const = 0;
    virtual QObject *createObject() const = 0;
    virtual QObject *readObject(const QVariant &key) const = 0;
    virtual bool insertObject(QObject *const object) = 0;
    virtual bool updateObject(QObject *const object) = 0;
    virtual bool removeObject(QObject *const object) = 0;

    QDataSuite::Error lastError() const;

protected:
    explicit AbstractDataAccessObject(QObject *parent = 0);
    void setLastError(const QDataSuite::Error &error) const;

private:
    Q_DISABLE_COPY(AbstractDataAccessObject)
    QSharedDataPointer<AbstractDataAccessObjectPrivate> d;
};

} // namespace QDataSuite

#endif // QDATASUITE_ABSTRACTDATAACCESSOBJECT_H
