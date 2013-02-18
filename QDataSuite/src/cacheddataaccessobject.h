#ifndef QDATASUITE_CACHEDDATAACCESSOBJECT_H
#define QDATASUITE_CACHEDDATAACCESSOBJECT_H

#include <QDataSuite/abstractdataaccessobject.h>

#include <QWeakPointer>

namespace QDataSuite {

template<class T>
class CachedDataAccessObject : public AbstractDataAccessObject
{
public:
    explicit CachedDataAccessObject(AbstractDataAccessObject *source, QObject *parent = 0);

    QDataSuite::MetaObject dataSuiteMetaObject() const Q_DECL_OVERRIDE;

    int count() const Q_DECL_OVERRIDE;
    QList<QVariant> allKeys() const Q_DECL_OVERRIDE;
    QList<QObject *> readAllObjects() const Q_DECL_OVERRIDE;
    QObject *createObject() const Q_DECL_OVERRIDE;
    QObject *readObject(const QVariant &key) const Q_DECL_OVERRIDE;
    bool insertObject(QObject *const object) Q_DECL_OVERRIDE;
    bool updateObject(QObject *const object) Q_DECL_OVERRIDE;
    bool removeObject(QObject *const object) Q_DECL_OVERRIDE;

    QList<T *> readAll() const;
    T *create() const;
    T *read(const QVariant &key) const;
    bool insert(T *const object);
    bool update(T *const object);
    bool remove(T *const object);

private:
    mutable QHash<QVariant, QSharedPointer<T> > m_cache;
    AbstractDataAccessObject *m_source;

    T *getFromCache(const QVariant &key) const;
    void insertIntoCache(const QVariant &key, T *object) const;
};

} // namespace QDataSuite

#include "cacheddataaccessobject.cpp"

#endif // QDATASUITE_CACHEDDATAACCESSOBJECT_H
