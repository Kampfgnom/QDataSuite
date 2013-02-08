#include "metaobject.h"

#include "metaproperty.h"

#include <QString>
#include <QDebug>
#include <QMetaClassInfo>
#include <QHash>

namespace QDataSuite {

class MetaObjectPrivate : public QSharedData
{
public:
    MetaObjectPrivate() :
        QSharedData()
    {}

    static QHash<QString, MetaObject> metaObjects;
    static QHash<int, ConverterBase *> convertersByUserType;
    static QHash<QString, ConverterBase *> convertersByClassName;

    void parseClassInfo();
};

QHash<QString, MetaObject> MetaObjectPrivate::metaObjects;
QHash<int, ConverterBase *> MetaObjectPrivate::convertersByUserType;
QHash<QString, ConverterBase *> MetaObjectPrivate::convertersByClassName;

void MetaObjectPrivate::parseClassInfo()
{
}

void MetaObject::registerMetaObject(const QMetaObject &metaObject)
{
    Q_ASSERT_X(!MetaObjectPrivate::metaObjects.contains(QLatin1String(metaObject.className())),
               Q_FUNC_INFO,
               QString("Meta object for class % already registered.").arg(metaObject.className()).toLatin1());

    MetaObjectPrivate::metaObjects.insert(QLatin1String(metaObject.className()),
                                          MetaObject(metaObject));
}

MetaObject MetaObject::metaObject(const QMetaObject *metaObject)
{
    Q_ASSERT_X(MetaObjectPrivate::metaObjects.contains(QLatin1String(metaObject->className())),
               Q_FUNC_INFO,
               QString("No such meta object %1. You have to register your objects.").arg(metaObject->className()).toLatin1());

    return MetaObjectPrivate::metaObjects.value(QLatin1String(metaObject->className()));
}

MetaObject MetaObject::metaObject(const QMetaObject &metaObject)
{
    Q_ASSERT_X(MetaObjectPrivate::metaObjects.contains(QLatin1String(metaObject.className())),
               Q_FUNC_INFO,
               QString("No such meta object %1. You have to register your objects.").arg(metaObject.className()).toLatin1());

    return MetaObjectPrivate::metaObjects.value(QLatin1String(metaObject.className()));
}

MetaObject MetaObject::metaObject(const QString &className)
{
    Q_ASSERT_X(MetaObjectPrivate::metaObjects.contains(className),
               Q_FUNC_INFO,
               QString("No such meta object %1. You have to register your objects.").arg(className).toLatin1());

    return MetaObjectPrivate::metaObjects.value(className);
}

MetaObject MetaObject::metaObject(const QObject *object)
{
    Q_ASSERT(object);

    return MetaObjectPrivate::metaObjects.value(QLatin1String(object->metaObject()->className()));
}

QList<MetaObject> MetaObject::registeredMetaObjects()
{
    return MetaObjectPrivate::metaObjects.values();
}

void MetaObject::registerConverter(int variantType, ConverterBase *converter)
{
    MetaObjectPrivate::convertersByUserType.insert(variantType, converter);
    MetaObjectPrivate::convertersByClassName.insert(converter->className(), converter);
}

QObject *MetaObject::objectCast(const QVariant &variant)
{
    Q_ASSERT(MetaObjectPrivate::convertersByUserType.contains(variant.userType()));

    return MetaObjectPrivate::convertersByUserType.value(variant.userType())->convertObject(variant);
}

QList<QObject *> MetaObject::objectListCast(const QVariant &variant)
{
    Q_ASSERT(MetaObjectPrivate::convertersByUserType.contains(variant.userType()));

    return MetaObjectPrivate::convertersByUserType.value(variant.userType())->convertList(variant);
}

QVariant MetaObject::variantCast(QObject *object, const QString &classN)
{
    QString className = classN;
    if(className.isEmpty()) {
        Q_ASSERT(object);
        className = QLatin1String(object->metaObject()->className());
    }
    Q_ASSERT(MetaObjectPrivate::convertersByClassName.contains(className));
    return MetaObjectPrivate::convertersByClassName.value(className)->convertVariant(object);
}

QVariant MetaObject::variantListCast(QList<QObject *> objects, const QString &className)
{
    Q_ASSERT(MetaObjectPrivate::convertersByClassName.contains(className));
    return MetaObjectPrivate::convertersByClassName.value(className)->convertVariantList(objects);
}

MetaObject::MetaObject(const QMetaObject &metaObject) :
    QMetaObject(metaObject),
    d(new MetaObjectPrivate)
{
    d->parseClassInfo();
}

MetaObject::MetaObject() :
    QMetaObject(),
    d(new MetaObjectPrivate)
{
}

MetaObject::~MetaObject()
{
}

MetaObject::MetaObject(const MetaObject &other) :
    QMetaObject(other),
    d(other.d)
{
}

MetaObject &MetaObject::operator =(const MetaObject &other)
{
    QMetaObject::operator=(other);

    if(this != &other) {
        d = other.d;
    }

    return *this;
}

bool MetaObject::hasMetaProperty(const QString &name) const
{
    int index = indexOfProperty(name.toLatin1());
    return index >= 0;
}

MetaProperty MetaObject::metaProperty(const QString &name) const
{
    int index = indexOfProperty(name.toLatin1());

    Q_ASSERT_X(index >= 0,
               Q_FUNC_INFO,
               qPrintable(QString("The %1 class has no property %2.")
                          .arg(className())
                          .arg(name)));

    return MetaProperty(property(index), *this);
}

QString MetaObject::tableName() const
{
    return MetaObject::classInformation(QDATASUITE_SQL_TABLENAME, QLatin1String(className()));
}

QString MetaObject::collectionName() const
{
    return MetaObject::classInformation(QDATASUITE_REST_COLLECTIONNAME, QLatin1String(className()));
}

QString MetaObject::primaryKeyPropertyName() const
{
    return MetaObject::classInformation(QDATASUITE_PRIMARYKEY, true);
}

MetaProperty MetaObject::primaryKeyProperty() const
{
    return metaProperty(primaryKeyPropertyName());
}

QList<MetaProperty> MetaObject::simpleProperties() const
{
    QList<MetaProperty> result;

    int count = propertyCount();
    for (int i=1; i < count; ++i) { // start at 1 because 0 is "objectName"
        QDataSuite::MetaProperty metaProperty(property(i), *this);

        if(metaProperty.isStored()
                && !metaProperty.isRelationProperty()) {
            result.append(metaProperty);
        }
    }

    return result;
}

QList<MetaProperty> MetaObject::relationProperties() const
{
    QList<MetaProperty> result;

    int count = propertyCount();
    for (int i=1; i < count; ++i) { // start at 1 because 0 is "objectName"
        QDataSuite::MetaProperty metaProperty(property(i), *this);

        if(metaProperty.isStored()
                && metaProperty.isRelationProperty()) {
            result.append(metaProperty);
        }
    }

    return result;
}

QString MetaObject::classInformation(const QString &name, const QString &defaultValue) const
{
    int index = indexOfClassInfo(name.toLatin1());

    if (index < 0)
        return defaultValue;

    return QLatin1String(classInfo(index).value());
}

QString MetaObject::classInformation(const QString &informationName, bool assertNotEmpty) const
{
    QString value = classInformation(informationName, QString());

    if(assertNotEmpty) {
        Q_ASSERT_X(!value.isEmpty(),
                   Q_FUNC_INFO,
                   qPrintable(QString("The %1 class does not define a %2.")
                              .arg(className())
                              .arg(informationName)));
    }

    return value;
}

} // namespace QDataSuite
