

/*
Partie 3
test d'un thread qui créer un thread
*/


#include "syscall.h"
int t1;
int t2;
void f_t2(void * str) {
    PutString("Je suis le thread t2\n");
    UserThreadExit();
}


void f_t1(void * str) {
    PutString("Je suis le thread t1 qui crée t2\n");
    t2 = UserThreadCreate(f_t2,0);

    UserThreadExit();
}


int main ()
{
    PutString("Début test thread\n");
    t1 = UserThreadCreate(f_t1,0);

    PutString("Main attend t2\n");
    UserThreadJoin(t1); //attend que t1 crée t2
    UserThreadJoin(t2);
    PutString("Main fin attente t2\n");


    return 1;
}

