#ifndef QRESTSERVER_LINKHELPER_H
#define QRESTSERVER_LINKHELPER_H

#include <QObject>

namespace QRestServer {

class Server;
class Collection;

class LinkHelper : public QObject
{
    Q_OBJECT
public:
    explicit LinkHelper(Server *server);

    QString objectLink(QObject *object);

    Collection *resolveCollectionPath(const QString &path);
    QObject *resolveObjectPath(const QString &path);

    QString collectionName(const QString &path);
    QString objectKey(const QString &path);

private:
    Server *m_server;
};

} // namespace QRestServer

#endif // QRESTSERVER_LINKHELPER_H
