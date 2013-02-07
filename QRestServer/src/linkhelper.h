#ifndef QRESTSERVER_LINKHELPER_H
#define QRESTSERVER_LINKHELPER_H

#include <QObject>

namespace QDataSuite {
class AbstractDataAccessObject;
}

namespace QRestServer {

class Server;
class Collection;

class LinkHelper : public QObject
{
    Q_OBJECT
public:
    explicit LinkHelper(Server *server);

    QUrl objectLink(const QObject *object);
    QUrl collectionLink(const QDataSuite::AbstractDataAccessObject *collection);

    QDataSuite::AbstractDataAccessObject *resolveCollectionPath(const QString &path);
    QObject *resolveObjectPath(const QString &path);

    QString collectionName(const QString &path);
    QVariant objectKey(const QString &path);

private:
    Server *m_server;
};

} // namespace QRestServer

#endif // QRESTSERVER_LINKHELPER_H
