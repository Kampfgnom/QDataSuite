#include "jsonserializer.h"

#include "error.h"

#include <QJsonDocument>
#include <QVariantMap>
#include <QMetaProperty>
#include <QStringList>
#include <QDebug>
#include <QRegularExpression>

namespace QDataSuite {

JsonSerializer::JsonSerializer() :
    Serializer("json", "application/json")
{
}

QByteArray JsonSerializer::serialize(const QObject *object) const
{
    return QJsonDocument::fromVariant(Serializer::objectToVariant(object)).toJson();
}

//QByteArray JsonSerializer::serialize(Collection *collection) const
//{
//    QVariantList result;

//    QStringList keys = collection->keys();
//    foreach (const QString key, keys) {
//        result.append(objectToVariant(collection->object(key)));
//    }

//    return QJsonDocument::fromVariant(result).toJson();
//}

QByteArray JsonSerializer::serialize(const QDataSuite::Error &error) const
{
    QVariantMap result;
    result["text"] = error.text();
    result["type"] = error.type();

    QMapIterator<QString, QVariant> it(error.additionalInformation());
    while(it.hasNext()) {
        it.next();
        result[it.key()] = it.value();
    }

    return QJsonDocument::fromVariant(result).toJson();
}

} // namespace QDataSuite
