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
void SynchConsole::SynchPutString (const char string[])
{
	ASSERT(string != NULL);
    
    for (unsigned index = 0 ; string[index] != '\0' ; index++)
    {
        SynchPutChar(string[index]);
    }
}
void SynchConsole::SynchGetString (char * string, int max)
{
    ASSERT(string != NULL && max > 0 && max <= MAX_STRING_SIZE);
    
    int index = 0;

    for (char ch = SynchGetChar() ; index < max - 1 && ch != EOF && ch != '\0' ; index++)
    {
        string[index] = ch;
        ch = SynchGetChar();
    }

    if (string[index] != '\0')
		string[index] = '\0';
}

void SynchConsole::SynchPutInt (int val)
{
    //
}

void SynchConsole::SynchGetInt (int *ref)
{
    ASSERT(ref != NULL);
}