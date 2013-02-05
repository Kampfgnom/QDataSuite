#include "abstractdataaccessobject.h"

#include "error.h"

namespace QDataSuite {

class AbstractDataAccessObjectPrivate : public QSharedData
{
public:
    AbstractDataAccessObjectPrivate() :
        QSharedData()
    {}

    mutable Error lastError;
};

AbstractDataAccessObject::AbstractDataAccessObject(QObject *parent) :
    QObject(parent),
    d(new AbstractDataAccessObjectPrivate)
{
}

AbstractDataAccessObject::~AbstractDataAccessObject()
{
}

Error AbstractDataAccessObject::lastError() const
{
    return d->lastError;
}

void AbstractDataAccessObject::setLastError(const Error &error) const
{
    d->lastError = error;
}

} // namespace QDataSuite

uint qHash( const QVariant & var )
{
    if ( !var.isValid() || var.isNull() )
        return -1;

    switch ( var.type() )
    {
    case QVariant::Int:
        return qHash( var.toInt() );
    case QVariant::UInt:
        return qHash( var.toUInt() );
    case QVariant::Bool:
        return qHash( var.toUInt() );
    case QVariant::Double:
        return qHash( var.toUInt() );
    case QVariant::LongLong:
        return qHash( var.toLongLong() );
    case QVariant::ULongLong:
        return qHash( var.toULongLong() );
    case QVariant::String:
        return qHash( var.toString() );
    case QVariant::Char:
        return qHash( var.toChar() );
    case QVariant::StringList:
        return qHash( var.toString() );
    case QVariant::ByteArray:
        return qHash( var.toByteArray() );
    case QVariant::Date:
    case QVariant::Time:
    case QVariant::DateTime:
    case QVariant::Url:
    case QVariant::Locale:
    case QVariant::RegExp:
        return qHash( var.toString() );
    case QVariant::Map:
    case QVariant::List:
    case QVariant::BitArray:
    case QVariant::Size:
    case QVariant::SizeF:
    case QVariant::Rect:
    case QVariant::LineF:
    case QVariant::Line:
    case QVariant::RectF:
    case QVariant::Point:
    case QVariant::UserType:
    case QVariant::Invalid:
    default:
        Q_ASSERT(false);
    }

    // could not generate a hash for the given variant
    Q_ASSERT(false);
}
