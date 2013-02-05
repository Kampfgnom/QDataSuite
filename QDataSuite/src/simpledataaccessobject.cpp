#include <QDataSuite/simpledataaccessobject.h>

#include <QDataSuite/metaproperty.h>

namespace QDataSuite {

template<class T>
SimpleDataAccessObject<T>::SimpleDataAccessObject(QObject *parent) :
    AbstractDataAccessObject(parent),
    m_metaObject(MetaObject::metaObject(T::staticMetaObject))
{}


template<class T>
QList<QVariant> SimpleDataAccessObject<T>::allKeys() const
{
    return m_objects.keys();
}

template<class T>
QList<T *> SimpleDataAccessObject<T>::readAll() const
{
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
    return m_objects.value(key);
}

template<class T>
QObject *SimpleDataAccessObject<T>::readObject(const QVariant &key) const
{
    return read(key);
}

template<class T>
bool SimpleDataAccessObject<T>::insert(T * const object)
{
    m_objects.insert(m_metaObject.primaryKeyProperty().read(object), object);
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
