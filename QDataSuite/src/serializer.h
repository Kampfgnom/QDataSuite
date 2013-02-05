#ifndef QDATASUITE_SERIALIZER_H
#define QDATASUITE_SERIALIZER_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QVariantMap>

class QByteArray;
class QObject;
class QString;

namespace QDataSuite {

class Error;

class SerializerPrivate;
class Serializer
{
public:
    virtual ~Serializer();

    virtual QByteArray serialize(const QObject *object) const = 0;
//    virtual QByteArray serialize(const Collection *collection) const = 0;
    virtual QByteArray serialize(const Error &error) const = 0;

    QString contentType() const;
    QString format() const;
    Error lastError() const;

    static QVariantMap objectToVariant(const QObject *object);

    static Serializer *forFormat(const QString &format);
    static void addSerializer(Serializer *serializer);
    static void setDefaultSerializer(Serializer *serializer);

protected:
    Serializer(const QString &format, const QString &contentType);

    void setLastError(const Error &error) const;
    void resetLastError() const;

private:
    QSharedDataPointer<SerializerPrivate> d;
};

} // namespace QDataSuite

#endif // QRESTSERVER_SERIALIZER_H
