#ifndef QDATASUITE_ERROR_H
#define QDATASUITE_ERROR_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QString>
#include <QtCore/QVariantMap>

class QSqlQuery;

namespace QDataSuite {

class ErrorPrivate;
class Error
{
public:
    enum ErrorType {
        NoError = 0,
        SqlError,
        ParserError,
        SerializerError,
        ServerError,
        UserError = 1024
    };

    Error(const QString &text = QString(),
          ErrorType type = NoError,
          QVariantMap additionalInformation = QVariantMap());
    ~Error();
    Error(const Error &other);
    Error &operator = (const Error &other);

    bool isValid() const;
    QString text() const;
    ErrorType type() const;

    QVariantMap additionalInformation() const;
    void addAdditionalInformation(const QString &key, const QVariant &value);

private:
    QSharedDataPointer<ErrorPrivate> d;
};

} // namespace QDataSuite

QDebug operator<<(QDebug dbg, const QDataSuite::Error &error);

#endif // QDATASUITE_ERROR_H
