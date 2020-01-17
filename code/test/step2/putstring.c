/*
    Test de putstring fait en partie 2
*/

#include "syscall.h"


int main() {
    //Test chaine de base
    PutString("abcd\n");


    //test chaine plus grande que la taille max de chaine acceptée (256 caractères)
    //Affiche 200 a puis 55 b. (le 256ème caractère est '\0') 
    char tabC[1000];
    for (int i=0;i<200;i++) {
        tabC[i]='a';
    }
    for (int i=200;i<1000;i++) {
        tabC[i]='b';
    }
    PutString(tabC);
    PutChar('\n');


    //test '\0' au milieu d'une chaine
    //Doit afficher "cccccccccc"
    for (int i=0;i<10;i++) {
        tabC[i]='c';
    }
    tabC[10]='\0';
    for (int i=11;i<1000;i++) {
        tabC[i]='d';
    }
    PutString(tabC);
    PutChar('\n');






    Halt();
}
