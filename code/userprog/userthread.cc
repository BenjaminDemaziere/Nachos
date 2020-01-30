
#include "system.h"
#include "userthread.h"


// Permet de passer la fonction et les arguments au fork de thread
typedef struct ThreadUserArgs {
    int f;
    int arg;
    int adrExit;
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

    //Registre de pile
    machine->WriteRegister(StackReg, currentThread->spaceStack);

    //Registre de retour
    machine->WriteRegister(RetAddrReg,args->adrExit);

    //Supprime les arguments
    delete args;

    //Lance la simulation
    machine->Run();
}


/*
Toute les structures de données sont bien instanciées à la fin de la fonction
*/
int do_UserThreadCreate(int f, int arg, int adrExit) {
    int idThread = -1;
    Thread * thread = new Thread("Thread utilisateur"); //Création du thread noyau pour l'utilisateur

    if(thread==NULL) {
        DEBUG('t', "do_UserThreadCreate: Création thread système pour l'utilisateur impossible\n");
    }
    else {        
        //Initialise les structures pour l'addrspace
        //Retourne l'adresse de début de pile
        int begStack = currentThread->space->BeginningStackThread(thread);

        if(begStack!=-1) { //L'initialisation s'est bien passée

            ThreadUserArgs * args = new ThreadUserArgs;
            args->f = f;
            args->arg = arg;
            args->adrExit = adrExit;

            idThread = currentThread->space->NewIdThread(); //Génère un id unique pour le thread
            thread->idT = idThread; 
            thread->spaceStack = begStack; //Le début de la pile
            thread->Fork(StartUserThread,(int) args); //lancement de la fonction StartUserThread
        }
        else { //Erreur , création thread impossible
            DEBUG('t', "Création thread système pour l'utilisateur impossible, plus de place dans l'addrspace\n");
        }
    }

    return idThread;
}



/*
Fonction utilisée dans do_UserThreadExit pour que le thread qui exit relache les sémaphores
permettant au thread ayant fait join avec ce thread de terminer
*/
static void userThreadCompareSemaphore(int s) {
    Semaphore * sem = (Semaphore *) s;
    sem->V(); //Relache le sémaphore
}
void do_UserThreadExit() {
    if(currentThread->listSemaphoreJoin != NULL) { //S'il y a une liste de sémaphore
        currentThread->listSemaphoreJoin->Mapcar(userThreadCompareSemaphore);
    }
    currentThread->space->ClearThread(currentThread); //Désalloue le thread dans l'addrspace
    currentThread->Finish(); //Termine le thread
}


void do_UserThreadJoin(int idT) {
    currentThread->space->JoinThread(idT);
}

void do_UserThreadKill() {
    
}