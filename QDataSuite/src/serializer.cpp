#include "serializer.h"

#include "error.h"
#include "metaproperty.h"
#include "metaobject.h"

#include <QHash>

namespace QDataSuite {

class SerializerPrivate : public QSharedData
{
public:
    SerializerPrivate() :
        QSharedData()
    {}

    mutable Error lastError;
    QString format;
    QString contentType;

    static QHash<QString, Serializer *> serializers;

    class Guard {
    public:
        ~Guard()
        {
            QHashIterator<QString, Serializer *> it(serializers);
            while(it.hasNext()) {
                it.next();
                delete it.value();
            }
        }
    };
    static Guard guard; // deletes the serializers upon destruction
};

QHash<QString, Serializer *> SerializerPrivate::serializers;
SerializerPrivate::Guard SerializerPrivate::guard;

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

Error Serializer::lastError() const
{
    return d->lastError;
}

void Serializer::setLastError(const Error &error) const
{
    d->lastError = error;
}

void Serializer::resetLastError() const
{
    setLastError(Error());
}

QVariantMap Serializer::objectToVariant(const QObject *object)
{
    QVariantMap result;
    const MetaObject metaObject = MetaObject::metaObject(object);
    int count = metaObject.propertyCount();
    for (int i = 1; i < count; ++i) {
        MetaProperty metaProperty(metaObject.property(i), metaObject);

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

void Serializer::addSerializer(Serializer *serializer)
{
    SerializerPrivate::serializers.insert(serializer->format(), serializer);
}

void Serializer::setDefaultSerializer(Serializer *serializer)
{
    SerializerPrivate::serializers.insert(QString(), serializer);
}

} // namespace QDataSuite

