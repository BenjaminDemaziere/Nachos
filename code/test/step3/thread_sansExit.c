

/*
Partie 3
test de thread sans UserThreadExit

Le thread main et t2 attendent le thread t1
*/

#include "syscall.h"

int t1;


void thread_1() {
    int i=0;
    while(i<4000) {
        i++;
    }
    PutString("Fin thread1\n");
    // UserThreadExit();
}

int main ()
{
    t1 = UserThreadCreate(thread_1,0);
    if(t1<0) {
        PutString("Erreur\n");
    }

    PutString("Main attend t1\n");
    UserThreadJoin(t1);
    PutString("Main fin attente t1\n");

}

