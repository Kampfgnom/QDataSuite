#include "serializer.h"

#include <QDataSuite/error.h>
#include <QDataSuite/metaproperty.h>
#include <QDataSuite/metaobject.h>

#include <QHash>

namespace QRestServer {

class SerializerPrivate : public QSharedData
{
public:
    SerializerPrivate() :
        QSharedData()
    {}

    mutable QDataSuite::Error lastError;
    QString format;
    QString contentType;

    static QHash<QString, Serializer *> serializers;
};

QHash<QString, Serializer *> SerializerPrivate::serializers;

Serializer::Serializer(const QString &format, const QString &contentType) :
    d(new SerializerPrivate)
{
    d->format = format;
    d->contentType = contentType;
}

Serializer::~Serializer()
{
}

QString Serializer::contentType() const
{
    return d->contentType;
}

QString Serializer::format() const
{
    return d->format;
}

QDataSuite::Error Serializer::lastError() const
{
    return d->lastError;
}

void Serializer::setLastError(const QDataSuite::Error &error) const
{
    d->lastError = error;
}

void Serializer::resetLastError() const
{
    setLastError(QDataSuite::Error());
}

QVariantMap Serializer::objectToVariant(const QObject *object)
{
    QVariantMap result;
    const QDataSuite::MetaObject metaObject = QDataSuite::MetaObject::metaObject(object);
    int count = metaObject.propertyCount();
    for (int i = 1; i < count; ++i) {
        QDataSuite::MetaProperty metaProperty(metaObject.property(i), metaObject);

        if (!metaProperty.isReadable()
                || !metaProperty.isStored()
                || metaProperty.isRelationProperty())
            continue;

        result[QLatin1String(metaProperty.name())] = object->property(metaProperty.name());
    }

    QList<QByteArray> dynamicPropertyNames = object->dynamicPropertyNames();
    foreach(const QByteArray dynamicPropertyName, dynamicPropertyNames) {
        result[QString(dynamicPropertyName)] = object->property(dynamicPropertyName);
    }

    return result;
}

Serializer *Serializer::forFormat(const QString &format)
{
    return SerializerPrivate::serializers.value(format);
}

void Serializer::registerSerializer(Serializer *serializer)
{
    if(!SerializerPrivate::serializers.contains(QString()))
        setDefaultSerializer(serializer);

    SerializerPrivate::serializers.insert(serializer->format(), serializer);
}

void Serializer::setDefaultSerializer(Serializer *serializer)
{
    SerializerPrivate::serializers.insert(QString(), serializer);
}

} // namespace QRestServer

