
#include "system.h"
#include "userthread.h"


// Permet de passer la fonction et les arguments au fork de thread
typedef struct ThreadUserArgs {
    int f;
    int arg;
}ThreadUserArgs;


static void StartUserThread(int f) {
    DEBUG('t', "StartUserThread\n");

    ThreadUserArgs * args = (ThreadUserArgs *) f; //Pointeur vers la fonction et les arguments 

    //Initialise les registres à 0
    for (int r=NumGPRegs; r<NumTotalRegs; r++) {
        machine->WriteRegister(r,0);
    }

    //Initialise le pointeur de pile vers l'instruction de la fonction
    machine->WriteRegister(PCReg,args->f);
    //Initialise NextPC à l'instruction suivante de PC
    machine->WriteRegister (NextPCReg, args->f+4);


    //Lui passe ses arguments dans le registre 4
    machine->WriteRegister(4,args->arg);

    //Initialise le pointeur de pile à 3 pages au dessous du pointeur de programme principal
    int begStack = currentThread->space->BeginningStackThread();
    if(begStack!=-1) {
        machine->WriteRegister(StackReg, begStack);

        //Lance la simulation
        machine->Run();

    }
    else { //Erreur , création thread impossible

    }
}


int do_UserThreadCreate(int f, int arg) {
    int idThread = -1;
    Thread * thread = new Thread("Thread utilisateur"); //Création du thread noyau pour l'utilisateur

    if(thread==NULL) {
        DEBUG('t', "Création thread système pour l'utilisateur impossible\n");
    }
    else {
        ThreadUserArgs * args = new ThreadUserArgs;
        args->f = f;
        args->arg = arg;

        thread->Fork(StartUserThread,(int) args); //lancement de la fonction StartUserThread
        currentThread->Yield();
        delete args;
    }

    return idThread; //TODO
}




void do_UserThreadExit() {
    // currentThread->space->();

    currentThread->Finish();
}
