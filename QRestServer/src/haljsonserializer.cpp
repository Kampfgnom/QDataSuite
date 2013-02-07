#include "haljsonserializer.h"
#include <QSharedData>

#include "server.h"
#include "linkhelper.h"

#include <QDataSuite/metaobject.h>
#include <QDataSuite/metaproperty.h>
#include <QDataSuite/error.h>
#include <QDataSuite/abstractdataaccessobject.h>

#include <qhalresource.h>
#include <qhallink.h>

#include <QJsonDocument>
#include <QUrl>

namespace QRestServer {

class HalJsonSerializerData : public QSharedData {
public:
    QHalResource objectToResource(const QObject *object, Server *server) const;
};

QHalResource HalJsonSerializerData::objectToResource(const QObject *object, Server *server) const
{
    QUrl objectUrl = server->linkHelper()->objectLink(object);
    QDataSuite::MetaObject metaObject = QDataSuite::MetaObject::metaObject(object);

    QHalResource itemResource;
    itemResource.setHref(objectUrl);

    // Properties
    foreach(QDataSuite::MetaProperty property, metaObject.simpleProperties()) {
        itemResource.setProperty(property.columnName(), property.read(object));
    }

    // Links
    foreach(QDataSuite::MetaProperty property, metaObject.relationProperties()) {
        QDataSuite::MetaProperty::Cardinality cardinality = property.cardinality();

        if(cardinality == QDataSuite::MetaProperty::ToOneCardinality
                || cardinality == QDataSuite::MetaProperty::ManyToOneCardinality) {
            QObject *relatedObject = QDataSuite::MetaObject::objectCast(property.read(object));

            QUrl relatedUrl;
            if(relatedObject)
                relatedUrl = server->linkHelper()->objectLink(relatedObject);

            itemResource.addLink(QHalLink(property.name(), relatedUrl));
        }
        else if(cardinality == QDataSuite::MetaProperty::ToManyCardinality
                || cardinality == QDataSuite::MetaProperty::OneToManyCardinality) {
            QList<QObject *> relatedObjects = QDataSuite::MetaObject::objectListCast(property.read(object));

            QHalLink link(property.name(), QList<QHalLink>());
            foreach(QObject *relatedObject, relatedObjects) {
                QUrl relatedUrl;
                if(relatedObject)
                    relatedUrl = server->linkHelper()->objectLink(relatedObject);

                link.append(QHalLink("", relatedUrl));
            }
            itemResource.addLink(link);
        }
        else if(cardinality == QDataSuite::MetaProperty::ManyToManyCardinality) {
            Q_ASSERT_X(false, Q_FUNC_INFO, "ManyToManyCardinality relations are not supported yet.");
        }
        else if(cardinality == QDataSuite::MetaProperty::OneToOneCardinality) {
            Q_ASSERT_X(false, Q_FUNC_INFO, "OneToOneCardinality relations are not supported yet.");
        }

    }

    return itemResource;
}

HalJsonSerializer::HalJsonSerializer() :
    Serializer("json", "application/hal+json"),
    data(new HalJsonSerializerData)
{
}

HalJsonSerializer::~HalJsonSerializer()
{
}

QByteArray HalJsonSerializer::serialize(const QObject *object, Server *server) const
{
    QHalResource objectResource = data->objectToResource(object, server);

    QDataSuite::MetaObject metaObject = QDataSuite::MetaObject::metaObject(object);
    QDataSuite::AbstractDataAccessObject *collection = server->collection(metaObject.collectionName());
    QUrl collectionUrl = server->linkHelper()->collectionLink(collection);

    objectResource.addLink(QHalLink("collection", collectionUrl));

    QVariant documentVariant = objectResource.toVariant();
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(documentVariant);
    return jsonDoc.toJson();
}

QByteArray HalJsonSerializer::serialize(const QDataSuite::Error &error) const
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

QByteArray HalJsonSerializer::serialize(const QDataSuite::AbstractDataAccessObject *collection, Server *server) const
{
    QUrl collectionUrl = server->linkHelper()->collectionLink(collection);
    QDataSuite::MetaObject metaObject = collection->dataSuiteMetaObject();

    QHalResource collectionResource;
    foreach(QObject *object, collection->readAllObjects()) {
        QHalResource objectResource = data->objectToResource(object, server);
        collectionResource.append(objectResource);
    }

    QHalResource resource;
    resource.setHref(collectionUrl);
    resource.embed(metaObject.collectionName(), collectionResource);
    resource.setProperty("count", QVariant(collectionResource.count()));

    QVariant documentVariant = resource.toVariant();
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(documentVariant);
    return jsonDoc.toJson();
}


} // namespace QRestServer
