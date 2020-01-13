// exception.cc 
//      Entry point into the Nachos kernel from user programs.
//      There are two kinds of things that can cause control to
//      transfer back to here from user code:
//
//      syscall -- The user code explicitly requests to call a procedure
//      in the Nachos kernel.  Right now, the only function we support is
//      "Halt".
//
//      exceptions -- The user code does something that the CPU can't handle.
//      For instance, accessing memory that doesn't exist, arithmetic errors,
//      etc.  
//
//      Interrupts (which can also cause control to transfer from user
//      code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "userthread.h"


//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------
static void
UpdatePC ()
{
    int pc = machine->ReadRegister (PCReg);
    machine->WriteRegister (PrevPCReg, pc);
    pc = machine->ReadRegister (NextPCReg);
    machine->WriteRegister (PCReg, pc);
    pc += 4;
    machine->WriteRegister (NextPCReg, pc);
}


//----------------------------------------------------------------------
// ExceptionHandler
//      Entry point into the Nachos kernel.  Called when a user program
//      is executing, and either does a syscall, or generates an addressing
//      or arithmetic exception.
//
//      For system calls, the following is the calling convention:
//
//      system call code -- r2
//              arg1 -- r4
//              arg2 -- r5
//              arg3 -- r6
//              arg4 -- r7
//
//      The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//      "which" is the kind of exception.  The list of possible exceptions 
//      are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler (ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if (which == SyscallException) {
        switch (type) {
            case SC_Exit: //Exit correspond à un halt
            case SC_Halt: {
                DEBUG('a', "Shutdown, initiated by user program.\n");
                interrupt->Halt();
                break;
            }
            case SC_PutChar: {
                DEBUG('a', "PutChar, initiated by user program.\n");
                char c = (char)machine->ReadRegister(4); //On récupère la valeur passée en argument
                synchconsole->SynchPutChar(c);
                break;
            }
            case SC_PutString: {
                DEBUG('a', "PutString, initiated by user program.\n");
                int adr = machine->ReadRegister(4); //On récupère l'adresse de la chaine

                char * buf = new char[MAX_STRING_SIZE];

                machine->copyStringFromMachine(adr,buf,MAX_STRING_SIZE); //Récupère la chaine dans buf

                synchconsole->SynchPutString(buf);

                delete buf; //On supprime le buffer
                break;
            }

            case SC_GetChar: {
                DEBUG('a', "GetChar, initiated by user program.\n");
                char c = synchconsole->SynchGetChar(); //Récupère le caractère
                DEBUG('a', "GetChar, returns value %d\n",c);
                machine->WriteRegister(2, (int)c); //Ecrit la valeur de retour dans le registre 2
                break;
            }

            case SC_GetString: {
                DEBUG('a', "GetString, initiated by user program.\n");
                int adr = machine->ReadRegister(4); //Pointeur vers la chaine
                int size = machine->ReadRegister(5); //taille max de la chaine à récupérer

                char * buf = new char[size];
                synchconsole->SynchGetString(buf,size); //Récupère la chaine
                DEBUG('a', "GetString, returns value %s\n",buf);

                machine->copyStringToMachine(buf,adr,size); //Ecrit la chaine dans la mémoire (vers le programme utilisateur)

                delete buf;
                break;
            }

            case SC_PutInt: {
                DEBUG('a', "PutInt, initiated by user program.\n");
                int n = machine->ReadRegister(4); //On récupère la valeur passée en argument
                synchconsole->SynchPutInt(n);
                break;
            }

            case SC_GetInt: {
                DEBUG('a', "GetInt, initiated by user program.\n");
                int n = synchconsole->SynchGetInt(); //Récupère un entier
                machine->WriteRegister(2, n); //Ecrit la valeur de retour dans le registre 2
                break;
            }


            case SC_UserThreadCreate: {
                DEBUG('a', "UserThreadCreate, initiated by user program.\n");
                int f = machine->ReadRegister(4); //Récupère le pointeur vers la fonction et les arguments
                int args = machine->ReadRegister(5);

                int th = do_UserThreadCreate(f,args);

                machine->WriteRegister(2, th); //Ecrit la valeur de retour dans le registre 2

                break;
            }

            case SC_UserThreadExit: {
                DEBUG('a', "UserThreadExit, initiated by user program.\n");
                do_UserThreadExit();
                break;
            }

            default: {
                printf("Unexpected user mode exception %d %d\n", which, type);
                ASSERT(FALSE);
            }
        }
        UpdatePC();
    }
}