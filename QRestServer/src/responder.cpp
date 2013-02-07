#include "responder.h"

#include "server.h"
#include "serializer.h"
#include "parser.h"

#include <QDataSuite/metaobject.h>
#include <QDataSuite/metaproperty.h>
#include <QDataSuite/abstractdataaccessobject.h>

#include <qhttprequest.h>
#include <qhttpresponse.h>

Q_DECLARE_METATYPE(QHttpResponse::StatusCode)

namespace QRestServer {

class ResponderPrivate : public QSharedData
{
public:
    ResponderPrivate() :
        QSharedData()
    {}

    QHttpRequest *req; // We need to delete the request
    QHttpResponse *resp;
    QDataSuite::AbstractDataAccessObject *collection;
    QObject *object;
    Server *server;

    Responder *q;

    void serveResponse(const QByteArray &data, QHttpResponse::StatusCode statusCode = QHttpResponse::STATUS_OK);
    void serveError(const QDataSuite::Error &error);
    void serveError(const QByteArray &message, QHttpResponse::StatusCode statusCode);

    void replyCollection();
    void createObject();
    void serveCollection();

    void replyObject();
    void serveObject();
    void serveObject(QObject *obj);
    void deleteObject();
    void updateObject();
};

void ResponderPrivate::serveError(const QByteArray &message, QHttpResponse::StatusCode statusCode)
{
    QDataSuite::Error error(message, QDataSuite::Error::ServerError);
    error.addAdditionalInformation(HttpStatusCode, statusCode);
    serveError(error);
}

void ResponderPrivate::serveResponse(const QByteArray &data, QHttpResponse::StatusCode statusCode)
{
    Responder::serve(resp, data, statusCode);
}

void ResponderPrivate::serveError(const QDataSuite::Error &err)
{
    Serializer *serializer = Serializer::forFormat(Server::formatFromRequest(req));
    serveResponse(serializer->serialize(err),
                  err.additionalInformation().value(HttpStatusCode).value<QHttpResponse::StatusCode>());
}

void ResponderPrivate::replyCollection()
{
    switch (req->method())
    {
    case QHttpRequest::HTTP_GET:
        serveCollection();
        break;
    case QHttpRequest::HTTP_POST:
        createObject();
        break;
    default:
        serveError(QByteArray("Method not allowed!"), QHttpResponse::STATUS_METHOD_NOT_ALLOWED);
        break;
    }
}

void ResponderPrivate::replyObject()
{
    switch (req->method())
    {
    case QHttpRequest::HTTP_GET:
        serveObject();
        break;
    case QHttpRequest::HTTP_DELETE:
        deleteObject();
        break;
    case QHttpRequest::HTTP_PUT:
        updateObject();
        break;
    default:
        serveError(QByteArray("Method not allowed!"), QHttpResponse::STATUS_METHOD_NOT_ALLOWED);
        break;
    }
}

void ResponderPrivate::serveCollection()
{
    Serializer *serializer = Serializer::forFormat(Server::formatFromRequest(req));
    QByteArray data = serializer->serialize(collection, server);

    if (serializer->lastError().isValid()) {
        serveError(serializer->lastError());
        return;
    }

    serveResponse(data);
}

void ResponderPrivate::createObject()
{
    QObject *newObject = collection->createObject();

    if (collection->lastError().isValid()) {
        serveError(collection->lastError());
        delete newObject;
        return;
    }

    Parser *parser = Parser::forFormat(Server::formatFromRequest(req));
    parser->parse(req->body(), newObject, server, Parser::Create);

    if (parser->lastError().isValid()) {
        serveError(parser->lastError());
        delete newObject;
        return;
    }

    QDataSuite::MetaObject metaObject = QDataSuite::MetaObject::metaObject(object);
    QDataSuite::MetaProperty keyMetaProperty = metaObject.primaryKeyProperty();

    if (!keyMetaProperty.isAutoIncremented()) {
        QString key = keyMetaProperty.read(object).toString();
        if (key.isEmpty()) {
            serveError(QByteArray("The collection has no auto-generated key. You have to specify a key to create a new object."),
                       QHttpResponse::STATUS_INTERNAL_SERVER_ERROR);
            delete newObject;
            return;
        }
    }

    collection->insertObject(newObject);

    if (collection->lastError().isValid()) {
        serveError(collection->lastError());
        return;
    }

    serveObject(newObject);
}

void ResponderPrivate::serveObject()
{
    serveObject(object);
}

void ResponderPrivate::serveObject(QObject *obj)
{
    Serializer *serializer = Serializer::forFormat(Server::formatFromRequest(req));
    QByteArray data = serializer->serialize(obj, server);

    if (serializer->lastError().isValid()) {
        serveError(serializer->lastError());
        return;
    }

    serveResponse(data);
}

void ResponderPrivate::deleteObject()
{
    collection->removeObject(object);

    if (collection->lastError().isValid()) {
        serveError(collection->lastError());
        return;
    }

    serveResponse(QByteArray("Delete OK!"));
}

void ResponderPrivate::updateObject()
{
    Parser *parser = Parser::forFormat(Server::formatFromRequest(req));
    parser->parse(req->body(), object, server, Parser::Update);

    if (parser->lastError().isValid()) {
        serveError(parser->lastError());
        return;
    }

    collection->updateObject(object);

    if (collection->lastError().isValid()) {
        serveError(collection->lastError());
        return;
    }

    serveObject();
}

Responder::Responder(QHttpRequest *req,
                     QHttpResponse *resp,
                     Server *server,
                     QDataSuite::AbstractDataAccessObject *collection,
                     QObject *object) :
    QObject(server),
    d(new ResponderPrivate)
{
    d->q = this;
    d->req = req;
    d->resp = resp;
    d->collection = collection;
    d->object = object;
    d->server = server;
    req->storeBody();

    connect(req, SIGNAL(end()), this, SLOT(reply()));
    connect(resp, SIGNAL(done()), this, SLOT(deleteLater()));
}

Responder::~Responder()
{
    delete d->req;
    d->req = 0;
}

void Responder::reply()
{
    if (d->collection) {
        if (d->object) {
            d->replyObject();
        }
        else {
            d->replyCollection();
        }
    }

    d->serveError(QByteArray("Internal server error!"), QHttpResponse::STATUS_INTERNAL_SERVER_ERROR);
}

void Responder::serve(QHttpResponse *resp, const QByteArray &data, QHttpResponse::StatusCode statusCode)
{
    resp->setHeader(HttpHeaderContentLength, QString::number(data.length()));
    resp->writeHead(statusCode);
    resp->write(data);
    resp->end();
}

void Responder::serveError(QHttpResponse *resp,
                           const QByteArray &message,
                           QHttpResponse::StatusCode statusCode,
                           const QString &format)
{
    QDataSuite::Error error(message, QDataSuite::Error::ServerError);
    error.addAdditionalInformation(HttpStatusCode, statusCode);
    serveError(resp, error, format);
}

void Responder::serveError(QHttpResponse *resp,
                           const QDataSuite::Error &err,
                           const QString &format)
{
    Serializer *serializer = Serializer::forFormat(format);
    serve(resp,
          serializer->serialize(err),
          err.additionalInformation().value(HttpStatusCode).value<QHttpResponse::StatusCode>());
}

} // namespace QRestServer
