#ifndef QRESTSERVER_SERIALIZER_H
#define QRESTSERVER_SERIALIZER_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QVariantMap>

class QByteArray;
class QObject;
class QString;

namespace QDataSuite {
class Error;
class AbstractDataAccessObject;
}

namespace QRestServer {

class Server;

class SerializerPrivate;
class Serializer
{
public:
    virtual ~Serializer();

    virtual QByteArray serialize(const QObject *object, Server *server) const = 0;
    virtual QByteArray serialize(const QDataSuite::AbstractDataAccessObject *collection,
                                 Server *server) const = 0;
    virtual QByteArray serialize(const QDataSuite::Error &error) const = 0;

    QString contentType() const;
    QString format() const;
    QDataSuite::Error lastError() const;

    static QVariantMap objectToVariant(const QObject *object);

    static Serializer *forFormat(const QString &format);
    static void registerSerializer(Serializer *serializer);
    static void setDefaultSerializer(Serializer *serializer);

protected:
    Serializer(const QString &format, const QString &contentType);

    void setLastError(const QDataSuite::Error &error) const;
    void resetLastError() const;

private:
    QSharedDataPointer<SerializerPrivate> d;
};

} // namespace QRestServer

#endif // QRESTSERVER_SERIALIZER_H
