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
	int type = machine->ReadRegister (2);

	if (which == SyscallException)
	{
		switch (type)
		{
			case SC_Exit :
			{
				if (machine->ReadRegister (4) != 0)
				{
					printf("Retour différent de 0, erreur dans le programme. Terminaison...\n");
					interrupt->Halt();
				}
				else
				{
					printf("Fin du programme utilisateur avec code retour 0\n");
				}
				
				break;
			}
			
			
			case SC_Halt :
			{
				DEBUG('a', "Shutdown, initiated by user program.\n");
				interrupt->Halt();
				break;
			}
			case SC_PutChar :
			{
				synchconsole->SynchPutChar ((char) machine->ReadRegister (4));
				break;
			}
			case SC_GetChar :
			{
				machine->WriteRegister (2, (int) synchconsole->SynchGetChar());
				break;
			}
			case SC_PutString :
			{
				char * buffer = new char[MAX_STRING_SIZE];

				copyStringFromMachine (machine->ReadRegister (4), buffer, (unsigned) MAX_STRING_SIZE);
				synchconsole->SynchPutString (buffer);

				delete [] buffer;

				break;
			}
			case SC_GetString :
			{
				char * buffer = new char[MAX_STRING_SIZE];
				unsigned size = (unsigned) machine->ReadRegister (5);
				
				synchconsole->SynchGetString (buffer, size);
				DEBUG('a', "buffer = %s\n", buffer);

				copyStringToMachine (machine->ReadRegister (4), buffer, size);

				delete [] buffer;

				break;
			}
			case SC_PutInt :
			{
				synchconsole->SynchPutInt (machine->ReadRegister (4));
				break;
			}
			case SC_GetInt :
			{
				int phyAddr;

				DEBUG('a', "Entrée dans SC_GetInt, arg = %d\n", machine->ReadRegister (4));

				machine->Translate(machine->ReadRegister (4), &phyAddr, (int) sizeof (int), TRUE);
				synchconsole->SynchGetInt ((int *) phyAddr);

				break;
			}
			default :
			{
				printf("Unexpected user mode exception %d %d\n", which, type);
				ASSERT(FALSE);
				break;
			}
		}

		UpdatePC();
	}
}

void copyStringFromMachine (int from, char * to, unsigned size)
{
	ASSERT(to != NULL && size > 0 && size <= MAX_STRING_SIZE);
	
	char ch;
	unsigned index = 0;
	unsigned ch_size = sizeof (char);
	
	for (machine->ReadMem (from, ch_size, (int *) &ch) ; index < size && ch != EOF && ch != '\0' ; index += ch_size)
	{
		DEBUG('a', "ch : %c ; index : %u ; to : %s ; ch_size : %u\n", ch, index, to, ch_size);
		to[index] = ch;
		machine->ReadMem (from + index + ch_size, ch_size, (int *) &ch);
		int count = 0; // instruction obligatoire pour contourner un souci sur l'instruction ReadMem
		count = count; // instruction obligatoire pour contourner un souci sur l'instruction ReadMem
	}

	if (to[index] != '\0')
		to[index] = '\0';
}

void copyStringToMachine (int to, char * from, unsigned size)
{
	ASSERT(to >= 0 && from != NULL && size > 0 && size <= MAX_STRING_SIZE);

	for (unsigned index = 0 ; from[index] != '\0' ; index++)	
	{
		machine->WriteMem (to + index, sizeof (char), (int) from[index]);
	}
}