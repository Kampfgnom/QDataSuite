#ifndef QRESTSERVER_HALJSONSERIALIZER_H
#define QRESTSERVER_HALJSONSERIALIZER_H

#include <QRestServer/serializer.h>

#include <QSharedDataPointer>

namespace QRestServer {

class HalJsonSerializerData;

class HalJsonSerializer : public Serializer
{
public:
    HalJsonSerializer();
    ~HalJsonSerializer();

    QByteArray serialize(const QObject *object, Server *server) const;
    QByteArray serialize(const QDataSuite::Error &error) const;
    QByteArray serialize(const QDataSuite::AbstractDataAccessObject *collection,
                         Server *server) const;
    
private:
    QSharedDataPointer<HalJsonSerializerData> data;

    Q_DISABLE_COPY(HalJsonSerializer)
};

} // namespace QRestServer

#endif // QRESTSERVER_HALJSONSERIALIZER_H
