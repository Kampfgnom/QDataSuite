#include <QDataSuite/simpledataaccessobject.h>

#include <QDataSuite/metaproperty.h>
#include <QDataSuite/metaobject.h>
#include <QDataSuite/error.h>

#include <QDebug>

namespace QDataSuite {

template<class T>
SimpleDataAccessObject<T>::SimpleDataAccessObject(QObject *parent) :
    AbstractDataAccessObject(parent),
    m_metaObject(MetaObject::metaObject(T::staticMetaObject))
{}

template<class T>
MetaObject SimpleDataAccessObject<T>::dataSuiteMetaObject() const
{
    return m_metaObject;
}


template<class T>
QList<QVariant> SimpleDataAccessObject<T>::allKeys() const
{
    resetLastError();
    return m_objects.keys();
}

template<class T>
QList<T *> SimpleDataAccessObject<T>::readAll() const
{
    resetLastError();
    return m_objects.values();
}

template<class T>
QList<QObject *> SimpleDataAccessObject<T>::readAllObjects() const
{
    QList<QObject *> result;
    Q_FOREACH(T *object, readAll()) result.append(object);
    return result;
}

template<class T>
T *SimpleDataAccessObject<T>::create() const
{
    resetLastError();
    return new T;
}

template<class T>
QObject *SimpleDataAccessObject<T>::createObject() const
{
    return create();
}

template<class T>
T *SimpleDataAccessObject<T>::read(const QVariant &key) const
{
    resetLastError();
    int type = m_metaObject.primaryKeyProperty().type();
    Q_ASSERT(key.canConvert(type));

    QVariant keyVariant(key);
    keyVariant.convert(type);

    return m_objects.value(keyVariant);
}

template<class T>
QObject *SimpleDataAccessObject<T>::readObject(const QVariant &key) const
{
    return read(key);
}

template<class T>
bool SimpleDataAccessObject<T>::insert(T * const object)
{
    resetLastError();
    QVariant key = m_metaObject.primaryKeyProperty().read(object);
    if(m_objects.contains(key)) {
        setLastError(QDataSuite::Error("An object with this key already exists.",
                                       QDataSuite::Error::StorageError));
        return false;
    }

    m_objects.insert(key, object);
    return true;
}

template<class T>
bool SimpleDataAccessObject<T>::insertObject(QObject *const object)
{
    T * const t = qobject_cast<T * const>(object);
    Q_ASSERT(t);
    return insert(t);
}

template<class T>
bool SimpleDataAccessObject<T>::update(T *const object)
{
    resetLastError();
    return m_objects.contains(m_metaObject.primaryKeyProperty().read(object));
}

template<class T>
bool SimpleDataAccessObject<T>::updateObject(QObject *const object)
{
    T *t = qobject_cast<T *>(object);
    Q_ASSERT(t);
    return update(t);
}

template<class T>
bool SimpleDataAccessObject<T>::remove(T *const object)
{
    resetLastError();
    QVariant key = m_metaObject.primaryKeyProperty().read(object);
    bool ok = m_objects.contains(key);
    if(ok) m_objects.remove(key);
    return ok;
}

template<class T>
bool SimpleDataAccessObject<T>::removeObject(QObject *const object)
{
    T *t = qobject_cast<T *>(object);
    Q_ASSERT(t);
    return remove(t);
}

} // namespace QDataSuite
