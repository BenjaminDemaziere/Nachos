#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "copyright.h"
#include "utility.h"
#include "console.h"
#include "synch.h"


/*
Etape 2:
Lecture et écriture dans la console
*/
class SynchConsole {
    public:
    SynchConsole(char *readFile, char *writeFile); // initialize the hardware console device
    
    ~SynchConsole(); // clean up console emulation
    
    void SynchPutChar(const char ch); // Unix putchar(3S)
    char SynchGetChar(); // Unix getchar(3S)
    

    void SynchPutString(const char *s); // Unix puts(3S)
    void SynchGetString(char *s, int n); // Unix fgets(3S)

    void SynchPutInt(int n);
    int SynchGetInt();
    
    private:
    Console *console;

    //Protège les appels à la console pour qu'un thread à la fois y est accés
    Semaphore * semaphoreGet;
    Semaphore * semaphorePut;
};
#endif // SYNCHCONSOLE_H