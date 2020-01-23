

/*
Partie 3
test de deux thread qui attendent un même thread 

Le thread main et t2 attendent le thread t1
*/

#include "syscall.h"

int t1, t2;


void thread_1() {
    int i=0;
    while(i<400000) {
        i++;
    }
    PutString("Fin thread1\n");
    UserThreadExit();
}

void thread_2() {
    int i=0;
    while(i<100000) {
        i++;
    }
    PutString("thread2 fin boucle\n");
    UserThreadJoin(t1);
    PutString("Fin thread2\n");
    UserThreadExit();
}

int main ()
{
    t1 = UserThreadCreate(thread_1,0);
    if(t1<0) {
        PutString("Erreur\n");
    }
    t2 = UserThreadCreate(thread_2,0);
    if(t2<0) {
        PutString("Erreur\n");
    }


    PutString("Main attend t1\n");
    UserThreadJoin(t1);
    PutString("Main fin attente t1\n");

    return 0;
}

