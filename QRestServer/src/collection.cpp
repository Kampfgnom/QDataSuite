#include "collection.h"

namespace QRestServer {

QDataSuite::Error Collection::lastError() const
{
    return m_lastError;
}

void Collection::setLastError(const QDataSuite::Error &error) const
{
    m_lastError = error;
}

void Collection::resetLastError() const
{
    setLastError(QDataSuite::Error());
}

} // namespace QRestServer
