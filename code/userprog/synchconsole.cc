#include "copyright.h"
#include "system.h"
#include "synchconsole.h"


    
static Semaphore *readAvail;
static Semaphore *writeDone;

//Handler utilisé quand un caractère peut être lu
static void ReadAvail(int arg) { 
    readAvail->V();
}

//Handler utilisé quand un caractère a été écrit dans la console
static void WriteDone(int arg) {
    writeDone->V();
}


SynchConsole::SynchConsole(char *readFile, char *writeFile) {
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    console = new Console (readFile, writeFile, ReadAvail, WriteDone, 0);
    semaphorePut = new Semaphore("Synch console put",1);
    semaphoreGet = new Semaphore("Synch console get",1);
}


SynchConsole::~SynchConsole() {
    delete console;
    delete writeDone;
    delete readAvail;
}

void SynchConsole::SynchPutChar(const char ch) {
    semaphorePut->P();
    console->PutChar(ch);
    writeDone->P(); //Attend que l'écriture soit terminé
    semaphorePut->V();
}

char SynchConsole::SynchGetChar() {
    semaphoreGet->P();
    readAvail->P(); //Attend qu'il y ait un caractère à lire
    char c = console->GetChar();
    semaphoreGet->V();
    return c;
}

void SynchConsole::SynchPutString(const char s[]) {
    semaphorePut->P();
    for(int i=0;i<MAX_STRING_SIZE && s[i]!='\0';i++) {//Affichage caractère par caractère de la chaine
        // SynchPutChar(s[i]);
        console->PutChar(s[i]);
        writeDone->P(); //Attend que l'écriture soit terminé
    }
    semaphorePut->V();
}

void SynchConsole::SynchGetString(char *s, int n) {
    semaphoreGet->P();
    char c; int i = 0;
    do {
        readAvail->P(); //Attend qu'il y ait un caractère à lire
        c = console->GetChar();
        s[i]=c;
        i++;
    } while(i<n-1 && c!=EOF && c!='\n'); //Lit des caractères tant qu'on a pas atteint fin de ligne ou EOF
    s[i]='\0'; //On n'oublie pas le caractère de fin de chaine
    semaphoreGet->V();
}

//Un entier int 32 bits ne fait pas plus de 9 charactères

void  SynchConsole::SynchPutInt(int n) {
    semaphorePut->P();
    char *nbr = new char[11];
    sprintf(nbr,"%d",n); //Convertit l'entier en chaine de caractère
    // SynchPutString(nbr); //Affiche le nombre
    for(int i=0;nbr[i]!='\0';i++) {//Affichage caractère par caractère de la chaine
        console->PutChar(nbr[i]);
        writeDone->P(); //Attend que l'écriture soit terminé
    }
    delete nbr;
    semaphorePut->V();
}

int  SynchConsole::SynchGetInt() {
    semaphoreGet->P();
    char *nbr = new char[11]; int n;
    // SynchGetString(nbr,11);
    char c; int i = 0;
    do {
        readAvail->P(); //Attend qu'il y ait un caractère à lire
        c = console->GetChar();
        nbr[i]=c;
        i++;
    } while(i<11 && c!=EOF && c!='\n'); //Lit des caractères tant qu'on a pas atteint fin de ligne ou EOF
    nbr[i]='\0'; //On n'oublie pas le caractère de fin de chaine
    sscanf(nbr,"%d",&n);
    delete nbr;
    semaphoreGet->V();
    return n;
}