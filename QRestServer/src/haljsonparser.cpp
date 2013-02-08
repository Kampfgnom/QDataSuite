#include "haljsonparser.h"
#include <QSharedData>

#include "server.h"
#include "linkhelper.h"

#include <QDataSuite/metaobject.h>
#include <QDataSuite/metaproperty.h>
#include <QDataSuite/error.h>

#include <qhalresource.h>
#include <qhallink.h>

#include <QJsonDocument>
#include <QDebug>
#include <QUrl>

namespace QRestServer {

class HalJsonParserData : public QSharedData {
public:

};

HalJsonParser::HalJsonParser() :
    Parser("json", "application/hal+json"),
    data(new HalJsonParserData)
{
}

HalJsonParser::~HalJsonParser()
{
}

void HalJsonParser::parse(const QByteArray &data, QObject *object, Server *server, Parser::Mode mode) const
{
    resetLastError();

    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data, &error);

    if(error.error != QJsonParseError::NoError) {
        QString dataString(data);
        QString errorMessage = QString("Parser error: %1 at offset %2")
                .arg(error.errorString())
                .arg(error.offset);
        errorMessage.append(dataString.mid(error.offset - 30, 30));
        errorMessage.append(QString(">> %1").arg(dataString.mid(error.offset,1)));
        errorMessage.append(dataString.mid(error.offset+1,30));
        setLastError(QDataSuite::Error(errorMessage, QDataSuite::Error::ParserError));
        return;
    }

    QHalResource resource = QHalResource::fromVariant(jsonDocument.toVariant());

    QDataSuite::MetaObject metaObject = QDataSuite::MetaObject::metaObject(object);

    foreach(QDataSuite::MetaProperty property, metaObject.simpleProperties()) {
        if(property.isPrimaryKey()
                && mode != Create)
            continue;

        property.write(object, resource.properties().value(property.name()));
    }

    foreach(QHalLink link, resource.links()) {
        if(!metaObject.hasMetaProperty(link.rel()))
            continue;

        QDataSuite::MetaProperty property = metaObject.metaProperty(link.rel());

        if(!property.isValid())
            continue;

        if(link.isList()) {
            if(!property.isToManyRelationProperty())
                continue;

            QList<QObject *> relatedObjects;
            for(int i = 0; i < link.count(); ++i) {
                QObject *relatedObject = server->linkHelper()->resolveObjectLink(link[i].href());
                if(relatedObject)
                    relatedObjects.append(relatedObject);
            }
            QVariant value = QDataSuite::MetaObject::variantListCast(relatedObjects, property.reverseClassName());
            property.write(object, value);
        }
        else {
            if(!property.isToOneRelationProperty())
                continue;

            QObject *relatedObject = server->linkHelper()->resolveObjectLink(link.href());
            QVariant value = QDataSuite::MetaObject::variantCast(relatedObject, property.reverseClassName());
            property.write(object, value);
        }
    }
}

} // namespace QRestServer
