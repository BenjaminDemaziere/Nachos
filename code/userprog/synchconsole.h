#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "copyright.h"
#include "utility.h"
#include "console.h"

/*
Etape 2: Lecture et Ecriture dans la console
*/
class SynchConsole {
    public:
    SynchConsole (char *readFile, char *writeFile); // initialize the hardware console device
    
    ~SynchConsole(); // clean up console emulation
    
    void SynchPutChar (const char ch); // Unix putchar(3S)
    char SynchGetChar(); // Unix getchar(3S)

    void SynchPutString (const char string[]); // Unix puts(3S)
    void SynchGetString (char * string, unsigned max); // Unix fgets(3S)

    void SynchPutInt (int val);
    void SynchGetInt (int *ref);
    
    private:
    Console *console;
};
#endif // SYNCHCONSOLE_H