#include "syscall.h"

/*
Test simple du réseau

Avant lancer simpleServeur
lancer: ./nachos -m 1 -x step6/simpleClient
*/


int main() {
    
    socket_t sock;
    SocketCreate(&sock);

    int ret = SocketConnect(&sock,0,0);

    if(ret==1) {
        SocketSend(&sock,"Bonjour\n",9);


        SocketClose(&sock);
    }
    else {
        PutString("Erreur\n");
    }

}