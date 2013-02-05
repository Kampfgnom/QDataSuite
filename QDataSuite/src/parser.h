#ifndef QDATASUITE_PARSER_H
#define QDATASUITE_PARSER_H

#include <QtCore/QSharedDataPointer>

class QByteArray;
class QObject;
class QString;

namespace QDataSuite {

class Error;

class ParserPrivate;
class Parser
{
public:
    virtual ~Parser();

    QString contentType() const;
    QString format() const;
    Error lastError() const;

    enum Mode { Update, Create };
    virtual void parse(const QByteArray &data, QObject *object, Mode mode) const = 0;

    static Parser *forFormat(const QString &format);
    static void addParser(Parser *parser);
    static void setDefaultParser(Parser *parser);

protected:
    Parser(const QString &format, const QString &contentType);

    void setLastError(const Error &error) const;
    void resetLastError() const;

private:
    QSharedDataPointer<ParserPrivate> d;
};

} // namespace QDataSuite

#endif // QRESTSERVER_PARSER_H
