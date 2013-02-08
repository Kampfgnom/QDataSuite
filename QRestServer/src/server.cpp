#include "server.h"

#include "linkhelper.h"
#include "responder.h"
#include "serializer.h"
#include "haljsonserializer.h"
#include "haljsonparser.h"

#include <QDataSuite/abstractdataaccessobject.h>
#include <QDataSuite/metaobject.h>

#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>

#include <QRegExp>
#include <QStringList>
#include <QDebug>

namespace QRestServer {

class ServerPrivate : public QSharedData
{
public:
    ServerPrivate() :
        QSharedData(),
        httpServer(0)
    {
    }

    QUrl baseUrl;
    QHash<QString, QDataSuite::AbstractDataAccessObject *> collections;
    QHttpServer *httpServer;
    LinkHelper *linkHelper;
    HalJsonSerializer *serializer;
    HalJsonParser *parser;

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

    d->serializer = new HalJsonSerializer;
    Serializer::registerSerializer(d->serializer);

    d->parser = new HalJsonParser;
    Parser::registerParser(d->parser);

    d->linkHelper = new LinkHelper(this);
}

Server::~Server()
{
}

void Server::listen(uint port)
{
    Q_ASSERT(port > 0);
    d->baseUrl.setPort(port);
    d->httpServer->listen(QHostAddress::Any, port);
}

void Server::setBaseUrl(const QUrl &baseUrl)
{
    Q_ASSERT(!baseUrl.isEmpty());

    d->baseUrl = baseUrl;

    QString path = d->baseUrl.path();
    if(!path.endsWith('/')) {
        path.append('/');
        d->baseUrl.setPath(path);
    }
}

QUrl Server::baseUrl() const
{
    return d->baseUrl;
}

void Server::addCollection(QDataSuite::AbstractDataAccessObject *collection)
{
    Q_ASSERT(collection);

    d->collections.insert(collection->dataSuiteMetaObject().collectionName(), collection);
}

QList<QDataSuite::AbstractDataAccessObject *> Server::collections() const
{
    return d->collections.values();
}

QDataSuite::AbstractDataAccessObject *Server::collection(const QString &name)
{
    return d->collections.value(name);
}

LinkHelper *Server::linkHelper() const
{
    return d->linkHelper;
}

void Server::dispatchRequest(QHttpRequest *req, QHttpResponse *resp)
{
    QDataSuite::AbstractDataAccessObject *collection = d->linkHelper->resolveCollectionPath(req->path());

    if (!collection) {
        // No such collection
        Responder::serveError(resp,
                              QString("Collection not found: %1").arg(req->path()).toLatin1(),
                              QHttpResponse::STATUS_NOT_FOUND,
                              formatFromRequest(req));
        return;
    }

    QVariant objectKey = d->linkHelper->objectKey(req->path());

    if (objectKey.isNull()) {
        // Collection requested
        new Responder(req, resp, this, collection);
        return;
    }

    QObject *object = collection->readObject(objectKey);

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

QString Server::formatFromRequest(QHttpRequest *req)
{
    return QString();
}

} // namespace QRestServer
