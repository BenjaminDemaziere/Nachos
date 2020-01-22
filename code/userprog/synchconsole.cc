#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
static Semaphore *getStringAvail;
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
    getStringAvail = new Semaphore ("getstring available", 1);
    console = new Console (readFile, writeFile, ReadAvail, WriteDone, 0);
}

SynchConsole::~SynchConsole()
{
    delete console;
    delete writeDone;
    delete readAvail;
    delete getStringAvail;
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
        SynchPutChar (string[index]);
    }
}
void SynchConsole::SynchGetString (char * string, unsigned max)
{
    ASSERT(string != NULL && max > 0 && max <= MAX_STRING_SIZE);

    DEBUG('a', "Entrée dans SynchGetString, string = %s et max = %u\n", string, max);
    
    unsigned index = 0;

    getStringAvail->P();

    char ch = SynchGetChar();

    if (ch == EOF)
        string = NULL;
    else
    {
        bool endRead = FALSE;
        while (index < max - 1 && !endRead)
        {
            if (ch == EOF)
                endRead = TRUE;
            else
            {
                if (ch == '\n')
                    endRead = TRUE;

                string[index] = ch;
                index++;
            }
            if (!endRead)
                ch = SynchGetChar();
        }
        if (string[index] != '\0')
            string[index] = '\0';
    }

    getStringAvail->V();
}

void SynchConsole::SynchPutInt (int val)
{
    char * buffer = new char[MAX_STRING_SIZE];

    snprintf (buffer, MAX_STRING_SIZE, "%d", val);
    SynchPutString (buffer);

    delete [] buffer;
}

void SynchConsole::SynchGetInt (int *ref)
{
    ASSERT(ref != NULL);

    DEBUG('a', "Entrée dans SynchGetInt, ref = %p\n", (void *) ref);

    char * buffer = new char[MAX_STRING_SIZE];

    SynchGetString (buffer, MAX_STRING_SIZE);

    DEBUG('a', "Retour dans SynchGetInt, ref = %p\n", (void *) ref);
    
    sscanf (buffer, "%d", ref);
    
    delete [] buffer;
}