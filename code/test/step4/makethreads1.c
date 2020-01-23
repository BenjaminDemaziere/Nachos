/*
Partie 3
test des threads
*/


#include "syscall.h"

int stop = 0;

void affichage(void * str) {
    PutChar('a');
    PutChar('\n');


    PutString("AAAAAA\n");

    char * s = (char *) str;
    PutString(s);
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
