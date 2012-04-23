#ifndef TONESEXCEPTION_H
#define TONESEXCEPTION_H

#include <QString>
#include <exception>

class TonesException : public std::exception
{
public:
    TonesException(const QString &msg):
        std::exception(),
        _msg(msg)
    {
    }

    ~TonesException() throw()
    {

    }

    const char *what() const throw()
    {
        return _msg.toUtf8().data();
    }

private:
    QString _msg;
};

#endif // TONESEXCEPTION_H
