#ifndef QPERSISTENCE_SQLQUERY_H
#define QPERSISTENCE_SQLQUERY_H

#include <QtSql/QSqlQuery>

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QVariant>

namespace QPersistence {

class SqlCondition;

class SqlQueryPrivate;
class SqlQuery : public QSqlQuery
{
public:
    enum Order {
        Ascending,
        Descending
    };

    SqlQuery();
    SqlQuery(const QSqlDatabase &database);
    SqlQuery(const SqlQuery &);
    SqlQuery &operator=(const SqlQuery &);
    ~SqlQuery();

    bool exec();

    QString table() const;

    void clear();
    void setTable(const QString &table);
    void addField(const QString &name, const QVariant &value = QVariant());
    void addForeignKey(const QString &columnName, const QString &keyName, const QString &foreignTableName);
    void setLimit(int limit);
    void setWhereCondition(const SqlCondition &condition);
    void addOrder(const QString &field, Order order = Ascending);

    void prepareCreateTable();
    void prepareDropTable();
    void prepareAlterTable();

    void prepareSelect();
    bool prepareUpdate();
    void prepareInsert();
    void prepareDelete();

private:
    QExplicitlySharedDataPointer<SqlQueryPrivate> d;
};

} // namespace QPersistence

#endif // QPERSISTENCE_SQLQUERY_H
