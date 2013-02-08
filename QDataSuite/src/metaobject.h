#ifndef QDATASUITE_METAOBJECT_H
#define QDATASUITE_METAOBJECT_H

#include <QtCore/QMetaObject>

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QSharedDataPointer>

#define QDATASUITE_PRIMARYKEY "QDATASUITE_PRIMARYKEY"
#define QDATASUITE_SQL_TABLENAME "QDATASUITE_SQL_TABLENAME"
#define QDATASUITE_REST_COLLECTIONNAME "QDATASUITE_REST_COLLECTIONNAME"

#define QDATASUITE_PROPERTYMETADATA "QDATASUITE_PROPERTYMETADATA"
#define QDATASUITE_PROPERTYMETADATA_REVERSERELATION "reverserelation"
#define QDATASUITE_PROPERTYMETADATA_AUTOINCREMENTED "autoincremented"
#define QDATASUITE_PROPERTYMETADATA_READONLY "readonly"
#define QDATASUITE_PROPERTYMETADATA_SQL_COLUMNNAME "columnname"

#define QDATASUITE_TRUE "true"
#define QDATASUITE_FALSE "false"


template<class K, class V>
class QHash;
class QVariant;

namespace QDataSuite {

class MetaProperty;
class ConverterBase;

class MetaObjectPrivate;
class MetaObject : public QMetaObject
{
public:
    MetaObject();
    ~MetaObject();
    MetaObject(const MetaObject &other);
    MetaObject &operator = (const MetaObject &other);

    QString classInformation(const QString &informationName, bool assertNotEmpty = false) const;
    QString classInformation(const QString &informationName, const QString &defaultValue) const;
    bool hasMetaProperty(const QString &name) const;
    MetaProperty metaProperty(const QString &name) const;
    QString tableName() const;
    QString collectionName() const;
    QString primaryKeyPropertyName() const;
    MetaProperty primaryKeyProperty() const;
    QList<MetaProperty> simpleProperties() const;
    QList<MetaProperty> relationProperties() const;


    static void registerMetaObject(const QMetaObject &metaObject);
    static QList<MetaObject> registeredMetaObjects();
    static void registerConverter(int variantType, ConverterBase *converter);
    static QObject *objectCast(const QVariant &variant);
    static QList<QObject *> objectListCast(const QVariant &variant);
    static QVariant variantCast(QObject *object, const QString &className = QString());
    static QVariant variantListCast(QList<QObject *> objects, const QString &className);

    static MetaObject metaObject(const QMetaObject *metaObject);
    static MetaObject metaObject(const QMetaObject &metaObject);
    static MetaObject metaObject(const QString &className);
    static MetaObject metaObject(const QObject *object);

private:
    QSharedDataPointer<MetaObjectPrivate> d;

    MetaObject(const QMetaObject &metaObject);
};

class ConverterBase : public QObject
{
public:
    ConverterBase(QObject *parent = 0) : QObject(parent) {}
    virtual ~ConverterBase() {}
    virtual QList<QObject *> convertList(const QVariant &variant) const = 0;
    virtual QObject *convertObject(const QVariant &variant) const = 0;
    virtual QVariant convertVariant(QObject *object) const = 0;
    virtual QVariant convertVariantList(QList<QObject *> objects) const = 0;
    virtual QString className() const = 0;
};

template<class O>
class Converter : public ConverterBase
{
public:
    Converter(QObject *parent = 0) : ConverterBase(parent) {}
    QList<QObject *> convertList(const QVariant &variant) const
    {
        QList<O *> list = variant.value<QList<O *> >();
        QList<QObject *> result;
        Q_FOREACH(O *object, list) result.append(object);
        return result;
    }

    QObject *convertObject(const QVariant &variant) const
    {
        return variant.value<O *>();
    }

    QVariant convertVariant(QObject *object) const
    {
        return QVariant::fromValue<O *>(static_cast<O *>(object));
    }

    QVariant convertVariantList(QList<QObject *> objects) const
    {
        QList<O *> result;
        Q_FOREACH(QObject *object, objects) result.append(static_cast<O *>(object));
        return QVariant::fromValue<QList<O *> >(result);
    }

    QString className() const
    {
        return QLatin1String(O::staticMetaObject.className());
    }

};

template<class T>
void registerMetaObject()
{
    static QObject guard;
    MetaObject::registerMetaObject(T::staticMetaObject);

    QVariant v = QVariant::fromValue<T *>(0);
    Converter<T> *converter = new Converter<T>(&guard);
    MetaObject::registerConverter(v.userType(), converter);

    v = QVariant::fromValue<QList<T *> >(QList<T *>());
    MetaObject::registerConverter(v.userType(), converter);
}

} // namespace QDataSuite

#endif // QDATASUITE_METAOBJECT_H
