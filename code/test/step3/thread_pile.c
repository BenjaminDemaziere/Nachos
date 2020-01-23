

/*
Partie 3
test de création de plusieurs threads et modification de la pile

*/


#include "syscall.h"

//Pas mettre trop grand car débordement de pile
#define  tailleTab 50


int t1,t2;
void calcul(void * str) {
    PutString("Début t1\n");
    int tab[tailleTab];
    int i;
    for(i=0;i<tailleTab;i++) {
        PutInt(i);
        PutChar('\n');
        tab[i]=i;
    }
    PutString("Fin t1\n");
    UserThreadExit();
}

void calcul2(void * str) {

    PutString("Début t2\n");

    int tab[tailleTab];
    int i;
    for(i=0;i<tailleTab;i++) {
        PutInt(tab[i]);
        PutChar('\n');
    }
    PutString("Fin t2\n");
    UserThreadExit();
}


int main ()
{
    PutString("Début test thread\n");
    t1=UserThreadCreate(calcul,0);
    if(t1==-1) {
        PutString("Erreur\n");
    }
    UserThreadJoin(t1);
    t2=UserThreadCreate(calcul2,0);
    if(t2==-1) {
        PutString("Erreur\n");
    }
    return 1;
}

