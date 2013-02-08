#ifndef QRESTSERVER_HALJSONPARSER_H
#define QRESTSERVER_HALJSONPARSER_H

#include <QRestServer/parser.h>

#include <QSharedDataPointer>

namespace QRestServer {

class HalJsonParserData;

class HalJsonParser : public Parser
{
public:
    HalJsonParser();
    ~HalJsonParser();

    void parse(const QByteArray &data, QObject *object, Server *server, Mode mode) const;
    
private:
    QSharedDataPointer<HalJsonParserData> data;

    Q_DISABLE_COPY(HalJsonParser)
};

} // namespace QRestServer

#endif // QRESTSERVER_HALJSONPARSER_H
