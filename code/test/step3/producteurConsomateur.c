

/*
Partie 3
test de création de sémaphore
*/


#include "syscall.h"

sem_t sem;
sem_t vide;
sem_t plein;


#define NB 10
int objet[NB];
int nbElement=0;

void produire() {
    SemaphoreP(&plein); //Si plein attend
    SemaphoreP(&sem);
    
    nbElement++;
    PutString("Je produis\n");

    SemaphoreV(&sem);
    SemaphoreV(&vide);

}





void consommer() {
    SemaphoreP(&vide); //Si c'est vide on attend
    SemaphoreV(&sem);

    nbElement--;
    PutString("Je consomme\n");


    SemaphoreV(&sem);
    SemaphoreV(&plein); //Enlève à plein

}




void consommateur() {
    int i=0;
    while(i<10) {
        consommer();
        i++;
    }
    PutString("Fin cons\n");
}

void producteur() {
    int i=0;
    while(i<10) {
        produire();
        i++;
    }
    PutString("Fin prod\n");
}

int main ()
{
    int t1,t2,t3,t4;
    SemaphoreInit(&sem,1);
    SemaphoreInit(&vide,0);
    SemaphoreInit(&plein,NB);



    PutString("Début test sem\n");

    t1=UserThreadCreate(consommateur,0);
    t2=UserThreadCreate(consommateur,0);
    t3=UserThreadCreate(producteur,0);
    t4=UserThreadCreate(producteur,0);



    UserThreadJoin(t1);
    UserThreadJoin(t2);    
    UserThreadJoin(t3);    
    UserThreadJoin(t4);    

    SemaphoreFree(&sem);
    SemaphoreFree(&vide);
    SemaphoreFree(&plein);

    PutString("Fin test sem\n");

    return 1;
}

