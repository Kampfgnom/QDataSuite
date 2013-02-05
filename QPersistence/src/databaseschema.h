#ifndef QPERSISTENCE_DATABASESCHEMAHELPER_H
#define QPERSISTENCE_DATABASESCHEMAHELPER_H

#include <QtCore/QObject>

#include <QtCore/QVariant>
#include <QtCore/QSharedDataPointer>
#include <QtSql/QSqlDatabase>

class QSqlQuery;

namespace QDataSuite {
class Error;
class MetaProperty;
}

namespace QPersistence {

class DatabaseSchemaPrivate;
class DatabaseSchema : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseSchema(const QSqlDatabase &database = QSqlDatabase::database(), QObject *parent = 0);
    ~DatabaseSchema();

    bool existsTable(const QMetaObject *metaObject);
    void createTable(const QMetaObject *metaObject);
    void createTableIfNotExists(const QMetaObject *metaObject);
    void dropTable(const QMetaObject *metaObject);
    bool addMissingColumns(const QMetaObject *metaObject);
    void addColumn(const QDataSuite::MetaProperty &metaProperty);

    template<class O> bool existsTable() { return existsTable(&O::staticMetaObject); }
    template<class O> void createTable() { createTable(&O::staticMetaObject); }
    template<class O> void createTableIfNotExists() { createTableIfNotExists(&O::staticMetaObject); }
    template<class O> void dropTable() { dropTable(&O::staticMetaObject); }
    template<class O> void addMissingColumns() { addMissingColumns(&O::staticMetaObject); }

    void createCleanSchema();
    void adjustSchema();

    QDataSuite::Error lastError() const;

    static QString variantTypeToSqlType(QVariant::Type type);

private:
    QSharedDataPointer<DatabaseSchemaPrivate> d;

    void setLastError(const QDataSuite::Error &error) const;
    void setLastError(const QSqlQuery &query) const;
};

} // namespace QPersistence

#endif // QPERSISTENCE_DATABASESCHEMAHELPER_H
