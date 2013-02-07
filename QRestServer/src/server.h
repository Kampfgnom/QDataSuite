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
class AbstractDataAccessObject;
}

namespace QRestServer {

class LinkHelper;

class ServerPrivate;
class Server : public QObject
{
    Q_OBJECT
public:
    Server(QObject *parent = 0);
    ~Server();

    void listen(uint port);

    void setBaseUrl(const QUrl &baseUrl);
    QUrl baseUrl() const;

    void addCollection(QDataSuite::AbstractDataAccessObject *collection);
    QList<QDataSuite::AbstractDataAccessObject *> collections() const;
    QDataSuite::AbstractDataAccessObject *collection(const QString &name);

    LinkHelper *linkHelper() const;

    static QString formatFromRequest(QHttpRequest *req);

private Q_SLOTS:
    void dispatchRequest(QHttpRequest *req, QHttpResponse *resp);

private:
    QSharedDataPointer<ServerPrivate> d;
    Q_DISABLE_COPY(Server)
};

} // namespace QRestServer

#endif // QRESTSERVER_SERVER_H
