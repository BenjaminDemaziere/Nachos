

/*
Partie 3
test de création de plusieurs threads
Le thread main doit attendre la terminaison des autres threads
*/


#include "syscall.h"


void affichage(void * str) {
    PutChar('a');
    PutChar('\n');
    PutString("ABCDEF\n");
    char * s = (char *) str;
    PutString(s);
    UserThreadExit();
}


int main ()
{
    PutString("Début test thread\n");
    UserThreadCreate(affichage,"Affichage1\n");
    UserThreadCreate(affichage,"Affichage2\n");
    UserThreadCreate(affichage,"Affichage3\n");
    UserThreadCreate(affichage,"Affichage4\n");
    UserThreadCreate(affichage,"Affichage5\n");

    return 1;
}

