#include "server.h"

#include "collection.h"
#include "linkhelper.h"
#include "global.h"
#include "collectionresponder.h"

#include <QDataSuite/serializer.h>

#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>

#include <QRegExp>
#include <QStringList>
#include <QDebug>

namespace QRestServer {

QString formatFromRequest(QHttpRequest *req)
{
    return QString();
}

class ServerPrivate : public QSharedData
{
public:
    ServerPrivate() :
        QSharedData(),
        port(-1),
        httpServer(0)
    {
    }

    QString baseUrl;
    quint16 port;
    QHash<QString, Collection *> collections;
    QHttpServer *httpServer;
    LinkHelper *linkHelper;

    Server *q;
};


Server::Server(QObject *parent) :
    QObject(parent),
    d(new ServerPrivate)
{
    d->q = this;
    d->httpServer = new QHttpServer(this);
    connect(d->httpServer, SIGNAL(newRequest(QHttpRequest*, QHttpResponse*)),
            this, SLOT(dispatchRequest(QHttpRequest*, QHttpResponse*)));

    d->linkHelper = new LinkHelper(this);
}

Server::~Server()
{
}

void Server::listen()
{
    Q_ASSERT(d->port > 0);

    d->httpServer->listen(QHostAddress::Any, d->port);
}

void Server::setPort(quint16 port)
{
    Q_ASSERT(port > 0);

    d->port = port;
}

quint16 Server::port() const
{
    return d->port;
}

void Server::setBaseUrl(const QString &baseUrl)
{
    Q_ASSERT(!baseUrl.isEmpty());

    d->baseUrl = baseUrl;

    if (!d->baseUrl.endsWith('/'))
        d->baseUrl.append('/');
}

QString Server::baseUrl() const
{
    return d->baseUrl;
}

void Server::addCollection(Collection *collection)
{
    Q_ASSERT(collection);
    Q_ASSERT(!collection->name().isEmpty());

    d->collections.insert(collection->name(), collection);
}

QList<Collection *> Server::collections() const
{
    return d->collections.values();
}

Collection *Server::collection(const QString &name)
{
    return d->collections.value(name);
}

LinkHelper *Server::linkHelper()
{
    return d->linkHelper;
}

void Server::dispatchRequest(QHttpRequest *req, QHttpResponse *resp)
{
    Collection *collection = d->linkHelper->resolveCollectionPath(req->path());

    if (!collection) {
        // No such collection
        Responder::serveError(resp,
                              QString("Collection not found: %1").arg(req->path()).toLatin1(),
                              QHttpResponse::STATUS_NOT_FOUND,
                              formatFromRequest(req));
        return;
    }

    QString objectKey = d->linkHelper->objectKey(req->path());

    if (objectKey.isEmpty()) {
        // Collection requested
        new Responder(req, resp, this, collection);
        return;
    }

    QObject *object = collection->object(objectKey);

    if (!object) {
        // No such object
        Responder::serveError(resp,
                              QString("Object not found: %1").arg(req->path()).toLatin1(),
                              QHttpResponse::STATUS_NOT_FOUND,
                              formatFromRequest(req));
        return;
    }

    // Object requested
    new Responder(req, resp, this, collection, object);
}

} // namespace QRestServer
