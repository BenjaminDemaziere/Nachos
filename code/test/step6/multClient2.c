#include "syscall.h"


/*
Test simple du réseau

Avant lancer multServeur
lancer: ./nachos -m 2 -x step6/multClient2
*/


int main() {
    
    socket_t sock;
    SocketCreate(&sock);

    SocketConnect(&sock,0,0);


    char str[10];
    str[0]='2';
    str[1]='a';
    str[2]='a';
    str[3]='a';
    str[4]='a';
    str[5]='\n';
    str[6]='\0';



    //Envoie au serveur
    int i;
    for(i=0;i<10;i++) {
        str[1]+=1;
        SocketSend(&sock,str,7);
    }


    //Reçoit du serveur
    int ret=1;
    char data[30];
    while(ret>0) {
        ret = SocketReceive(&sock,data,30);
        if (ret>0) { 
            PutInt(2);
            PutString(data);
        }
    }


    SocketClose(&sock);
}