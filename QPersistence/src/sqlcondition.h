#ifndef QPERSISTENCE_SQLCONDITION_H
#define QPERSISTENCE_SQLCONDITION_H

#include <QtCore/QSharedDataPointer>

#include <QtCore/QVariantList>

class QVariant;

namespace QPersistence {

class SqlConditionData;

class SqlCondition
{
public:
    enum BooleanOperator {
        And,
        Or,
        Not
    };

    enum ComparisonOperator {
        EqualTo,
        GreaterThan,
        LessThan,
        GreaterThanOrEqualTo,
        LessThanOrEqualTo,
        NotEqualTo
    };

    SqlCondition();
    SqlCondition(const QString &key, ComparisonOperator op, const QVariant &value);
    SqlCondition(BooleanOperator op, const QList<SqlCondition> &conditions);

    bool isValid() const;

    SqlCondition(const SqlCondition &);
    SqlCondition &operator=(const SqlCondition &);
    ~SqlCondition();

    SqlCondition operator !();
    SqlCondition operator ||(const SqlCondition &rhs);
    SqlCondition operator &&(const SqlCondition &rhs);

    QString toWhereClause(bool bindValues = true) const;
    QVariantList bindValues() const;

    QString booleanOperator() const;
    QString comparisonOperator() const;

private:
    QSharedDataPointer<SqlConditionData> d;
};

} // namespace QPersistence

#endif // QPERSISTENCE_SQLCONDITION_H
