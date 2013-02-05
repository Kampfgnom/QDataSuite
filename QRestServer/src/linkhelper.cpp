#include "linkhelper.h"

#include "collection.h"
#include "global.h"
#include "server.h"

#include <QDataSuite/metaobject.h>
#include <QDataSuite/metaproperty.h>

#include <QVariant>
#include <QUrl>
#include <QStringList>

namespace QRestServer {

LinkHelper::LinkHelper(Server *server) :
    QObject(server),
    m_server(server)
{
}

QString LinkHelper::objectLink(QObject *object)
{
    QDataSuite::MetaObject metaObject = QDataSuite::MetaObject::metaObject(object);

    QVariant keyVariant = metaObject.primaryKeyProperty().read(object);
    Q_ASSERT(keyVariant.canConvert(QVariant::String));
    QString key = keyVariant.toString();

    QString collectionName = metaObject.collectionName();
    QString baseUrl = m_server->baseUrl();

    Q_ASSERT(!key.isEmpty());
    Q_ASSERT(!collectionName.isEmpty());
    Q_ASSERT(!baseUrl.isEmpty());

    return baseUrl + collectionName + '/' + key;
}

Collection *LinkHelper::resolveCollectionPath(const QString &path)
{
    QString name = collectionName(path);
    return m_server->collection(name);
}

QObject *LinkHelper::resolveObjectPath(const QString &path)
{
    Collection *collection = resolveCollectionPath(path);

    if (!collection)
        return 0;

    QString key = objectKey(path);

    return collection->object(key);
}

QString LinkHelper::collectionName(const QString &path)
{
    QStringList pathList = path.split('/');
    if (pathList.size() <= 1)
        return QString();

    return pathList.at(1);
}

QString LinkHelper::objectKey(const QString &path)
{
    QStringList pathList = path.split('/');
    if (pathList.size() <= 2)
        return QString();

    return pathList.at(2);
}

} // namespace QRestServer
