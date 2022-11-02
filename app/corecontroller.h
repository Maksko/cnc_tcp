#ifndef CORECONTROLLER_H
#define CORECONTROLLER_H

#include "tcpserver.h"
#include "globstr.h"

class CoreController{

public:
    CoreController();
    ~CoreController();

protected:

private:
    TCPServer server;
};

#endif