#ifndef COLLECTIONRESPONDER_H
#define COLLECTIONRESPONDER_H

#include <QtCore/QObject>

#include <QtCore/QSharedData>
#include <QDataSuite/error.h>

#include <qhttpresponse.h>

class QHttpRequest;

namespace QRestServer {

class Collection;
class Server;

class ResponderPrivate;
class Responder : public QObject
{
    Q_OBJECT
public:
    explicit Responder(QHttpRequest *req, QHttpResponse *resp, Server *server, Collection *collection, QObject *object = 0);
    ~Responder();

    static void serve(QHttpResponse *resp, const QByteArray &data, QHttpResponse::StatusCode statusCode);
    static void serveError(QHttpResponse *resp, const QDataSuite::Error &err, const QString &format);
    static void serveError(QHttpResponse *resp, const QByteArray &message, QHttpResponse::StatusCode statusCode, const QString &format);
private slots:
    void reply();

private:
    QSharedDataPointer<ResponderPrivate> d;
    Q_DISABLE_COPY(Responder)
};

} // namespace QRestServer

#endif // COLLECTIONRESPONDER_H
