#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"


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
}


SynchConsole::~SynchConsole() {
    delete console;
    delete writeDone;
    delete readAvail;
}

void SynchConsole::SynchPutChar(const char ch) {
    console->PutChar(ch);
    writeDone->P(); //Attend que l'écriture soit terminé
}

char SynchConsole::SynchGetChar() {
    readAvail->P(); //Attend qu'il y ait un caractère à lire
    return console->GetChar();
}

void SynchConsole::SynchPutString(const char s[]) {
    for(int i=0;i<MAX_STRING_SIZE && s[i]!='\0';i++) {//Affichage caractère par caractère de la chaine
        SynchPutChar(s[i]);
    }
}

void SynchConsole::SynchGetString(char *s, int n) {
    char c; int i = 0;
    do {
        c = SynchGetChar();
        s[i]=c;
        i++;
    } while(i<n-1 && c!=EOF && c!='\n'); //Lit des caractères tant qu'on a pas atteint fin de ligne ou EOF
    s[i]='\0'; //On n'oublie pas le caractère de fin de chaine
}

void  SynchConsole::SynchPutInt(int n) {
    char *nbr = new char[11];
    sprintf(nbr,"%d",n); //Convertit l'entier en chaine de caractère
    SynchPutString(nbr); //Affiche le nombre
    delete nbr;
}

int  SynchConsole::SynchGetInt() {
    char *nbr = new char[11]; int n;
    SynchGetString(nbr,11);
    sscanf(nbr,"%d",&n);

    delete nbr;
    return n;
}