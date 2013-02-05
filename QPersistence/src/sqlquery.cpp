#include "sqlquery.h"

#include "sqlcondition.h"

#include <QSharedData>
#include <QStringList>
#include <QHash>
#include <QDebug>
#include <QRegularExpressionMatchIterator>
#define COMMA ,

namespace QPersistence {

class SqlQueryPrivate : public QSharedData {
public:
    QString table;
    QHash<QString, QVariant> fields;
    int limit;
    SqlCondition whereCondition;
    QList<QPair<QString, SqlQuery::Order> > orderBy;
    QList<QStringList> foreignKeys;
};

SqlQuery::SqlQuery() :
    QSqlQuery(),
    d(new SqlQueryPrivate)
{
    d->limit = -1;
}

SqlQuery::SqlQuery(const QSqlDatabase &database) :
    QSqlQuery(database),
    d(new SqlQueryPrivate)
{
}

SqlQuery::SqlQuery(const SqlQuery &rhs) :
    QSqlQuery(rhs),
    d(rhs.d)
{
}

SqlQuery &SqlQuery::operator=(const SqlQuery &rhs)
{
    QSqlQuery::operator =(rhs);

    if (this != &rhs)
        d.operator=(rhs.d);

    return *this;
}

SqlQuery::~SqlQuery()
{
}

bool SqlQuery::exec()
{
    bool ok = QSqlQuery::exec();
    QString query = executedQuery();
    int index = query.indexOf('?');
    int i = 0;
    QList<QVariant> values = boundValues().values();
    while (index >= 0) {
        QString value = values.at(i).toString();
        if(value.isEmpty())
            value = QLatin1String("NULL");
        query.replace(index, 1, value);
        index = query.indexOf('?', index + value.length());
        ++i;
    }
    qDebug() << qPrintable(query);
    return ok;
}

QString SqlQuery::table() const
{
    return d->table;
}

void SqlQuery::clear()
{
    QSqlQuery::clear();

    d->table = QString();
    d->fields.clear();
    d->limit = -1;
    d->whereCondition = SqlCondition();
    d->orderBy.clear();
}

void SqlQuery::setTable(const QString &table)
{
    d->table = table;
}

void SqlQuery::addField(const QString &name, const QVariant &value)
{
    d->fields.insert(name, value);
}

void SqlQuery::addForeignKey(const QString &columnName, const QString &keyName, const QString &foreignTableName)
{
    d->foreignKeys.append(QStringList() << columnName << keyName << foreignTableName);
}

void SqlQuery::setLimit(int limit)
{
    d->limit = limit;
}

void SqlQuery::setWhereCondition(const SqlCondition &condition)
{
    d->whereCondition = condition;
}

void SqlQuery::addOrder(const QString &field, SqlQuery::Order order)
{
    d->orderBy.append(QPair<QString, SqlQuery::Order>(field, order));
}

void SqlQuery::prepareCreateTable()
{
    QString query("CREATE TABLE \"");
    query.append(d->table).append("\" (\n\t");

    QStringList fields;
    QHashIterator<QString, QVariant> it(d->fields);
    while(it.hasNext()) {
        it.next();
        fields.append(QString("\"%1\" %2")
                      .arg(it.key())
                      .arg(it.value().toString()));
    }
    query.append(fields.join(",\n\t"));

    foreach(const QStringList foreignKey, d->foreignKeys) {
        query.append(QString(",\n\tFOREIGN KEY (%1) REFERENCES %2(%3)")
                     .arg(foreignKey.first())
                     .arg(foreignKey.last())
                     .arg(foreignKey.at(1)));
    }

    query.append("\n);");

    QSqlQuery::prepare(query);
}

void SqlQuery::prepareDropTable()
{
    QString query("DROP TABLE \"");
    query.append(d->table).append("\";");

    QSqlQuery::prepare(query);
}

void SqlQuery::prepareAlterTable()
{
    QSqlQuery::prepare(QString("ALTER TABLE \"%1\" ADD COLUMN \"%2\" %3;")
                       .arg(d->table)
                       .arg(d->fields.keys().first())
                       .arg(d->fields.values().first().toString()));
}

void SqlQuery::prepareSelect()
{
    QString query("SELECT ");

    if(d->fields.isEmpty()) {
        query.append("*");
    }
    else {
        QStringList fields;
        foreach(const QString &field, d->fields.keys()) {
            fields.append(QString("\"%1\"").arg(field));
        }
        query.append(fields.join(", "));
    }

    query.append(" FROM \"").append(d->table).append("\"");

    if(d->whereCondition.isValid()) {
        query.append("\n\tWHERE ").append(d->whereCondition.toWhereClause());
    }

    if(!d->orderBy.isEmpty()) {
        query.append("\n\tORDER BY ");
        QStringList orderClauses;
        foreach(QPair<QString COMMA SqlQuery::Order> order, d->orderBy) {
            QString orderClause = order.first.prepend("\n\t\t");
            if(order.second == SqlQuery::Descending)
                orderClause.append(" DESC");
            else
                orderClause.append(" ASC");
            orderClauses.append(orderClause);
        }
        query.append(orderClauses.join(','));
    }

    if(d->limit >= 0) {
        query.append(QString("\n\tLIMIT %1").arg(d->limit));
    }

    query.append(';');
    QSqlQuery::prepare(query);

    foreach(const QVariant value, d->whereCondition.bindValues()) {
        QSqlQuery::addBindValue(value);
    }
}

bool SqlQuery::prepareUpdate()
{
    if(d->fields.isEmpty())
        return false;

    QString query("UPDATE \"");
    query.append(d->table).append("\" SET\n\t");

    QStringList fields;
    foreach(const QString &field, d->fields.keys()) {
        fields.append(QString("\"%1\" = ?").arg(field));
    }
    query.append(fields.join(",\n\t"));

    if(d->whereCondition.isValid()) {
        query.append("\n\tWHERE ").append(d->whereCondition.toWhereClause());
    }

    query.append(';');
    QSqlQuery::prepare(query);

    foreach(const QVariant value, d->fields.values()) {
        QSqlQuery::addBindValue(value);
    }
    foreach(const QVariant value, d->whereCondition.bindValues()) {
        QSqlQuery::addBindValue(value);
    }

    return true;
}

void SqlQuery::prepareInsert()
{
    QString query("INSERT INTO \"");
    query.append(d->table).append("\"\n\t(");

    QStringList fields;
    foreach(const QString &field, d->fields.keys()) {
        fields.append(QString("\"%1\"").arg(field));
    }
    query.append(fields.join(", "));

    query.append(")\n\tVALUES (");
    query.append(QString("?, ").repeated(fields.size() - 1));
    if(fields.size() > 0)
        query.append("?");

    query.append(");");

    QSqlQuery::prepare(query);

    foreach(const QVariant value, d->fields.values()) {
        QSqlQuery::addBindValue(value);
    }
}

void SqlQuery::prepareDelete()
{
    QString query("DELETE FROM \"");
    query.append(d->table).append("\"\n\tWHERE ");

    if(d->whereCondition.isValid()) {
        query.append(d->whereCondition.toWhereClause());
    }

    query.append(';');
    QSqlQuery::prepare(query);

    foreach(const QVariant value, d->whereCondition.bindValues()) {
        QSqlQuery::addBindValue(value);
    }
}

} // namespace QPersistence

#undef COMMA
