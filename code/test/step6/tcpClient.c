#include "syscall.h"

/*
tcp client

Avant lancer tcpServeur ./nachos -m 0 -x step6/tcpServeur
lancer: ./nachos -m 1 -x step6/tcpClient
*/

int main() {


    socket_t sock;
    SocketCreate(&sock);
    SocketConnect(&sock,0,0);



    

    SocketSend(&sock,"Bonjour\n",9);


    SocketClose(&sock);
}