#include "jsonparser.h"

#include "jsonserializer.h"
#include "metaproperty.h"
#include "metaobject.h"
#include "error.h"

#include <QJsonDocument>
#include <QVariantMap>
#include <QDebug>
#include <QUrl>

namespace QDataSuite {

JsonParser::JsonParser() :
    Parser("json", "application/json")
{
}

void JsonParser::parse(const QByteArray &data, QObject *object, Mode mode) const
{
    resetLastError();

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(data, &error);
    if (!document.isObject()) {
        setLastError(Error(QString("The parsed JSON is no valid object: %1\n%2").arg(error.errorString()).arg(data.data()),
                           QDataSuite::Error::ParserError));
        return;
    }

    MetaObject metaObject = MetaObject::metaObject(object);

    QVariantMap variant = document.toVariant().toMap();
    QVariantMap::const_iterator iter;


    for (iter = variant.constBegin(); iter != variant.constEnd(); ++iter) {
        QString propertyName = iter.key();
        QVariant propertyValue = iter.value();
        QDataSuite::MetaProperty metaProperty(propertyName, metaObject);

        // When writing to newly created object instances, we ignore the read-only attribute
        // When updating an existing object, we first check
        if(mode != Create) {
            // Since all properties have to be "writable" for the meta-object-system,
            // we implement another layer of access restrictions with the help of class-info
            if(metaProperty.isReadOnly())
                continue;
        }

        if (metaProperty.isValid()) {
            // Write dynamic property
            object->setProperty(propertyName.toLatin1(), propertyValue);
        }
        else {
            // Write normal properties
            metaProperty.write(object, propertyValue);
        }
    }
}

} // namespace QDataSuite
