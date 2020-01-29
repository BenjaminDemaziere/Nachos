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

#include <exception>

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "userthread.h"
#include "utility.h"
#include "synch.h"
#include "../network/socketTCP.h"



extern int nbprocess;
extern void StartProcess (char *file);
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


Semaphore *mutex= new Semaphore("mutex",1);
Semaphore *attente= new Semaphore("attente",0);
int nbAtt=0;

void
ExceptionHandler (ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if (which == SyscallException) {
        switch (type) {
            case SC_Exit: {
                DEBUG('a', "Exit, initiated by user program.\n");

                //Thread main attend les autres threads du processus
                if(currentThread->idT==0) {//thread Main
                    while(currentThread->space->nbThreads>1) {
                        currentThread->space->semaphoreEnd->P(); //Attend que tout les threads se terminent
                    }
                }

            
              mutex->P();
              nbAtt++;
              if(nbAtt == nbprocess){
                for (int i =0; i<nbprocess - 1;i++){
                  attente->V();
                }
                nbAtt=0;
                mutex->V();
              }else{ //Désalloue le processus
                mutex->V();
                attente->P();
                delete currentThread->space;
                currentThread->Finish();
              }
                //Dernier processus termine nachos
                interrupt->Halt();

                break;
            }
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
                int adrExit = machine->ReadRegister(6); //Adresse de UserThreadExit

                int th = do_UserThreadCreate(f,args,adrExit);

                machine->WriteRegister(2, th); //Ecrit la valeur de retour dans le registre 2

                break;
            }

            case SC_UserThreadExit: {
                DEBUG('a', "UserThreadExit, initiated by user program.\n");
                do_UserThreadExit();
                break;
            }

            case SC_UserThreadJoin: {
                DEBUG('a', "UserThreadJoin, initiated by user program.\n");
                int idT = machine->ReadRegister(4);
                do_UserThreadJoin(idT);
                break;
            }

            case SC_SemaphoreInit: {
                DEBUG('a', "SempahoreInit, initiated by user program.\n");
                int sem = machine->ReadRegister(4); //Le sem utilisateur
                int nbValue = machine->ReadRegister(5); //Le nombre pour initialiser le sémaphore

                Semaphore * semSystem = new Semaphore("Sem utilisateur",nbValue); //Création du sémaphore système

                machine->WriteMem(sem,4,(int)semSystem); //Ecrit dans le coté utilisateur l'adresse du sémahpore système

                break;
            }

            case SC_SemaphoreP: {
                DEBUG('a', "SempahoreP, initiated by user program.\n");
                int userSem = machine->ReadRegister(4); //Le sémaphore utilisateur

                int adrSem;
                machine->ReadMem(userSem,4,&adrSem); //Lit l'adresse du sémaphore système
                Semaphore * sem = (Semaphore * ) adrSem;

                sem->P();
                break;
            }

            case SC_SemaphoreV: {
                DEBUG('a', "SempahoreV, initiated by user program.\n");
                int userSem = machine->ReadRegister(4); //Adresse utilisateur

                int adrSem;
                machine->ReadMem(userSem,4,&adrSem); //Lit l'adresse du sémaphore système
                Semaphore * sem = (Semaphore * ) adrSem;

                sem->V();
                break;
            }

            case SC_SemaphoreFree: {
                DEBUG('a', "SempahoreV, initiated by user program.\n");
                int userSem = machine->ReadRegister(4); //Adresse utilisateur

                int adrSem;
                machine->ReadMem(userSem,4,&adrSem); //Lit l'adresse du sémaphore système
                Semaphore * sem = (Semaphore * ) adrSem;

                delete sem; //On supprime le sémaphore
                break;
            }

            case SC_ForkExec: {

              int s =machine->ReadRegister(4);
              char * buf= new char[MAX_STRING_SIZE];
              machine->copyStringFromMachine(s,buf,MAX_STRING_SIZE);//Récupération de la string
              Thread *newThread = new Thread("Thread système");//Création de thread système
              newThread->Fork((VoidFunctionPtr)StartProcess,(int)buf);//Lancement du nouveau programme dans ce thread
              machine->WriteRegister(2, 0); // On écrit la valeur de retour
              currentThread->Yield();
              break;
            }


            case SC_SocketCreate: {
                DEBUG('a', "SocketCreate, initiated by user program.\n");
                int userSock = machine->ReadRegister(4); //Adresse utilisateur

                SocketClientTCP * sock = new SocketClientTCP(); //Création de la socket système

                machine->WriteMem(userSock,4,(int)sock); //Ecrit dans le coté utilisateur l'adresse de la socket système
                machine->WriteMem(userSock+sizeof(int),1,1); //Type de la socket

                break;
            }


            case SC_SocketServerCreate: {
                DEBUG('a', "SocketServerCreate, initiated by user program.\n");
                int userSock = machine->ReadRegister(4); //Adresse utilisateur
                int port = machine->ReadRegister(5); //Port de la connexion

                SocketServerTCP * sock = NULL;
                try{
                    sock = new SocketServerTCP(port);

                }
                catch(const std::exception& e)
                {
                    DEBUG('a', "SocketServerCreate, port already used\n");
                }
                machine->WriteMem(userSock,4,(int)sock); //Ecrit dans le coté utilisateur l'adresse de la socket système
                machine->WriteMem(userSock+sizeof(int),1,2); //type de la socket

                break;
            }


            case SC_SocketClose: {
                DEBUG('a', "SocketClose, initiated by user program.\n");
                int userSock = machine->ReadRegister(4); //Adresse utilisateur

                int adrSock; int typeSocket;
                machine->ReadMem(userSock,4,&adrSock); //Lit l'adresse de la socket système
                machine->ReadMem(userSock+sizeof(int),1,&typeSocket); //Lit le type de la socket
                if(typeSocket==1) {
                    SocketClientTCP * sock = (SocketClientTCP *) adrSock;
                    sock->Close();
                }
                else if(typeSocket==2) {
                    SocketServerTCP * sock = (SocketServerTCP *) adrSock;
                    sock->Close();
                }

  
                break;
            }

            case SC_SocketSend: {
                DEBUG('a', "SocketSend, initiated by user program.\n");
                int userSock = machine->ReadRegister(4); //Adresse utilisateur
                int dataUser = machine->ReadRegister(5); //Donnée à envoyer
                int size = machine->ReadRegister(6); //Taille des données

                int adrSock;
                machine->ReadMem(userSock,4,&adrSock); //Lit l'adresse de la socket système
                SocketClientTCP * sock = (SocketClientTCP *) adrSock;

                char * data = new char[size]; //Les données à envoyer
                machine->copyDataFromMachine(dataUser,data,size); //récupère les données depuis le programme utilisateur

                printf("Taille:%d data:\"%s\"\n",size,data);

                int ret = sock->Write(data,size);
                machine->WriteRegister(2,ret); //Le nombre d'octets écrits

                delete data;
                break;
            }

            case SC_SocketReceive: {
                DEBUG('a', "SocketReceive, initiated by user program.\n");
                int userSock = machine->ReadRegister(4); //Adresse utilisateur
                int dataUser = machine->ReadRegister(5); //Pointeur des données où on doit stocker
                int size = machine->ReadRegister(6); //Taille des données à recevoir

                int adrSock;
                machine->ReadMem(userSock,4,&adrSock); //Lit l'adresse de la socket système
                SocketClientTCP * sock = (SocketClientTCP *) adrSock;

                char * data = new char[size]; //Les données à recevoir

                int ret = sock->Read(data,size);
                machine->copyDataToMachine(data,dataUser,size); //Met les données dans le programme utilisateur


                machine->WriteRegister(2,ret); //Le nombre d'octets lus

                delete data;
                break;
            }

            case SC_SocketAccept: {
                DEBUG('a', "SocketAccept, initiated by user program.\n");
                int userSock = machine->ReadRegister(4); //Adresse utilisateur
                int userSockResponse = machine->ReadRegister(5); //Adresse utilisateur de la socket reponse


                int adrSockServ;
                machine->ReadMem(userSock,4,&adrSockServ); //Lit l'adresse de la socket système
                SocketServerTCP * sock = (SocketServerTCP *) adrSockServ;

                int adrSockResp;
                machine->ReadMem(userSockResponse,4,&adrSockResp); //Lit l'adresse de la socket système
                SocketClientTCP * sockClient = (SocketClientTCP *) adrSockResp;

                sockClient = sock->Accept();
                if(sockClient ==NULL) {
                    DEBUG('a', "SocketAccept, failed return a null socket\n");
                    machine->WriteRegister(2,0); //Accept fonctionne pas
                }
                else {
                    machine->WriteMem(userSockResponse,4,(int)sockClient); //Ecrit dans le coté utilisateur l'adresse de la socket système
                    machine->WriteMem(userSockResponse+sizeof(int),1,1); //Indique que c'est une socket utilisateur

                    machine->WriteRegister(2,1); //Accept ok
                }



                break;
            }

            case SC_SocketConnect: {
                DEBUG('a', "SocketConnect, initiated by user program.\n");
                int userSock = machine->ReadRegister(4); //Adresse utilisateur
                int adr = machine->ReadRegister(5); //Adresse du serveur
                int port = machine->ReadRegister(6); //Port du serveur


                int adrSock;
                machine->ReadMem(userSock,4,&adrSock); //Lit l'adresse de la socket système
                SocketClientTCP * sock = (SocketClientTCP *) adrSock;

                int ret = sock->Connect(adr,port); //ret = 1 si la connection s'est bien passé, 0 si pas réussi à se connecter
                machine->WriteRegister(2,ret);
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
