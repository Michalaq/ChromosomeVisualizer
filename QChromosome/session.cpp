#include "session.h"

Session::Session() :
    action(new QAction)
{

}

QAction* Session::getAction()
{
    return action;
}
