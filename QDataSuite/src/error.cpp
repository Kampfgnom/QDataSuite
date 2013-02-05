#include "error.h"

#include <QDebug>

namespace QDataSuite {

class ErrorPrivate : public QSharedData
{
    public:
        ErrorPrivate() :
            QSharedData(),
            text(QString()),
            isValid(false),
            type(Error::NoError)
        {}

        QString text;
        bool isValid;
        QVariantMap additionalInformation;
        Error::ErrorType type;
};

Error::Error(const QString &text,
             ErrorType type,
             QVariantMap additionalInformation) :
    d(new ErrorPrivate)
{
    d->text = text;
    d->type = type;
    d->additionalInformation = additionalInformation;
    d->isValid = (type != NoError && ! text.isEmpty());
}

Error::~Error()
{
}

Error::Error(const Error &other)
{
    d = other.d;
}

Error &Error::operator =(const Error &other)
{
    if (&other != this)
        d = other.d;

    return *this;
}

bool Error::isValid() const
{
    return d->isValid;
}

QString Error::text() const
{
    return d->text;
}

Error::ErrorType Error::type() const
{
    return d->type;
}

QVariantMap Error::additionalInformation() const
{
    return d->additionalInformation;
}

void Error::addAdditionalInformation(const QString &key, const QVariant &value)
{
    d->additionalInformation.insert(key, value);
}

} // namespace QDataSuite

QDebug operator<<(QDebug dbg, const QDataSuite::Error &error)
{
    dbg.nospace() << "(" << error.type() << ", " << error.text() << ")";
    return dbg.space();
}
