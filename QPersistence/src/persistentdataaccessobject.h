#ifndef QPERSISTENCE_PERSISTENTDATAACCESSOBJECT_H
#define QPERSISTENCE_PERSISTENTDATAACCESSOBJECT_H

#include <QDataSuite/abstractdataaccessobject.h>

#include <QDataSuite/metaobject.h>
#include <QtCore/QSharedDataPointer>
#include <QtSql/QSqlDatabase>

namespace QDataSuite {
class Error;
}

class QSqlQuery;

namespace QPersistence {

class SqlDataAccessObjectHelper;

template<class T>
class PersistentDataAccessObject : public QDataSuite::AbstractDataAccessObject
{
public:
    explicit PersistentDataAccessObject(const QSqlDatabase &database = QSqlDatabase::database(), QObject *parent = 0);

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
    SqlDataAccessObjectHelper *m_sqlDataAccessObjectHelper;
    QDataSuite::MetaObject m_metaObject;
};

} // namespace QPersistence

#include "persistentdataaccessobject.cpp"

#endif // QPERSISTENCE_PERSISTENTDATAACCESSOBJECT_H
