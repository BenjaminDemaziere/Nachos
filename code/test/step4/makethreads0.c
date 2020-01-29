/*
Partie 4
programme à faire run par un processus
*/


#include "syscall.h"

int stop = 0;

void affichage(void * str) {

    PutString("Affichage du premier processus !\n");
    stop = 1;
    UserThreadExit();
}

int main ()
{
    // PutString("Debut test thread\n");
    UserThreadCreate(affichage,"Affichage\n");
    // int g = UserThreadCreate(affichage,"Affichage\n");

    while(stop==0);

    return 0;
}
