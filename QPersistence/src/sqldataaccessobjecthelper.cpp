#include "sqldataaccessobjecthelper.h"

#include "databaseschema.h"
#include "sqlquery.h"
#include "sqlcondition.h"

#include <QDataSuite/metaproperty.h>
#include <QDataSuite/error.h>
#include <QDataSuite/metaobject.h>

#include <QDebug>
#include <QMetaProperty>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QVariant>

namespace QPersistence {

class SqlDataAccessObjectHelperPrivate : public QSharedData
{
public:
    SqlDataAccessObjectHelperPrivate() :
        QSharedData()
    {}

    QSqlDatabase database;
    mutable QDataSuite::Error lastError;

    static QHash<QString, SqlDataAccessObjectHelper *> helpersForConnection;
};

QHash<QString, SqlDataAccessObjectHelper *> SqlDataAccessObjectHelperPrivate::helpersForConnection;

SqlDataAccessObjectHelper::SqlDataAccessObjectHelper(const QSqlDatabase &database, QObject *parent) :
    QObject(parent),
    d(new SqlDataAccessObjectHelperPrivate)
{
    d->database = database;
}

SqlDataAccessObjectHelper::~SqlDataAccessObjectHelper()
{
}

SqlDataAccessObjectHelper *SqlDataAccessObjectHelper::forDatabase(const QSqlDatabase &database)
{
    static QObject guard;

    SqlDataAccessObjectHelper* asd = new SqlDataAccessObjectHelper(database, &guard);

    if(!SqlDataAccessObjectHelperPrivate::helpersForConnection.contains(database.connectionName()))
        SqlDataAccessObjectHelperPrivate::helpersForConnection.insert(database.connectionName(),
                                                                      asd);

    return SqlDataAccessObjectHelperPrivate::helpersForConnection.value(database.connectionName());
}

QList<QVariant> SqlDataAccessObjectHelper::allKeys(const QDataSuite::MetaObject &metaObject) const
{
    SqlQuery query(d->database);
    query.clear();
    query.setTable(metaObject.tableName());
    query.addField(metaObject.primaryKeyPropertyName());
    query.prepareSelect();

    QList<QVariant> result;
    if ( !query.exec()
         || query.lastError().isValid()) {
        setLastError(query);
        return result;
    }

    while (query.next()) {
        result.append(query.value(0));
    }

    return result;
}

bool SqlDataAccessObjectHelper::readObject(const QDataSuite::MetaObject &metaObject, const QVariant &key, QObject *object)
{
    Q_ASSERT(object);
    Q_ASSERT(!key.isNull());

    SqlQuery query(d->database);
    query.setTable(metaObject.tableName());
    query.setLimit(1);
    query.setWhereCondition(SqlCondition(metaObject.primaryKeyProperty().columnName(),
                                         SqlCondition::EqualTo,
                                         key));
    query.prepareSelect();

    if ( !query.exec()
         || !query.first()
         || query.lastError().isValid()) {
        setLastError(query);
        return false;
    }

    readQueryIntoObject(query, object);
    return true;
}

bool SqlDataAccessObjectHelper::insertObject(const QDataSuite::MetaObject &metaObject, QObject *object)
{
    Q_ASSERT(object);

    // Create main INSERT query
    SqlQuery query(d->database);
    query.setTable(metaObject.tableName());
    fillValuesIntoQuery(metaObject, object, query);

    // Insert the object itself
    query.prepareInsert();
    if ( !query.exec()
         || query.lastError().isValid()) {
        setLastError(query);
        return false;
    }

    if(metaObject.primaryKeyProperty().isAutoIncremented()) {
        metaObject.primaryKeyProperty().write(object, query.lastInsertId());
    }

    // Update related objects
    return adjustRelations(metaObject, object);
}

bool SqlDataAccessObjectHelper::updateObject(const QDataSuite::MetaObject &metaObject, const QObject *object)
{
    Q_ASSERT(object);

    // Create main UPDATE query
    SqlQuery query(d->database);
    query.setTable(metaObject.tableName());
    query.setWhereCondition(SqlCondition(metaObject.primaryKeyProperty().columnName(),
                                         SqlCondition::EqualTo,
                                         metaObject.primaryKeyProperty().read(object)));
    fillValuesIntoQuery(metaObject, object, query);

    // Insert the object itself
    query.prepareUpdate();
    if ( !query.exec()
         || query.lastError().isValid()) {
        setLastError(query);
        return false;
    }

    // Update related objects
    return adjustRelations(metaObject, object);
}

void SqlDataAccessObjectHelper::fillValuesIntoQuery(const QDataSuite::MetaObject &metaObject,
                                                    const QObject *object,
                                                    SqlQuery &query)
{
    // Add simple properties
    foreach(const QDataSuite::MetaProperty property, metaObject.simpleProperties()) {
        query.addField(property.columnName(), property.read(object));
    }

    // Add relation properties
    foreach(const QDataSuite::MetaProperty property, metaObject.relationProperties()) {
        QDataSuite::MetaProperty::Cardinality cardinality = property.cardinality();

        // Only care for "XtoOne" relations, since only they have to be inserted into our table
        if(cardinality == QDataSuite::MetaProperty::ToOneCardinality
                || cardinality == QDataSuite::MetaProperty::ManyToOneCardinality) {
            QObject *relatedObject = QDataSuite::MetaObject::objectCast(property.read(object));

            if(!relatedObject)
                continue;

            QVariant foreignKey = property.reverseMetaObject().primaryKeyProperty().read(relatedObject);
            query.addField(property.columnName(), foreignKey);
        }
        else if(cardinality == QDataSuite::MetaProperty::OneToOneCardinality) {
            Q_ASSERT_X(false, Q_FUNC_INFO, "OneToOneCardinality relations are not supported yet.");
        }
    }
}

void SqlDataAccessObjectHelper::readQueryIntoObject(const QSqlQuery &query, QObject *object)
{
    QSqlRecord record = query.record();
    int fieldCount = record.count();
    for (int i = 0; i < fieldCount; ++i) {
        QString fieldName = record.fieldName(i);
        QVariant value = query.value(i);
        object->setProperty(fieldName.toLatin1(), value);
    }
}

bool SqlDataAccessObjectHelper::adjustRelations(const QDataSuite::MetaObject &metaObject, const QObject *object)
{
    QVariant primaryKey = metaObject.primaryKeyProperty().read(object);

    QList<SqlQuery> queries;

    foreach(const QDataSuite::MetaProperty property, metaObject.relationProperties()) {
        QDataSuite::MetaProperty::Cardinality cardinality = property.cardinality();

        // Only care for "XtoMany" relations, because these reside in other tables
        if(cardinality == QDataSuite::MetaProperty::ToManyCardinality
                || cardinality == QDataSuite::MetaProperty::OneToManyCardinality) {

            // Prepare a query, which resets the relation (set all foreign keys to NULL)
            SqlQuery resetRelationQuery(d->database);
            resetRelationQuery.setTable(property.tableName());
            resetRelationQuery.addField(metaObject.primaryKeyProperty().columnName(), QVariant());
            resetRelationQuery.setWhereCondition(SqlCondition(property.columnName(),
                                                              SqlCondition::EqualTo,
                                                              primaryKey));
            resetRelationQuery.prepareUpdate();
            queries.append(resetRelationQuery);

            // Check if there are related objects
            QList<QObject *> relatedObjects = QDataSuite::MetaObject::objectListCast(property.read(object));
            if(relatedObjects.isEmpty())
                continue;

            // Build an OR'd where clause, which matches all related objects
            QList<SqlCondition> relatedObjectsWhereClauses;
            QDataSuite::MetaProperty reversePrimaryKey = property.reverseMetaObject().primaryKeyProperty();
            foreach(QObject *relatedObject, relatedObjects) {
                relatedObjectsWhereClauses.append(SqlCondition(reversePrimaryKey.columnName(),
                                                               SqlCondition::EqualTo,
                                                               reversePrimaryKey.read(relatedObject)));
            }
            SqlCondition relatedObjectsWhereClause(SqlCondition::Or, relatedObjectsWhereClauses);

            // Prepare a query, which sets the foreign keys of the related objects to our objects key
            SqlQuery setForeignKeysQuery(d->database);
            setForeignKeysQuery.setTable(property.tableName());
            setForeignKeysQuery.addField(property.columnName(), primaryKey);
            setForeignKeysQuery.setWhereCondition(relatedObjectsWhereClause);
            setForeignKeysQuery.prepareUpdate();
            queries.append(setForeignKeysQuery);
        }
        else if(cardinality == QDataSuite::MetaProperty::ManyToManyCardinality) {
            Q_ASSERT_X(false, Q_FUNC_INFO, "ManyToManyCardinality relations are not supported yet.");
        }
        else if(cardinality == QDataSuite::MetaProperty::OneToOneCardinality) {
            Q_ASSERT_X(false, Q_FUNC_INFO, "OneToOneCardinality relations are not supported yet.");
        }
    }

    foreach(SqlQuery query, queries) {
        if ( !query.exec()
             || query.lastError().isValid()) {
            setLastError(query);
            return false;
        }
    }

    return true;
}

bool SqlDataAccessObjectHelper::removeObject(const QDataSuite::MetaObject &metaObject, const QObject *object)
{
    Q_ASSERT(object);

    SqlQuery query(d->database);
    query.setTable(metaObject.tableName());
    query.setWhereCondition(SqlCondition(metaObject.primaryKeyProperty().columnName(),
                                         SqlCondition::EqualTo,
                                         metaObject.primaryKeyProperty().read(object)));
    query.prepareDelete();

    if ( !query.exec()
         || query.lastError().isValid()) {
        setLastError(query);
        return false;
    }

    return true;
}

QDataSuite::Error SqlDataAccessObjectHelper::lastError() const
{
    return d->lastError;
}

void SqlDataAccessObjectHelper::setLastError(const QDataSuite::Error &error) const
{
    qDebug() << error;
    d->lastError = error;
}

void SqlDataAccessObjectHelper::setLastError(const QSqlQuery &query) const
{
    setLastError(QDataSuite::Error(query.lastError().text().append(": ").append(query.executedQuery()), QDataSuite::Error::SqlError));
}

} // namespace QPersistence
