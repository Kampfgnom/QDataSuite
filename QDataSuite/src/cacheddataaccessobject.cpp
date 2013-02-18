#include <QDataSuite/cacheddataaccessobject.h>

#include <QDataSuite/metaproperty.h>
#include <QDataSuite/metaobject.h>
#include <QDataSuite/error.h>

#include <QDebug>

namespace QDataSuite {

template<class T>
CachedDataAccessObject<T>::CachedDataAccessObject(AbstractDataAccessObject *source, QObject *parent) :
    AbstractDataAccessObject(parent),
    m_source(source)
{}

template<class T>
T *CachedDataAccessObject<T>::getFromCache(const QVariant &key) const
{
    if(m_cache.contains(key)) {
        QSharedPointer<T> p = m_cache.value(key);
        if(p) {
            return p.data();
        }
        else {
            m_cache.remove(key);
        }
    }

    return nullptr;
}

template<class T>
void CachedDataAccessObject<T>::insertIntoCache(const QVariant &key, T *object) const
{
    Q_ASSERT(!m_cache.contains(key));

    QSharedPointer<T> p(object);
    m_cache.insert(key, p);
}

template<class T>
MetaObject CachedDataAccessObject<T>::dataSuiteMetaObject() const
{
    return m_source->dataSuiteMetaObject();
}

template<class T>
int CachedDataAccessObject<T>::count() const
{
    return m_source->count();
}

template<class T>
QList<QVariant> CachedDataAccessObject<T>::allKeys() const
{
    resetLastError();
    return m_source->allKeys();
}

template<class T>
QList<T *> CachedDataAccessObject<T>::readAll() const
{
    resetLastError();

    QList<T *> result;
    Q_FOREACH(QVariant key, allKeys()) {
        T *t = getFromCache(key);

        if(!t) {
            t = static_cast<T *>(m_source->readObject(key));
            insertIntoCache(key, t);
        }

        result.append(t);
    }

    return result;
}

template<class T>
QList<QObject *> CachedDataAccessObject<T>::readAllObjects() const
{
    QList<QObject *> result;
    Q_FOREACH(T *object, readAll()) result.append(object);
    return result;
}

template<class T>
T *CachedDataAccessObject<T>::create() const
{
    resetLastError();
    return static_cast<T *>(m_source->createObject());
}

template<class T>
QObject *CachedDataAccessObject<T>::createObject() const
{
    return create();
}

template<class T>
T *CachedDataAccessObject<T>::read(const QVariant &key) const
{
    resetLastError();

    T *t = getFromCache(key);

    if(!t) {
        t = static_cast<T *>(m_source->readObject(key));
        insertIntoCache(key, t);
    }

    return t;
}

template<class T>
QObject *CachedDataAccessObject<T>::readObject(const QVariant &key) const
{
    return read(key);
}

template<class T>
bool CachedDataAccessObject<T>::insert(T * const object)
{
    resetLastError();

    if(!m_source->insertObject(object)) {
        setLastError(m_source->lastError());
        return false;
    }

    QVariant key = dataSuiteMetaObject().primaryKeyProperty().read(object);
    insertIntoCache(key, object);

    emit objectInserted(object);
    return true;
}

template<class T>
bool CachedDataAccessObject<T>::insertObject(QObject *const object)
{
    T * const t = qobject_cast<T * const>(object);
    Q_ASSERT(t);
    return insert(t);
}

template<class T>
bool CachedDataAccessObject<T>::update(T *const object)
{
    resetLastError();

    QVariant key = dataSuiteMetaObject().primaryKeyProperty().read(object);
    Q_ASSERT(m_cache.contains(key));

    if(!m_source->updateObject(object)) {
        setLastError(m_source->lastError());
        return false;
    }

    emit objectUpdated(object);
    return true;
}

template<class T>
bool CachedDataAccessObject<T>::updateObject(QObject *const object)
{
    T *t = qobject_cast<T *>(object);
    Q_ASSERT(t);
    return update(t);
}

template<class T>
bool CachedDataAccessObject<T>::remove(T *const object)
{
    resetLastError();
    QVariant key = dataSuiteMetaObject().primaryKeyProperty().read(object);
    Q_ASSERT(m_cache.contains(key));

    if(!m_source->removeObject(object)) {
        setLastError(m_source->lastError());
        return false;
    }

    m_cache.remove(key);

    emit objectRemoved(object);
    return true;
}

template<class T>
bool CachedDataAccessObject<T>::removeObject(QObject *const object)
{
    T *t = qobject_cast<T *>(object);
    Q_ASSERT(t);
    return remove(t);
}

} // namespace QDataSuite
