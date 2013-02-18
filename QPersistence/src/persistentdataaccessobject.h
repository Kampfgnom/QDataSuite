#ifndef QPERSISTENCE_PERSISTENTDATAACCESSOBJECT_H
#define QPERSISTENCE_PERSISTENTDATAACCESSOBJECT_H

#include <QDataSuite/abstractdataaccessobject.h>

#include <QPersistence/sqldataaccessobjecthelper.h>
#include <QDataSuite/metaobject.h>
#include <QtCore/QSharedDataPointer>
#include <QtSql/QSqlDatabase>

namespace QDataSuite {
class Error;
}

class QSqlQuery;

namespace QPersistence {

class SqlDataAccessObjectHelper;

class PersistentDataAccessObjectBasePrivate;
class PersistentDataAccessObjectBase : public QDataSuite::AbstractDataAccessObject
{
    Q_OBJECT
public:
    explicit PersistentDataAccessObjectBase(const QMetaObject &metaObject,
                                            const QSqlDatabase &database = QSqlDatabase::database(),
                                            QObject *parent = 0);
    ~PersistentDataAccessObjectBase();

    SqlDataAccessObjectHelper *sqlDataAccessObjectHelper() const;
    QDataSuite::MetaObject dataSuiteMetaObject() const;

    int count() const Q_DECL_OVERRIDE;
    QList<QVariant> allKeys() const Q_DECL_OVERRIDE;
    QList<QObject *> readAllObjects() const Q_DECL_OVERRIDE;
    QObject *readObject(const QVariant &key) const Q_DECL_OVERRIDE;
    bool insertObject(QObject *const object) Q_DECL_OVERRIDE;
    bool updateObject(QObject *const object) Q_DECL_OVERRIDE;
    bool removeObject(QObject *const object) Q_DECL_OVERRIDE;

private:
    QSharedDataPointer<PersistentDataAccessObjectBasePrivate> d;

    Q_DISABLE_COPY(PersistentDataAccessObjectBase)
};

template<class T>
class PersistentDataAccessObject : public PersistentDataAccessObjectBase
{
public:
    explicit PersistentDataAccessObject(const QSqlDatabase &database = QSqlDatabase::database(), QObject *parent = 0) :
        PersistentDataAccessObjectBase(T::staticMetaObject, database, parent)
    {
    }

    QList<T *> readAll() const
    {
        QList<T *> result;
        Q_FOREACH(QObject *object, readAllObjects()) result.append(static_cast<T *>(object));
        return result;
    }

    QObject *createObject() const Q_DECL_OVERRIDE { return new T; }
    T *create() const { return static_cast<T *>(createObject()); }
    T *read(const QVariant &key) const { return static_cast<T *>(readObject(key)); }
    bool insert(T *const object) { return insertObject(object); }
    bool update(T *const object) { return updateObject(object); }
    bool remove(T *const object) { return removeObject(object); }
};

} // namespace QPersistence

#endif // QPERSISTENCE_PERSISTENTDATAACCESSOBJECT_H
