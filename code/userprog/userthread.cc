
int do_UserThreadCreate(int f, int arg){
  DEBUG('a',"UserThreadCreate, initiated by user program.\n");
  //Récuperation des arguments
  void *f = (void*)arg1;
  void *arg = (void*)arg2;

  void *args[2];
  args[0] = f;
  args[1] = arg;

  Thread *newThread = new Thread ("userThread");
  newThread->space=currentThread->space;
  newThread->Fork(StartUserThread,args);
  if(newThread->stack == NULL){//Dans le cas ou la création échoue
    machine->WriteRegister(2,-1)// On écrit -1 dans le registre de retour pour signaler l'erreur.
  }
  newThread->Finish();
}

extern void do_UserThreadExit(){
  
}

static void StartUserThread(int f){

}
