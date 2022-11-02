#include "main.h"


EventQueue          *queue = NULL;  // очередь для планирования событий
BusOut              Led(LED1, LED2, LED3);  // BusOut для объединения нескольких выводов DigitalOut для одновременной записи на них

int main()
{ 
    Led = eth_status;   //  состояние соединения
    queue = mbed_event_queue();     // Запросить общую очередь
    while(1) if(eth_connect()) Led = recv();
}
