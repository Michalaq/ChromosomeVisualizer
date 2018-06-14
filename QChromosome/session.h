#ifndef SESSION_H
#define SESSION_H


#include <QAction>

class Session
{
public:
    Session();

    QAction* getAction();

private:
    QAction *action;
};

#endif // SESSION_H
