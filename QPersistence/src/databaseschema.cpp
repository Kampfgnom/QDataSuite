#include "databaseschema.h"

#include "sqlquery.h"

#include <QDataSuite/metaproperty.h>
#include <QDataSuite/error.h>
#include <QDataSuite/metaobject.h>

#include <QSqlQuery>
#include <QSqlError>
#include <QMetaProperty>
#include <QStringList>
#include <QDebug>
#include <QSqlRecord>

namespace QPersistence {

class DatabaseSchemaPrivate : public QSharedData
{
public:
    DatabaseSchemaPrivate() :
        QSharedData()
    {}

    QSqlDatabase database;
    mutable QDataSuite::Error lastError;
    SqlQuery query;

    QString metaPropertyToColumnDefinition(const QDataSuite::MetaProperty &property);
};

DatabaseSchema::DatabaseSchema(const QSqlDatabase &database, QObject *parent) :
    QObject(parent),
    d(new DatabaseSchemaPrivate)
{
    d->database = database;
    d->query = SqlQuery(database);
}

DatabaseSchema::~DatabaseSchema()
{
}

bool DatabaseSchema::existsTable(const QMetaObject *metaObject)
{
    Q_ASSERT(metaObject);

    QDataSuite::MetaObject meta = QDataSuite::MetaObject::metaObject(metaObject);
    return d->database.tables().contains(meta.tableName());
}

void DatabaseSchema::createTableIfNotExists(const QMetaObject *metaObject)
{
    if (!existsTable(metaObject))
        createTable(metaObject);
}

void DatabaseSchema::dropTable(const QMetaObject *metaObject)
{
    QDataSuite::MetaObject meta = QDataSuite::MetaObject::metaObject(metaObject);

    d->query.clear();
    d->query.setTable( meta.tableName());
    d->query.prepareDropTable();

    if ( !d->query.exec()
         || d->query.lastError().isValid()) {
        setLastError(d->query);
    }
}

void DatabaseSchema::createTable(const QMetaObject *metaObject)
{
    QDataSuite::MetaObject meta = QDataSuite::MetaObject::metaObject(metaObject);

    d->query.clear();
    d->query.setTable(meta.tableName());

    int count = metaObject->propertyCount();
    for (int i=1; i < count; ++i) { // start at 1 because 0 is "objectName"
        QDataSuite::MetaProperty metaProperty(metaObject->property(i), meta);

        if (!metaProperty.isStored())
            continue;

        if(!metaProperty.isRelationProperty()) {
            QString columnName = metaProperty.columnName();
            QString columnType = DatabaseSchema::variantTypeToSqlType(metaProperty.type());

            if (metaProperty.isPrimaryKey()) {
                columnType.append(QLatin1String(" PRIMARY KEY"));
            }

            if(metaProperty.isAutoIncremented()) {
                columnType.append(QLatin1String(" AUTO INCREMENT"));
            }

            d->query.addField(columnName, columnType);
        }
        else {
            if(metaProperty.tableName() == meta.tableName()) {
                QDataSuite::MetaProperty reverseRelation = metaProperty.reverseRelation();
                if(reverseRelation.isValid()) {
                    QDataSuite::MetaObject reverseMetaObject = reverseRelation.metaObject();
                    QString columnName = metaProperty.columnName();
                    QString columnType = DatabaseSchema::variantTypeToSqlType(reverseMetaObject.primaryKeyProperty().type());

                    d->query.addField(columnName, columnType);
                    d->query.addForeignKey(metaProperty.columnName(),
                                           reverseMetaObject.primaryKeyProperty().columnName(),
                                           reverseMetaObject.tableName());
                }
            }
        }
    }

    d->query.prepareCreateTable();

    if ( !d->query.exec()
         || d->query.lastError().isValid()) {
        setLastError(d->query);
    }
}

bool DatabaseSchema::addMissingColumns(const QMetaObject *metaObject)
{
    QDataSuite::MetaObject meta = QDataSuite::MetaObject::metaObject(metaObject);
    QSqlRecord record = d->database.record(meta.tableName());;

    int count = metaObject->propertyCount();
    for (int i = 1; i < count; ++i) {
        QDataSuite::MetaProperty metaProperty(metaObject->property(i), meta);

        if (!metaProperty.isStored())
            continue;

        if (record.indexOf(metaProperty.columnName()) != -1)
            continue;

        addColumn(metaProperty);

        if (lastError().isValid())
            return false;
    }

    return true;
}

void DatabaseSchema::addColumn(const QDataSuite::MetaProperty &metaProperty)
{
    QString tableName;
    QString name;
    QString type;

    if(metaProperty.isRelationProperty()) {
        name = metaProperty.columnName();
        tableName = metaProperty.tableName();
        type = DatabaseSchema::variantTypeToSqlType(metaProperty.foreignKeyType());

        qWarning("The relation %s will be added to the %s table. "
                 "But SQLite does not support adding foreign key contraints after a table has been created. "
                 "You might want to alter the table manually (by creating a new table and copying the original data).",
                 qPrintable(name),
                 qPrintable(tableName));
    }
    else {
        tableName = metaProperty.metaObject().tableName();
        name = metaProperty.columnName();
        type = DatabaseSchema::variantTypeToSqlType(metaProperty.type());
    }

    d->query.clear();
    d->query.setTable(tableName);
    d->query.addField(name, type);
    d->query.prepareAlterTable();

    if ( !d->query.exec()
         || d->query.lastError().isValid()) {
        setLastError(d->query);
    }
}

void DatabaseSchema::createCleanSchema()
{
    foreach(const QDataSuite::MetaObject metaObject, QDataSuite::MetaObject::registeredMetaObjects()) {
        if(existsTable(&metaObject))
            dropTable(&metaObject);
        createTable(&metaObject);
    }
}

void DatabaseSchema::adjustSchema()
{
    foreach(const QDataSuite::MetaObject metaObject, QDataSuite::MetaObject::registeredMetaObjects()) {
        createTableIfNotExists(&metaObject);
        addMissingColumns(&metaObject);
    }
}

QDataSuite::Error DatabaseSchema::lastError() const
{
    return d->lastError;
}

QString DatabaseSchema::variantTypeToSqlType(QVariant::Type type)
{
    switch (type) {
    case QVariant::UInt:
    case QVariant::Int:
    case QVariant::Bool:
    case QVariant::ULongLong:
        return QLatin1String("INTEGER");
    case QVariant::String:
    case QVariant::StringList:
    case QVariant::Date:
    case QVariant::DateTime:
    case QVariant::Time:
    case QVariant::Char:
    case QVariant::Url:
        return QLatin1String("TEXT");
    case QVariant::Double:
        return QLatin1String("REAL");
    default:
        return QLatin1String("BLOB");
    }
}

void DatabaseSchema::setLastError(const QDataSuite::Error &error) const
{
    qDebug() << error;
    d->lastError = error;
}

void DatabaseSchema::setLastError(const QSqlQuery &query) const
{
    setLastError(QDataSuite::Error(query.lastError().text().append(": ").append(query.executedQuery()), QDataSuite::Error::SqlError));
}

QString DatabaseSchemaPrivate::metaPropertyToColumnDefinition(const QDataSuite::MetaProperty &metaProperty)
{
    QString name;
    QString type;

    if(metaProperty.isRelationProperty()) {
        QDataSuite::MetaProperty reverseRelation = metaProperty.reverseRelation();
        name = metaProperty.columnName();
        type = DatabaseSchema::variantTypeToSqlType(reverseRelation.metaObject().primaryKeyProperty().type());

        switch(metaProperty.cardinality()) {
        case QDataSuite::MetaProperty::ToOneCardinality:
        case QDataSuite::MetaProperty::OneToOneCardinality:
        case QDataSuite::MetaProperty::ManyToOneCardinality:
            // My table gets a foreign key column
            break;
        case QDataSuite::MetaProperty::ToManyCardinality:
        case QDataSuite::MetaProperty::OneToManyCardinality:
            // The related table gets a foreign key column
            return QString();
        case QDataSuite::MetaProperty::ManyToManyCardinality:
            // The relation need a whole table
            qDebug() << "Many to many relations are not supported yet.";
            return QString();
        default:
        case QDataSuite::MetaProperty::NoCardinality:
            // This is BAD
            Q_ASSERT_X(false, Q_FUNC_INFO,
                       QString("The relation %1 has no cardinality. This is an internal error and should never happen.")
                       .arg(metaProperty.name())
                       .toLatin1());
            return QString();
        }
    }
    else {
        name = metaProperty.columnName();
        type = DatabaseSchema::variantTypeToSqlType(metaProperty.type());
    }

    if(name.isEmpty()
            || type.isEmpty()) {
        return QString();
    }

    return QString("\"%2\" %3")
            .arg(name)
            .arg(type);
}

} // namespace QPersistence
