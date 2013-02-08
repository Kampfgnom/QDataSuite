#include "linkhelper.h"

#include "server.h"

#include <QDataSuite/metaobject.h>
#include <QDataSuite/metaproperty.h>
#include <QDataSuite/abstractdataaccessobject.h>

#include <QVariant>
#include <QUrl>
#include <QStringList>

namespace QRestServer {

LinkHelper::LinkHelper(Server *server) :
    QObject(server),
    m_server(server)
{
}

QUrl LinkHelper::objectLink(const QObject *object)
{
    Q_ASSERT(object);

    QDataSuite::MetaObject metaObject = QDataSuite::MetaObject::metaObject(object);

    QVariant keyVariant = metaObject.primaryKeyProperty().read(object);
    Q_ASSERT(keyVariant.canConvert(QVariant::String));

    QString key = keyVariant.toString();
    QString collectionName = metaObject.collectionName();

    Q_ASSERT(!key.isEmpty());
    Q_ASSERT(!collectionName.isEmpty());

    QUrl url = m_server->baseUrl();
    QString path = url.path();
    path.append(collectionName).append('/').append(key);
    url.setPath(path);
    return url;
}

QUrl LinkHelper::collectionLink(const QDataSuite::AbstractDataAccessObject *collection)
{
    Q_ASSERT(collection);

    QDataSuite::MetaObject metaObject = collection->dataSuiteMetaObject();
    QString collectionName = metaObject.collectionName();

    Q_ASSERT(!collectionName.isEmpty());

    QUrl url = m_server->baseUrl();
    QString path = url.path();
    path.append(collectionName);
    url.setPath(path);
    return url;
}

QDataSuite::AbstractDataAccessObject *LinkHelper::resolveCollectionPath(const QString &path)
{
    QString name = collectionName(path);
    return m_server->collection(name);
}

QObject *LinkHelper::resolveObjectPath(const QString &path)
{
    QDataSuite::AbstractDataAccessObject *collection = resolveCollectionPath(path);

    if (!collection)
        return 0;

    QVariant key = objectKey(path);
    return collection->readObject(key);
}

QObject *LinkHelper::resolveObjectLink(const QUrl &link)
{
    return resolveObjectPath(link.path());
}

QString LinkHelper::collectionName(const QString &path)
{
    QStringList pathList = path.split('/');
    if (pathList.size() <= 1)
        return QString();

    return pathList.at(1);
}

QVariant LinkHelper::objectKey(const QString &path)
{
    QStringList pathList = path.split('/');
    if (pathList.size() <= 2)
        return QVariant();

    return QVariant(pathList.at(2));
}

} // namespace QRestServer
