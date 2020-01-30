#include "syscall.h"

/*
Test simple du réseau

lancer: ./nachos -m 0 -x step6/simpleServeur
*/

int main() {
    socket_t serveur;

    SocketServerCreate(&serveur,0);

    socket_t client;
    int retAccept = SocketAccept(&serveur,&client);

    if(retAccept==0) {

    }
    int ret=1;
    char data[30];
    while(ret!=0) {
        PutString("Attend receive\n");
        ret = SocketReceive(&client,data,30);
        PutString(data);
    }

    SocketClose(&client);
    SocketClose(&serveur);

}