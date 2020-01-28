#include "syscall.h"

/*
tcp Serveur

./nachos -m 0 -x step6/tcpServeur
*/



int main() {
    socket_t serveur;

    SocketServerCreate(&serveur,0);

    socket_t client;
    SocketAccept(&serveur,&client);


    int ret=1;
    char data[30];
    while(ret!=0) {
        PutString("Attend receive\n");
        ret = SocketReceive(&client,data,30);
        PutString(data);
    }



}