#ifndef QPERSISTENCE_SQLDATAACCESSOBJECTHELPER_H
#define QPERSISTENCE_SQLDATAACCESSOBJECTHELPER_H

#include <QtCore/QObject>

#include <QtCore/QSharedDataPointer>
#include <QtSql/QSqlDatabase>

namespace QDataSuite {
class Error;
class MetaObject;
}

class QSqlQuery;

namespace QPersistence {

class SqlQuery;
class PersistentDataAccessObjectBase;

class SqlDataAccessObjectHelperPrivate;
class SqlDataAccessObjectHelper : public QObject
{
    Q_OBJECT
public:
    ~SqlDataAccessObjectHelper();

    static SqlDataAccessObjectHelper *forDatabase(const QSqlDatabase &database = QSqlDatabase::database());

    QList<QVariant> allKeys(const QDataSuite::MetaObject &metaObject) const;
    bool readObject(const QDataSuite::MetaObject &metaObject, const QVariant &key, QObject *object);
    bool insertObject(const QDataSuite::MetaObject &metaObject, QObject *object);
    bool updateObject(const QDataSuite::MetaObject &metaObject, const QObject *object);
    bool removeObject(const QDataSuite::MetaObject &metaObject, const QObject *object);
    bool readRelatedObjects(const QDataSuite::MetaObject &metaObject, QObject *object);

    QDataSuite::Error lastError() const;

    void registerPersistentDataAccessObject(const QString &className, PersistentDataAccessObjectBase *dataAccessObject);
    PersistentDataAccessObjectBase *persistentDataAccessObject(const QDataSuite::MetaObject &metaObject);

private:
    QSharedDataPointer<SqlDataAccessObjectHelperPrivate> d;

    explicit SqlDataAccessObjectHelper(const QSqlDatabase &database, QObject *parent = 0);

    void setLastError(const QDataSuite::Error &error) const;
    void setLastError(const QSqlQuery &query) const;

    void fillValuesIntoQuery(const QDataSuite::MetaObject &metaObject,
                             const QObject *object,
                             SqlQuery &queryconst);
    void readQueryIntoObject(const QSqlQuery &query,
                             QObject *object);
    bool adjustRelations(const QDataSuite::MetaObject &metaObject, const QObject *object);
    bool readRelatedObjects(const QDataSuite::MetaObject &metaObject,
                            QObject *object,
                            QHash<QString, QHash<QVariant, QObject *> > &alreadyReadObjectsPerTable);

};

} // namespace QPersistence

#endif // SQLDATAACCESSOBJECTHELPER_H
