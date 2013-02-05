#include <QPersistence/persistentdataaccessobject.h>

#include <QPersistence/sqldataaccessobjecthelper.h>
#include <QDataSuite/error.h>
#include <QtCore/QVariant>

namespace QPersistence {

template<class T>
PersistentDataAccessObject<T>::PersistentDataAccessObject(const QSqlDatabase &database, QObject *parent) :
    AbstractDataAccessObject(parent),
    m_sqlDataAccessObjectHelper(SqlDataAccessObjectHelper::forDatabase(database)),
    m_metaObject(QDataSuite::MetaObject::metaObject(T::staticMetaObject))
{}


template<class T>
QList<QVariant> PersistentDataAccessObject<T>::allKeys() const
{
    QList<QVariant> result = m_sqlDataAccessObjectHelper->allKeys(m_metaObject);

    if(m_sqlDataAccessObjectHelper->lastError().isValid())
        setLastError(m_sqlDataAccessObjectHelper->lastError());

    return result;
}

template<class T>
QList<T *> PersistentDataAccessObject<T>::readAll() const
{
    QList<T *> result;
    Q_FOREACH(const QVariant key, allKeys()) result.append(read(key));
    return result;
}

template<class T>
QList<QObject *> PersistentDataAccessObject<T>::readAllObjects() const
{
    QList<QObject *> result;
    Q_FOREACH(T *object, readAll()) result.append(object);
    return result;
}

template<class T>
T *PersistentDataAccessObject<T>::create() const
{
    return new T;
}

template<class T>
QObject *PersistentDataAccessObject<T>::createObject() const
{
    return create();
}

template<class T>
T *PersistentDataAccessObject<T>::read(const QVariant &key) const
{
    T *t = create();

    if(!m_sqlDataAccessObjectHelper->readObject(m_metaObject, key, t)) {
        setLastError(m_sqlDataAccessObjectHelper->lastError());
        delete t;
        return nullptr;
    }

    return t;
}

template<class T>
QObject *PersistentDataAccessObject<T>::readObject(const QVariant &key) const
{
    return read(key);
}

template<class T>
bool PersistentDataAccessObject<T>::insert(T * const object)
{
    if(!m_sqlDataAccessObjectHelper->insertObject(m_metaObject, object)) {
        setLastError(m_sqlDataAccessObjectHelper->lastError());
        return false;
    }

    return true;
}

template<class T>
bool PersistentDataAccessObject<T>::insertObject(QObject *const object)
{
    T * const t = qobject_cast<T * const>(object);
    Q_ASSERT(t);
    return insert(t);
}

template<class T>
bool PersistentDataAccessObject<T>::update(T *const object)
{
    if(!m_sqlDataAccessObjectHelper->updateObject(m_metaObject, object)) {
        setLastError(m_sqlDataAccessObjectHelper->lastError());
        return false;
    }

    return true;
}

template<class T>
bool PersistentDataAccessObject<T>::updateObject(QObject *const object)
{
    T *t = qobject_cast<T *>(object);
    Q_ASSERT(t);
    return update(t);
}

template<class T>
bool PersistentDataAccessObject<T>::remove(T *const object)
{
    if(m_sqlDataAccessObjectHelper->removeObject(m_metaObject, object)) {
        setLastError(m_sqlDataAccessObjectHelper->lastError());
        return false;
    }

    return true;
}

template<class T>
bool PersistentDataAccessObject<T>::removeObject(QObject *const object)
{
    T *t = qobject_cast<T *>(object);
    Q_ASSERT(t);
    return remove(t);
}

} // namespace QPersistence
