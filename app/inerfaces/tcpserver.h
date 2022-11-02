#ifndef TCPSERVER_H
#define TCPSERVER_H

#define MBED_PROJECT    "TCP-sample"
#define IP_SERVER       "192.168.0.123"
#define PORT_SERVER     80
#define TIMEOUT_SERVER  100
#define BUFFER_SIZE     1072

#include "mbed.h"
#include "EthernetInterface.h"
#include <sstream>

class TCPServer{

public:
    TCPServer();
    ~TCPServer();
    //void sendData(const void *data, nsapi_size_t size); //отправить данные в сеть
    //void bind(uint16_t port);
    //void clean(); //удалить сокет и все данные

protected:

private:
    //TCPSocket serverTCP, *clientTCP;
};

enum enum_status { WHITE, CYAN, MAGENTA_ACCEPT, BLUE_CLIENT, YELLOW_CONNECTING, GREEN_GLOBAL_UP, RED_DISCONNECTED, BLACK_INITIALIZE };

void            serverTCP_event(void);
bool            serverTCP_connect(void);
void            eth_event(nsapi_event_t, intptr_t);
bool            eth_connect(void);
void            eth_state(void);
void            serverTCP_accept(void);

enum_status     recv(void);
nsapi_error_t   send(const string& buff);

#endif