#include "machine.h"
#include "thread.h"
#include "system.h"
#include "userthread.h"

int do_UserThreadCreate(int f, int arg){
  DEBUG('a',"UserThreadCreate, initiated by user program.\n");

  int *args[2];
  args[0] = &f;
  args[1] = &arg;

  Thread *newThread = new Thread ("userThread");
  newThread->space = currentThread->space; //On copie l'espace d'addressage
  newThread->parent = currentThread;
  newThread->Fork((VoidFunctionPtr)StartUserThread,(int)&args);
  currentThread->childCounter++;
  //if(newThread->stackTop == NULL){//Dans le cas ou la création échoue
  if(machine->ReadRegister(StackReg) == 0){
    // On retourne -1 pour signaler l'erreur.
    currentThread->childCounter--;
    return -1;
  }
  return 0;
  //}

  //newThread->Finish();
}

extern void do_UserThreadExit(){

  currentThread->Finish();
  currentThread->parent->childCounter--;
  //TODO détruire les adrspace
}

static void StartUserThread(int* f){
  int *args[2] ;
  args[0] = f;
  args[1] = (f++);
  int i;

  for (i = 0; i < NumTotalRegs; i++){ //Initialisation des registres a 0

    machine->WriteRegister(i,0);
  }

  machine->WriteRegister (PCReg, *(args[0])); //On initialise le PC et le nextPC au début de f
  machine->WriteRegister (NextPCReg, *(args[0]+4));

  machine->WriteRegister(4,*args[1]); //On charge l'argument de f dans le registre 4


  currentThread->stackTop = currentThread->parent->stackTop - (2 * PageSize);
  machine->Run(); //On lance l'exécution du programme utilisateur
}
