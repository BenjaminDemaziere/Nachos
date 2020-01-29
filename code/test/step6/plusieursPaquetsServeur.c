#include "syscall.h"

/*
Test simple du réseau

lancer: ./nachos -m 0 -x step6/plusieursPaquetsServeur
*/

int main() {
    socket_t serveur;

    SocketServerCreate(&serveur,0);

    socket_t client;
    SocketAccept(&serveur,&client);


    int ret=1;
    char data[30];
    for(int i=0;i<10;i++) {
        ret = SocketReceive(&client,data,30);
        if (ret!=0) PutString(data);
    }

    data[0]=data[1]=data[2];
    data[3]='\n';
    data[4]='\0';
    for(int i=0;i<5;i++) {
        data[1]+=1;
        SocketSend(&client,data,5);
    }

    SocketClose(&client);
    SocketClose(&serveur);

}