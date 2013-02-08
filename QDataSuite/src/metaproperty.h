#ifndef QDATASUITE_METAPROPERTY_H
#define QDATASUITE_METAPROPERTY_H

#include <QtCore/QMetaProperty>

#include <QtCore/QSharedDataPointer>

struct QMetaObject;

namespace QDataSuite {

class MetaObject;

class MetaPropertyPrivate;
class MetaProperty : public QMetaProperty
{
public:
    enum Cardinality {
        NoCardinality,
        ToOneCardinality,
        ToManyCardinality,
        OneToOneCardinality,
        OneToManyCardinality,
        ManyToOneCardinality,
        ManyToManyCardinality
    };

    MetaProperty(const QString &propertyName, const MetaObject &metaObject);
    MetaProperty(const QMetaProperty &property, const MetaObject &metaObject);
    ~MetaProperty();
    MetaProperty(const MetaProperty &other);
    MetaProperty &operator = (const MetaProperty &other);

    MetaObject metaObject() const;

    QString columnName() const;
    bool isAutoIncremented() const;
    bool isReadOnly() const;
    bool isPrimaryKey() const;

    // Relations
    bool isRelationProperty() const;
    bool isToOneRelationProperty() const;
    bool isToManyRelationProperty() const;
    Cardinality cardinality() const;

    QString reverseClassName() const;
    MetaObject reverseMetaObject() const;
    QString reverseRelationName() const;
    MetaProperty reverseRelation() const;

    QString tableName() const;
    QVariant::Type foreignKeyType() const;

    bool write(QObject *obj, const QVariant &value) const;

private:
    QSharedDataPointer<MetaPropertyPrivate> d;
};

} // namespace QDataSuite

#endif // QDATASUITE_METAPROPERTY_H
