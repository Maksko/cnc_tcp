#include "tcpserver.h"
#include "hardware.h"

enum_status  eth_status = RED_DISCONNECTED;

TCPServer::TCPServer(){

}

TCPServer::~TCPServer(){

}

//-----------------------------------------------------------------------------------------------
EthernetInterface   eth;
TCPSocket           serverTCP, *clientTCP;


bool CONNECT = false;

void serverTCP_event(void){ 
    queue->call(&serverTCP_accept);        // Вызывает событие в очереди.
}
// создание tcp соединения
bool serverTCP_connect(void){
    if(!CONNECT)
        if(serverTCP.open(&eth) == NSAPI_ERROR_OK)              // инициализировать сокет на указанном NetworkInterface
            if(serverTCP.bind(PORT_SERVER) == NSAPI_ERROR_OK)   // Привязка сокет к определенному порту
                if(serverTCP.listen() == NSAPI_ERROR_OK)        // Переведите сокет в режим прослушивания
                {                                   // set_blocking(false) эквивалентно set_timeout(0) set_blocking(true) эквивалентно set_timeout(-1)
                    serverTCP.set_blocking(false);              // Установка блокирующего или неблокирующего режима сокета.
                    serverTCP.sigio(callback(&serverTCP_event)); // Регистрация обратного вызова при изменении состояния сокета.
                    // callback вызывается при изменении состояния, когда сокет может успешно принимать/отправлять/принимать и когда происходит ошибка
                    CONNECT = true;
                }
    return CONNECT;
}

void eth_event(nsapi_event_t, intptr_t param){
    switch(param)
    {
        case NSAPI_STATUS_DISCONNECTED: eth_status = RED_DISCONNECTED;  break;
        case NSAPI_STATUS_CONNECTING:if(eth_status == BLUE_CLIENT) clientTCP->close();
                                        eth_status = YELLOW_CONNECTING; break;
        case NSAPI_STATUS_GLOBAL_UP:    eth_status = GREEN_GLOBAL_UP;
                                        if(!CONNECT) serverTCP_connect();
                                        else         serverTCP_event(); break;
        default:                                                        break;
    }
}

bool eth_connect(void){
    switch(eth.get_connection_status())         // Получение статуса соединения
    {
        case NSAPI_STATUS_DISCONNECTED:         // /*!< нет подключения к сети */
            eth.set_blocking(false);
            //eth.set_dhcp(true);
            eth.set_network(SocketAddress(IP_SERVER), SocketAddress("255.255.255.0"), SocketAddress("192.168.0.1")); // if not static eth.set_dhcp(true);
            eth.attach(callback(&eth_event));
            eth.connect();                              break;
        case NSAPI_STATUS_GLOBAL_UP: return CONNECT;    break;
        default:                                        break;
    }
    return false;
}

void eth_state(void){
    switch(eth.get_connection_status())     // Получение статуса соединения
    {
        case NSAPI_STATUS_DISCONNECTED: eth_status = RED_DISCONNECTED;  break;      // нет подключения к сети
        case NSAPI_STATUS_CONNECTING:   eth_status = YELLOW_CONNECTING; break;      // подключение к сети
        case NSAPI_STATUS_GLOBAL_UP:    eth_status = GREEN_GLOBAL_UP;   break;      // установка глобального IP-адреса
        default:                                                        break;
    }
}
            // соединение с сокетом
void serverTCP_accept(void){
    if(eth_status == GREEN_GLOBAL_UP)
    {   // Тип, используемый для представления размера данных, передаваемых через сокеты
        nsapi_error_t ack = NSAPI_ERROR_WOULD_BLOCK;        // данные недоступны, но вызов является неблокирующим
        eth_status = MAGENTA_ACCEPT;
        clientTCP = serverTCP.accept(&ack);     // Принимает соединение на сокете
        switch(ack)
        {
            case NSAPI_ERROR_OK:         // отсутствие ошибок 
                clientTCP->set_timeout(TIMEOUT_SERVER);   // блокировка конфигурации клиента с таймаутом, иначе ограничение на передачу 1072 байт
                eth_status = BLUE_CLIENT;
            break;
            case NSAPI_ERROR_NO_CONNECTION:     // не подключен к сети
                eth_state();
                serverTCP_event();
            break;
            default:
                eth_state();
            break;
        }
    }
}

enum_status recv(void){
    if(eth_connect() && (eth_status == BLUE_CLIENT))
    {
        char buffer[BUFFER_SIZE] = {0};
        nsapi_error_t ack = NSAPI_ERROR_WOULD_BLOCK, size = 0;
        while((ack = clientTCP->recv(&buffer[size], BUFFER_SIZE-size)) > NSAPI_ERROR_OK) size += ack;
        if(!size && (ack == NSAPI_ERROR_OK) || (ack == NSAPI_ERROR_NO_CONNECTION))
        {
            clientTCP->close();
            eth_state();
            serverTCP_event();
        }
        if(size)
        {
            string cmd(buffer);
            for(char &c : cmd) if((c >= 'a') && (c <= 'z')) c += 'A'-'a';
            if(!cmd.empty())
            {
                ostringstream ssend;
                if(cmd.find("GET / HTTP") != string::npos)
                {
                    ssend << "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
                    ssend << "<!DOCTYPE html>\r\n<html>\r\n\t<head>\r\n\t\t<title>" << MBED_PROJECT << "</title>\r\n\t\t<meta http-equiv=refresh content=10>\r\n\t</head>";
                    ssend << "\r\n\t<body style=background-color:dimgray>\r\n\t\t<center>\r\n\t\t\t<h1>" << MBED_PROJECT << "</h1>\r\n\t\t</center>\r\n\t</body>\r\n</html>";
                }
                else ssend << "HTTP/1.1 204 No Content\r\n";
                send(ssend.str());
            }
        }
    }
    return eth_status;
}

nsapi_error_t send(const string& buff){
    nsapi_error_t ack = NSAPI_ERROR_WOULD_BLOCK;
    string ssend(buff+"\n");
    if((eth_status == BLUE_CLIENT) && !buff.empty())
        ack = clientTCP->send(ssend.c_str(), ssend.size());
    //------------------- HTTP request must close
    clientTCP->close();
    eth_state();
    serverTCP_event();
    //-------------------
    return ack;
}