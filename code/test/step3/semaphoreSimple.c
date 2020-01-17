

/*
Partie 3
test de création de sémaphore
*/


#include "syscall.h"

sem_t sem;

void test() {
    SemaphoreV(&sem);
}


int main ()
{


    SemaphoreInit(&sem,0);
    PutString("Début test sem\n");

    UserThreadCreate(test,0);


    SemaphoreP(&sem);
    PutString("Fin test sem\n");

    SemaphoreFree(&sem);

    return 1;
}

