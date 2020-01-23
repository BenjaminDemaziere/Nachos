

/*
Partie 3
test du thread principal qui atteint 2 threads
*/


#include "syscall.h"

void thread_1() {
    int i=0;
    while(i<100000) {
        i++;
    }
    PutString("Fin thread\n");
    UserThreadExit();
}

void thread_2() {
    int i=0;
    while(i<400000) {
        i++;
    }
    PutString("Fin thread2\n");
    UserThreadExit();
}

int main ()
{
    int t1, t2;
    t1 = UserThreadCreate(thread_1,0);
    if(t1<0) {
        PutString("Erreur\n");
    }

    t2 = UserThreadCreate(thread_2,0);

    PutString("Main attend t1\n");
    UserThreadJoin(t1);
    PutString("Main a fini d'attendre t1\n");
    PutString("Main attend t2\n");
    UserThreadJoin(t2);
    PutString("Main a fini d'attendre t2\n");

    return 0;
}

