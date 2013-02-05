#ifndef QRESTSERVER_JSONSERIALIZER_H
#define QRESTSERVER_JSONSERIALIZER_H

#include <QtCore/QObject>
#include <QDataSuite/serializer.h>

#define QRESTSERVER_JSON_SELFLINK "$self"

namespace QDataSuite {

class Server;

class JsonSerializer : public Serializer
{
public:
    JsonSerializer();

    QByteArray serialize(const QObject *resource) const;
//    QByteArray serialize(Collection *collection) const;
    QByteArray serialize(const QDataSuite::Error &error) const;


private:

    Server *m_server;
};

} // namespace QDataSuite

#endif // QRESTSERVER_JSONSERIALIZER_H
