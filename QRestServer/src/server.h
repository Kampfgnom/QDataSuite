#ifndef QRESTSERVER_SERVER_H
#define QRESTSERVER_SERVER_H

#include <QtCore/QObject>

#include <QtCore/QSharedDataPointer>

#define HttpHeaderContentLength "Content-Length"
#define HttpStatusCode "httpStatusCode"

class QHttpRequest;
class QHttpResponse;

namespace QDataSuite {
class Error;
}

namespace QRestServer {

QString formatFromRequest(QHttpRequest *req);

class Collection;
class LinkHelper;

class ServerPrivate;
class Server : public QObject
{
    Q_OBJECT
public:
    Server(QObject *parent = 0);
    ~Server();

    void listen();

    void setPort(quint16 port);
    quint16 port() const;

    void setBaseUrl(const QString &baseUrl);
    QString baseUrl() const;

    void addCollection(Collection *collection);
    QList<Collection *> collections() const;
    Collection *collection(const QString &name);

    LinkHelper *linkHelper();

private slots:
    void dispatchRequest(QHttpRequest *req, QHttpResponse *resp);

private:
    QSharedDataPointer<ServerPrivate> d;
    Q_DISABLE_COPY(Server)
};

} // namespace QRestServer

#endif // QRESTSERVER_SERVER_H
