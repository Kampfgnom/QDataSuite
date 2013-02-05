#include "parser.h"

#include "error.h"

#include <QHash>

namespace QDataSuite {

class ParserPrivate : public QSharedData
{
public:
    ParserPrivate() :
        QSharedData()
    {}

    mutable Error lastError;
    QString format;
    QString contentType;

    static QHash<QString, Parser *> parsers;

    class Guard {
    public:
        ~Guard()
        {
            QHashIterator<QString, Parser *> it(parsers);
            while(it.hasNext()) {
                it.next();
                delete it.value();
            }
        }
    };
    static Guard guard; // deletes the parsers upon destruction
};

QHash<QString, Parser *> ParserPrivate::parsers;
ParserPrivate::Guard ParserPrivate::guard;

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

Error Parser::lastError() const
{
    return d->lastError;
}

void Parser::setLastError(const Error &error) const
{
    d->lastError = error;
}

void Parser::resetLastError() const
{
    setLastError(Error());
}

Parser *Parser::forFormat(const QString &format)
{
    return ParserPrivate::parsers.value(format);
}

void Parser::addParser(Parser *parser)
{
    ParserPrivate::parsers.insert(parser->format(), parser);
}

void Parser::setDefaultParser(Parser *parser)
{
    ParserPrivate::parsers.insert(QString(), parser);
}

} // namespace QDataSuite

