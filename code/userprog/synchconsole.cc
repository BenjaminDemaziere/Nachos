#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"


// Sémaphores utilisés pour l'intéraction avec la console
static Semaphore *readAvail;
static Semaphore *writeDone;

// sémaphores utilisés pour garantir le bon fonctionnement des fonctions de lecture
static Semaphore *getCharAvail;
static Semaphore *getStringAvail;
static Semaphore *getIntAvail;

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

    getCharAvail = new Semaphore ("getchar available", 1);
    getStringAvail = new Semaphore ("getstring available", 1);
    getIntAvail = new Semaphore ("getint available", 1);

    console = new Console (readFile, writeFile, ReadAvail, WriteDone, 0);
}

SynchConsole::~SynchConsole()
{
    delete writeDone;
    delete readAvail;

    delete getCharAvail;
    delete getStringAvail;
    delete getIntAvail;

    delete console;
}

void SynchConsole::SynchPutChar (const char ch)
{
	console->PutChar (ch);
	writeDone->P ();
}
char SynchConsole::SynchGetChar()
{
	getCharAvail->P();

    readAvail->P ();
    
    char ch = console->GetChar();

    getCharAvail->V();

	return ch;
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

    DEBUG('a', "Entrée dans SynchGetString, string : %s et max : %u\n", string, max);
    
    getStringAvail->P();
    
    unsigned index = 0;
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

void SynchConsole::SynchPutInt (const int val)
{
    char * buffer = new char[MAX_STRING_SIZE];

    snprintf (buffer, MAX_STRING_SIZE, "%d", val);
    SynchPutString (buffer);

    delete [] buffer;
}

void SynchConsole::SynchGetInt (int * ref)
{
    ASSERT(ref != NULL);

    getIntAvail->P();

    char * buffer = new char[MAX_STRING_SIZE];

    SynchGetString (buffer, MAX_STRING_SIZE);
    sscanf (buffer, "%d", ref);

    getIntAvail->V();
    
    delete [] buffer;
}