#include <QDataSuite/cacheddataaccessobject.h>

#include <QDataSuite/metaproperty.h>
#include <QDataSuite/metaobject.h>
#include <QDataSuite/error.h>

#include <QDebug>

namespace QDataSuite {

template<class T>
CachedDataAccessObject<T>::CachedDataAccessObject(AbstractDataAccessObject *source, QObject *parent) :
    AbstractDataAccessObject(parent),
    m_source(source),
    m_cachedCount(-1),
    m_cachedAll(false)
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

    if(!m_cachedAll) {
        int c = m_cache.size();
        if(c == count()) {
            m_cachedAll = true;
            m_cachedCount = c;
        }
    }
}

template<class T>
MetaObject CachedDataAccessObject<T>::dataSuiteMetaObject() const
{
    return m_source->dataSuiteMetaObject();
}

template<class T>
int CachedDataAccessObject<T>::count() const
{
    if(m_cachedCount >= 0)
        return m_cachedCount;

    int c = m_source->count();
    m_cachedCount = c;
    return c;
}

template<class T>
QList<QVariant> CachedDataAccessObject<T>::allKeys() const
{
    resetLastError();

    if(m_cachedAll)
        return m_cache.keys();

    return m_source->allKeys();
}

template<class T>
QList<T *> CachedDataAccessObject<T>::readAll() const
{
    resetLastError();

    QList<T *> result;

    if(m_cachedAll) {
        Q_FOREACH(QSharedPointer<T> t, m_cache.values()) {
            result.append(t.data());
        }
    }
    else {
        Q_FOREACH(QVariant key, allKeys()) {
            T *t = getFromCache(key);

            if(!t) {
                t = static_cast<T *>(m_source->readObject(key));
                insertIntoCache(key, t);
            }

            result.append(t);
        }
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
    ++m_cachedCount;
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

    --m_cachedCount;
    emit objectRemoved(object);

    m_cache.remove(key);
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
