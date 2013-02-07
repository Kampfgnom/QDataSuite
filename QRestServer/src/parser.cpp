#include "parser.h"

#include <QDataSuite/error.h>

#include <QHash>

namespace QRestServer {

class ParserPrivate : public QSharedData
{
public:
    ParserPrivate() :
        QSharedData()
    {}

    mutable QDataSuite::Error lastError;
    QString format;
    QString contentType;

    static QHash<QString, Parser *> parsers;
};

QHash<QString, Parser *> ParserPrivate::parsers;

Parser::Parser(const QString &format, const QString &contentType) :
    d(new ParserPrivate)
{
    d->format = format;
    d->contentType = contentType;
}

Parser::~Parser()
{
}

QString Parser::contentType() const
{
    return d->contentType;
}

QString Parser::format() const
{
    return d->format;
}

QDataSuite::Error Parser::lastError() const
{
    return d->lastError;
}

void Parser::setLastError(const QDataSuite::Error &error) const
{
    d->lastError = error;
}

void Parser::resetLastError() const
{
    setLastError(QDataSuite::Error());
}

Parser *Parser::forFormat(const QString &format)
{
    return ParserPrivate::parsers.value(format);
}

void Parser::registerParser(Parser *parser)
{
    if(!ParserPrivate::parsers.contains(QString()))
        setDefaultParser(parser);

    ParserPrivate::parsers.insert(parser->format(), parser);
}

void Parser::setDefaultParser(Parser *parser)
{
    ParserPrivate::parsers.insert(QString(), parser);
}

} // namespace QRestServer

