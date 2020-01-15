#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
Console *console;

static void ReadAvail (int arg)
{ 
    readAvail->V();
}
static void WriteDone (int arg)
{
    writeDone->V();
}

SynchConsole::SynchConsole (char *readFile, char *writeFile)
{
    readAvail = new Semaphore ("read avail", 0);
    writeDone = new Semaphore ("write done", 0);
    console = new Console (readFile, writeFile, ReadAvail, WriteDone, 0);
}

SynchConsole::~SynchConsole()
{
    delete console;
    delete writeDone;
    delete readAvail;
}

void copyStringFromMachine(int from, char *to, unsigned size)
{
    // char buffer[MAX_STRING_SIZE];
    // char * origin = (char *) from;

    strlcpy (to, (char *) from, (size_t) size);
}

void SynchConsole::SynchPutChar (const char ch)
{
	console->PutChar (ch);
	writeDone->P ();	// wait for write to finish
}
char SynchConsole::SynchGetChar()
{
	readAvail->P ();	// wait for character to arrive
	return console->GetChar ();
}
void SynchConsole::SynchPutString (const char s[])
{
	for (int i = 0 ; s[i] != '\0' ; i++)
    {
        SynchPutChar(s[i]);
    }
}
void SynchConsole::SynchGetString (char *s, int n)
{
    for (int i = 0 ; i <= n - 2 ; i++)
    {
        s[i] = SynchGetChar();
    }
    s[n - 1] = '\0';
}