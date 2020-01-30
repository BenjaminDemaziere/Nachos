#include "syscall.h"

/*
Test de plusieurs connexion vers le serveur

Lancer: aller dans test/step6
./step6/mult.sh
=> lance le serveur et les clients
*/

sem_t sem;

void handler(void * arg) {
    socket_t *client = (socket_t*)arg;

    int ret=1;
    char data[30];
    for(int i=0;i<10;i++) {
        ret = SocketReceive(client,data,30);
        if (ret!=0) PutString(data);
    }

    SemaphoreP(&sem);

    data[0]=data[1]=data[2];
    data[3]='\n';
    data[4]='\0';
    for(int i=0;i<5;i++) {
        data[1]+=1;
        SocketSend(client,data,5);
    }
    SemaphoreV(&sem);

    SocketClose(client);
}

int main() {
    SemaphoreInit(&sem,1);
    socket_t serveur;

    SocketServerCreate(&serveur,0);


    socket_t tab[3];
    int tabT[3];


    for(int j=0;j<3;j++) {
        PutString("Attend connection\n");
        SocketAccept(&serveur,&(tab[j]));
        tabT[j]=UserThreadCreate(handler,(void *)(&(tab[j])));
    }
    for(int j=0;j<3;j++) {
        UserThreadJoin(tabT[j]);
    }


    SocketClose(&serveur);

}