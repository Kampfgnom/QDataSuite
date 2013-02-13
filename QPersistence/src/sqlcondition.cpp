#include "sqlcondition.h"

#include <QSharedData>
#include <QList>
#include <QStringList>
#include <QVariant>
#include <QVariantList>

namespace QPersistence {

class SqlConditionData : public QSharedData {
public:
    QString key;
    QVariant value;
    SqlCondition::BooleanOperator booleanOperator;
    SqlCondition::ComparisonOperator comparisonOperator;
    QList<SqlCondition> conditions;
};

SqlCondition::SqlCondition() :
    d(new SqlConditionData)
{
}

SqlCondition::SqlCondition(const QString &key, SqlCondition::ComparisonOperator op, const QVariant &value) :
    d(new SqlConditionData)
{
    d->key = key;
    d->booleanOperator = And;
    d->comparisonOperator = op;
    d->value = value;
}

SqlCondition::SqlCondition(SqlCondition::BooleanOperator op, const QList<SqlCondition> &conditions) :
    d(new SqlConditionData)
{
    d->booleanOperator = op;
    d->conditions = conditions;
    d->comparisonOperator = EqualTo;
}

bool SqlCondition::isValid() const
{
    return !d->key.isEmpty()
            || (d->booleanOperator == Not
                && d->conditions.size() == 1)
            || !d->conditions.isEmpty();
}

SqlCondition::SqlCondition(const SqlCondition &rhs) :
    d(rhs.d)
{
}

SqlCondition &SqlCondition::operator=(const SqlCondition &rhs)
{
    if (this != &rhs)
        d.operator=(rhs.d);

    return *this;
}

SqlCondition::~SqlCondition()
{
}

SqlCondition SqlCondition::operator !()
{
    SqlCondition result(SqlCondition::Not, QList<SqlCondition>() << *this);
    return result;
}

SqlCondition SqlCondition::operator ||(const SqlCondition &rhs)
{
    return SqlCondition(SqlCondition::Or, QList<SqlCondition>() << *this << rhs);
}

SqlCondition SqlCondition::operator &&(const SqlCondition &rhs)
{
    return SqlCondition(SqlCondition::And, QList<SqlCondition>() << *this << rhs);
}

QString SqlCondition::toWhereClause(bool bindValues) const
{
    if(d->booleanOperator == Not) {
        Q_ASSERT(d->conditions.size() == 1);

        return booleanOperator().append(d->conditions.first().toWhereClause());
    }

    if(!d->conditions.isEmpty()) {
        QStringList conditions;
        foreach(const SqlCondition &condition, d->conditions) {
            conditions.append(condition.toWhereClause(bindValues));
        }

        QString result = conditions.join(booleanOperator());
        if(d->conditions.size() > 1)
            result = result.prepend('(').append(')');

        return result;
    }

    Q_ASSERT(!d->key.isEmpty());

    return comparisonOperator().prepend(QString("\"%1\"").arg(d->key)).append("?");
}

QVariantList SqlCondition::bindValues() const
{
    QVariantList result;

    foreach(const SqlCondition& condition, d->conditions) {
        result.append(condition.bindValues());
    }

    if(!d->key.isEmpty())
        result.append(d->value);

    return result;
}

QString SqlCondition::booleanOperator() const
{
    switch(d->booleanOperator) {
    case And:
        return " AND ";
    case Or:
        return " OR ";
    case Not:
        return "NOT ";
    }
}

QString SqlCondition::comparisonOperator() const
{
    switch(d->comparisonOperator) {
    case EqualTo:
        return " = ";
    case GreaterThan:
        return " > ";
    case LessThan:
        return " < ";
    case GreaterThanOrEqualTo:
        return " >= ";
    case LessThanOrEqualTo:
        return " <= ";
    case NotEqualTo:
        return " <> ";
    }
}

} // namespace QPersistence
