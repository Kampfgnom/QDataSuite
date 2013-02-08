#include "metaproperty.h"

#include "metaobject.h"

#include <QMetaClassInfo>
#include <QRegularExpression>
#include <QStringList>
#include <QDebug>

namespace QDataSuite {

class MetaPropertyPrivate : public QSharedData
{
public:
    MetaPropertyPrivate() :
        QSharedData()
    {}

    MetaObject metaObject;
    QHash<QString, QString> attributes;

    MetaProperty *q;

    void parseClassInfo();
};

void MetaPropertyPrivate::parseClassInfo()
{
    QString classInfoName = QString(QDATASUITE_PROPERTYMETADATA).append(":").append(q->name());
    QString classInfoRawValue = metaObject.classInformation(classInfoName.toLatin1(), QString());

    // First parse the attributes
    QRegularExpression reg("(\\w+)=(\\w+)");
    QStringList attributesList = classInfoRawValue.split(';');
    foreach(const QString attribute, attributesList) {
        QRegularExpressionMatch match = reg.match(attribute);
        if(!match.hasMatch())
            continue;

        attributes.insert( match.captured(1), match.captured(2) );
    }
}

MetaProperty::MetaProperty(const QString &propertyName, const MetaObject &metaObject) :
    QMetaProperty(metaObject.property(metaObject.indexOfProperty(propertyName.toLatin1()))),
    d(new MetaPropertyPrivate)
{
    d->q = this;
    d->metaObject = metaObject;
    d->parseClassInfo();
}

MetaProperty::MetaProperty(const QMetaProperty &property, const MetaObject &metaObject) :
    QMetaProperty(property),
    d(new MetaPropertyPrivate)
{
    d->q = this;
    d->metaObject = metaObject;
    d->parseClassInfo();
}

MetaProperty::~MetaProperty()
{
}

MetaProperty::MetaProperty(const MetaProperty &other) :
    QMetaProperty(other),
    d(other.d)
{
}

MetaProperty &MetaProperty::operator =(const MetaProperty &other)
{
    QMetaProperty::operator=(other);

    if(&other != this)
        d.operator =(other.d);

    return *this;
}

MetaObject MetaProperty::metaObject() const
{
    return d->metaObject;
}

bool MetaProperty::isAutoIncremented() const
{
    return d->attributes.value(QDATASUITE_PROPERTYMETADATA_AUTOINCREMENTED) == QLatin1String(QDATASUITE_TRUE);
}

QString MetaProperty::columnName() const
{
    if(d->attributes.contains(QDATASUITE_PROPERTYMETADATA_SQL_COLUMNNAME))
        return d->attributes.value(QDATASUITE_PROPERTYMETADATA_SQL_COLUMNNAME);

    if(isToManyRelationProperty()) {
        QString result = QString(name());

        MetaProperty reverse(reverseRelation());
        if(reverse.isValid()) {
            result = QString(reverse.name());
        }

        result.append("_fk_").append(reverseMetaObject().primaryKeyPropertyName());
        return result;
    }
    else if(isToOneRelationProperty()) {
        return QString(name())
                .append("_fk_")
                .append(reverseMetaObject().primaryKeyPropertyName());
    }

    return QString(name());
}

bool MetaProperty::isReadOnly() const
{
    return d->attributes.value(QDATASUITE_PROPERTYMETADATA_READONLY) == QLatin1String(QDATASUITE_TRUE);
}

bool MetaProperty::isPrimaryKey() const
{
    return d->metaObject.primaryKeyPropertyName() == QLatin1String(name());
}

bool MetaProperty::isRelationProperty() const
{
    return isToOneRelationProperty() || isToManyRelationProperty();
}

bool MetaProperty::isToOneRelationProperty() const
{
    return QString(typeName()).endsWith('*');
}

bool MetaProperty::isToManyRelationProperty() const
{
    static QRegularExpression regExp("QList<(\\w+)\\*>");
    return regExp.match(QLatin1String(typeName())).hasMatch();
}

MetaProperty::Cardinality MetaProperty::cardinality() const
{
    MetaProperty reverse = reverseMetaObject().metaProperty(reverseRelationName());

    if(isToOneRelationProperty()) {
        if(!reverse.isValid() ||
                QString(reverse.typeName()).isEmpty()) {
            return ToOneCardinality;
        }
        else if(reverse.isToOneRelationProperty()) {
            return OneToOneCardinality;
        }
        else if(reverse.isToManyRelationProperty()) {
            return ManyToOneCardinality;
        }
    }
    else if(isToManyRelationProperty()) {
        if(!reverse.isValid() ||
                QString(reverse.typeName()).isEmpty()) {
            return ToManyCardinality;
        }
        else if(reverse.isToManyRelationProperty()) {
            return ManyToManyCardinality;
        }
        else if(reverse.isToOneRelationProperty()) {
            return OneToManyCardinality;
        }
    }

    Q_ASSERT_X(false, Q_FUNC_INFO,
               QString("The relation %1 has no cardinality. This is an internal error and should never happen.")
               .arg(name())
               .toLatin1());
    return NoCardinality;
}

QString MetaProperty::reverseClassName() const
{
    QString name(typeName());
    if(isToOneRelationProperty()) {
        return name.left(name.length() - 1);
    }

    static QRegularExpression regExp("QList\\<(\\w*)\\*\\>");
    return regExp.match(name).captured(1);
}

MetaObject MetaProperty::reverseMetaObject() const
{
    return MetaObject::metaObject(reverseClassName());
}

QString MetaProperty::reverseRelationName() const
{
    return d->attributes.value(QDATASUITE_PROPERTYMETADATA_REVERSERELATION);
}

MetaProperty MetaProperty::reverseRelation() const
{
    return reverseMetaObject().metaProperty(reverseRelationName());
}

QString MetaProperty::tableName() const
{
    if(!isRelationProperty())
        return metaObject().tableName();

    switch(cardinality()) {
    case QDataSuite::MetaProperty::ToOneCardinality:
    case QDataSuite::MetaProperty::ManyToOneCardinality:
        // My table gets a foreign key column
        return metaObject().tableName();

    case QDataSuite::MetaProperty::ToManyCardinality:
    case QDataSuite::MetaProperty::OneToManyCardinality:
        // The related table gets a foreign key column
        return reverseMetaObject().tableName();

    case QDataSuite::MetaProperty::OneToOneCardinality:
        Q_ASSERT_X(false, Q_FUNC_INFO, "OneToOneCardinality relations are not supported yet.");

    case QDataSuite::MetaProperty::ManyToManyCardinality:
        Q_ASSERT_X(false, Q_FUNC_INFO, "ManyToManyCardinality relations are not supported yet.");

    case QDataSuite::MetaProperty::NoCardinality:
    default:
        // This is BAD and should have asserted in cardinality()
        Q_ASSERT(false);
    }

    return QString();
}

QVariant::Type MetaProperty::foreignKeyType() const
{
    if(!isRelationProperty())
        return QVariant::Invalid;

    switch(cardinality()) {
    case QDataSuite::MetaProperty::ToOneCardinality:
    case QDataSuite::MetaProperty::ManyToOneCardinality:
        // My table gets a foreign key column: I.e. the foreign key type is the type of the related primary key
        return reverseMetaObject().primaryKeyProperty().type();

    case QDataSuite::MetaProperty::ToManyCardinality:
    case QDataSuite::MetaProperty::OneToManyCardinality:
        // The related table gets a foreign key column: I.e. the foreign key type is the type of my primary key
        return metaObject().primaryKeyProperty().type();

    case QDataSuite::MetaProperty::OneToOneCardinality:
        Q_ASSERT_X(false, Q_FUNC_INFO, "OneToOneCardinality relations are not supported yet.");

    case QDataSuite::MetaProperty::ManyToManyCardinality:
        Q_ASSERT_X(false, Q_FUNC_INFO, "ManyToManyCardinality relations are not supported yet.");

    case QDataSuite::MetaProperty::NoCardinality:
    default:
        // This is BAD and should have asserted in cardinality()
        Q_ASSERT(false);
    }

    return QVariant::Invalid;
}

bool MetaProperty::write(QObject *obj, const QVariant &value) const
{
    if (!isWritable())
        return false;

    QVariant::Type t = type();
    if (value.canConvert(t)) {
        QVariant v(value);
        v.convert(t);
        return QMetaProperty::write( obj, v );
    }

    return QMetaProperty::write( obj, value );
}

} // namespace QDataSuite
