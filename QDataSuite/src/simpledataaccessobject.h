#ifndef QDATASUITE_SIMPLEDATAACCESSOBJECT_H
#define QDATASUITE_SIMPLEDATAACCESSOBJECT_H

#include <QDataSuite/abstractdataaccessobject.h>

#include <QDataSuite/metaobject.h>
#include <QtCore/QHash>
#include <QtCore/QVariant>

namespace QDataSuite {

template<class T>
class SimpleDataAccessObject : public AbstractDataAccessObject
{
public:
    SimpleDataAccessObject(QObject *parent = 0);

    QDataSuite::MetaObject dataSuiteMetaObject() const Q_DECL_OVERRIDE;

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
    QHash<QVariant, T *> m_objects;
    MetaObject m_metaObject;
};

} // namespace QDataSuite

#include "simpledataaccessobject.cpp"

#endif // QDATASUITE_SIMPLEDATAACCESSOBJECT_H

