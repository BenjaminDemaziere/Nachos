#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "copyright.h"
#include "utility.h"
#include "console.h"

/*
Etape 2:
Lecture et Ecriture dans la console
*/
class SynchConsole {
    public:
    SynchConsole(char *readFile, char *writeFile); // initialize the hardware console device
    
    ~SynchConsole(); // clean up console emulation
    
    void SynchPutChar(const char ch); // Unix putchar(3S)
    char SynchGetChar(); // Unix getchar(3S)
    
    void copyStringFromMachine(int from, char *to, unsigned size); // copy a string from MIPS-based system to Linux system

    void SynchPutString(const char *s); // Unix puts(3S)
    void SynchGetString(char *s, int n); // Unix fgets(3S)
    
    private:
    Console *console;
};
#endif // SYNCHCONSOLE_H