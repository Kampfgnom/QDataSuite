#ifndef QDATASUITE_JSONPARSER_H
#define QDATASUITE_JSONPARSER_H

#include <QDataSuite/parser.h>

namespace QDataSuite {

class JsonParser : public Parser
{
public:
    JsonParser();

    void parse(const QByteArray &data, QObject *object, Mode mode) const;
};

} // namespace QDataSuite

#endif // QDATASUITE_JSONPARSER_H
