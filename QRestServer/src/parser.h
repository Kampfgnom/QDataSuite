#ifndef QDATASUITE_PARSER_H
#define QDATASUITE_PARSER_H

#include <QtCore/QSharedDataPointer>

class QByteArray;
class QObject;
class QString;

namespace QDataSuite {
class Error;
}

namespace QRestServer {

class Server;

class ParserPrivate;
class Parser
{
public:
    virtual ~Parser();

    QString contentType() const;
    QString format() const;
    QDataSuite::Error lastError() const;

    enum Mode { Update, Create };
    virtual void parse(const QByteArray &data, QObject *object, Server *server, Mode mode) const = 0;

    static Parser *forFormat(const QString &format);
    static void registerParser(Parser *parser);
    static void setDefaultParser(Parser *parser);

protected:
    Parser(const QString &format, const QString &contentType);

    void setLastError(const QDataSuite::Error &error) const;
    void resetLastError() const;

private:
    QSharedDataPointer<ParserPrivate> d;
};

} // namespace QRestServer

#endif // QRESTSERVER_PARSER_H
