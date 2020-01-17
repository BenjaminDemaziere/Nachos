// addrspace.cc
//      Routines to manage address spaces (executing user programs).
//
//      In order to run a user program, you must:
//
//      1. link with the -N -T 0 option
//      2. run coff2noff to convert the object file to Nachos format
//              (Nachos object code format is essentially just a simpler
//              version of the UNIX executable object code format)
//      3. load the NOFF file into the Nachos file system
//              (if you haven't implemented the file system yet, you
//              don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"


#include <strings.h>		/* for bzero */

//----------------------------------------------------------------------
// SwapHeader
//      Do little endian to big endian conversion on the bytes in the
//      object file header, in case the file was generated on a little
//      endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void
SwapHeader (NoffHeader * noffH)
{
    noffH->noffMagic = WordToHost (noffH->noffMagic);
    noffH->code.size = WordToHost (noffH->code.size);
    noffH->code.virtualAddr = WordToHost (noffH->code.virtualAddr);
    noffH->code.inFileAddr = WordToHost (noffH->code.inFileAddr);
    noffH->initData.size = WordToHost (noffH->initData.size);
    noffH->initData.virtualAddr = WordToHost (noffH->initData.virtualAddr);
    noffH->initData.inFileAddr = WordToHost (noffH->initData.inFileAddr);
    noffH->uninitData.size = WordToHost (noffH->uninitData.size);
    noffH->uninitData.virtualAddr =
	WordToHost (noffH->uninitData.virtualAddr);
    noffH->uninitData.inFileAddr = WordToHost (noffH->uninitData.inFileAddr);
}

// ReadAtVirtual : Read a portion of a file, starting at "position",
//   and writes it in the virtual space defined by pageTable, virtualAddr
//   and numPages
//  "executable" -- file to read from
//	"virtualaddr" -- virtual address to write data to
//	"numBytes" -- the number of bytes to transfer
//	"position" -- the offset within the file of the first byte to be
//  "pageTable" -- page table to be used
//  "numPages" -- number of pages in the page table
//			read
static void ReadAtVirtual(OpenFile *executable, int virtualaddr,int numBytes,
                          int position,TranslationEntry *pageTable,unsigned numPages){
    TranslationEntry * tablePageMachine = machine->pageTable;
    unsigned int numPagesMachine = machine->pageTableSize;

    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;

    int i =0;
    char *buf = new char[numBytes];
    executable->ReadAt(buf,numBytes,position);
    while(i<numBytes){
      machine->WriteMem(virtualaddr+i, sizeof(char), (int)buf[i]);
      i++;
    }

    machine->pageTable = tablePageMachine;
    machine->pageTableSize = numPagesMachine;
}





//----------------------------------------------------------------------
// AddrSpace::AddrSpace
//      Create an address space to run a user program.
//      Load the program from a file "executable", and set everything
//      up so that we can start executing user instructions.
//
//      Assumes that the object code file is in NOFF format.
//
//      First, set up the translation from program memory to physical
//      memory.  For now, this is really simple (1:1), since we are
//      only uniprogramming, and we have a single unsegmented page table
//
//      "executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace (OpenFile * executable)
{
    NoffHeader noffH;
    unsigned int i, size;

    executable->ReadAt ((char *) &noffH, sizeof (noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) &&
	(WordToHost (noffH.noffMagic) == NOFFMAGIC))
	SwapHeader (&noffH);
    ASSERT (noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size + UserStackSize;	// we need to increase the size
    // to leave room for the stack
    numPages = divRoundUp (size, PageSize);
    size = numPages * PageSize;

    ASSERT (numPages <= frameprovider->NumAvailFrame());	// check we're not trying
    // to run anything too big --
    // at least until we have
    // virtual memory


    listThreads = new List;
    nbThreads = 1; //Nombre de thread actif (1 car il y a le thread main)
    uniqueIdT = 0;
    semAddrspace = new Semaphore("semAddrspace",1);
    semaphoreEnd = new Semaphore("semaphoreEnd",0);

    /*Création de la bitmap pour la table des pages*/
    usedPageTable = new BitMap(numPages);

    DEBUG ('a', "Initializing address space, num pages %d, size %d\n",
	   numPages, size);
// first, set up the translation

    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++)
      {
	  pageTable[i].virtualPage = i;
	  pageTable[i].physicalPage = frameprovider->GetEmptyFrame();
	  pageTable[i].valid = TRUE;
	  pageTable[i].use = FALSE;
	  pageTable[i].dirty = FALSE;
	  pageTable[i].readOnly = FALSE;	// if the code segment was entirely on
	  // a separate page, we could set its
	  // pages to be read-only
      }

// zero out the entire address space, to zero the unitialized data segment
// and the stack segment
    //bzero (machine->mainMemory, size);

// then, copy in the code and data segments into memory
    if (noffH.code.size > 0)
      {
	  DEBUG ('a', "Initializing code segment, at 0x%x, size %d\n",
		 noffH.code.virtualAddr, noffH.code.size);
     ReadAtVirtual(executable,noffH.code.virtualAddr,noffH.code.size,noffH.code.inFileAddr,pageTable,numPages);
	 /*executable->ReadAt (&(machine->mainMemory[noffH.code.virtualAddr]),
			      noffH.code.size, noffH.code.inFileAddr);*/

        // Les pages pour le code sont occupées
        int startPage = noffH.code.virtualAddr / PageSize; //The first page of the code
        int endPage = startPage +  noffH.code.size / PageSize; //The end page of the code
        for(int p = startPage ; p < endPage ;p++) {
            usedPageTable->Mark(p);
        }
        DEBUG ('a', "BitMap code: %d %d \n", startPage, endPage);

      }
    if (noffH.initData.size > 0)
      {
	  DEBUG ('a', "Initializing data segment, at 0x%x, size %d\n",
		 noffH.initData.virtualAddr, noffH.initData.size);
     ReadAtVirtual(executable,noffH.initData.virtualAddr,noffH.initData.size,noffH.initData.inFileAddr,pageTable,numPages);
	  /*executable->ReadAt (&
			      (machine->mainMemory
			       [noffH.initData.virtualAddr]),
			      noffH.initData.size, noffH.initData.inFileAddr);*/

        // Les pages pour la zone data sont occupées
        int startPage = noffH.initData.virtualAddr / PageSize; //The first page of the data
        int endPage = startPage +  noffH.initData.size / PageSize; //The end page of the data
        for(int p = startPage ; p < endPage ;p++) {
            usedPageTable->Mark(p);
        }
        DEBUG ('a', "BitMap data: %d %d \n", startPage, endPage);

      }

}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//      Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace ()
{
  // LB: Missing [] for delete
  // delete pageTable;
  int i =0;
  for(i ; i < numPages ; i++){
    frameprovider->ReleaseFrame(pageTable[i].physicalPage);
  }
  delete [] pageTable;
  delete usedPageTable;
  delete semAddrspace;
  delete semaphoreEnd;
  delete listThreads;
  // End of modification
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
//      Set the initial values for the user-level register set.
//
//      We write these directly into the "machine" registers, so
//      that we can immediately jump to user code.  Note that these
//      will be saved/restored into the currentThread->userRegisters
//      when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters ()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister (i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister (PCReg, 0);

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister (NextPCReg, 4);

    // Set the stack register to the end of the address space, where we
    // allocated the stack; but subtract off a bit, to make sure we don't
    // accidentally reference off the end!
    machine->WriteRegister (StackReg, numPages * PageSize - 16);

    // usedPageTable->Mark(numPages-1); //Indique que les pages sont réservées
    // usedPageTable->Mark(numPages-2);
    // usedPageTable->Mark(numPages-3);

    DEBUG ('a', "Initializing stack register to %d\n",
	   numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
//      On a context switch, save any machine state, specific
//      to this address space, that needs saving.
//
//      For now, nothing!
//----------------------------------------------------------------------

void
AddrSpace::SaveState ()
{
  pageTable = machine->pageTable;
  numPages = machine-> pageTableSize;
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
//      On a context switch, restore the machine state so that
//      this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void
AddrSpace::RestoreState ()
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}

int AddrSpace::NewIdThread() {
    semAddrspace->P();
    uniqueIdT++;
    semAddrspace->V();
    return uniqueIdT;
}

int AddrSpace::BeginningStackThread(Thread * thread) {
    semAddrspace->P();
    int numP = 0;
    //Cherche 3 pages consécutives libres
    while(numP<(int)numPages-2 && (usedPageTable->Test(numP)==1 || usedPageTable->Test(numP+1)==1 || usedPageTable->Test(numP+2)==1)) {
        numP++;
    }
    if(numP==(int)numPages-2) { //Erreur aucune page de libre
        DEBUG ('a', "Aucun espace pour allouer une nouvelle pile pour le thread\n");
        return -1;
    }
    else { //On occupe 3 page pour la pile
        usedPageTable->Mark(numP);
        usedPageTable->Mark(numP+1);
        usedPageTable->Mark(numP+2);
        numP+=2; //Le début de la pile

        //Met dans la liste le thread
        nbThreads++;
        listThreads->Append(thread);
    }
    semAddrspace->V();
    return (numP+1)*PageSize-16;
}

/*
    Fonction utilisée pour listThread->RemoveElement()
*/
int compareThread(void *a,void *b) {
    int cmp = 0;
    Thread *t1 = (Thread *)a;
    Thread *t2 = (Thread *)b;
    if(t1->idT==t2->idT) {
        cmp =1;
    }
    return cmp;
}

int AddrSpace::ClearThread(Thread * t) {
    semAddrspace->P();
    int succ=0;
    Thread * th = (Thread *) listThreads->RemoveElement((void *)(t), compareThread);
    if(th!=NULL) { //Le thread a bien été supprimé de la liste
        nbThreads--;
        succ = 1;
        int idT = (t->spaceStack)/PageSize;
        usedPageTable->Clear(idT);
        usedPageTable->Clear(idT-1);
        usedPageTable->Clear(idT-2);
        if(nbThreads==1) { //S'il n'y a plus de threads 
            semaphoreEnd->V(); //Le programme peut se terminer
        }
    }
    semAddrspace->V();
    return succ;
}

/*
    Fonction utilisée dans do_UserThreadJoin pour touver un thread à partir de son id
*/
static int userThreadCompare(void * id, void * t) {
    Thread * th = (Thread *) t;
    int * idT = (int *) id;
    return th->idT == *idT;
}
void AddrSpace::JoinThread(int idT) {
    semAddrspace->P();
    Semaphore * sem;
    //On récupére le thread qui à l'ID voulu
    Thread * th = (Thread *) currentThread->space->listThreads->IsPresent(&idT,userThreadCompare);
    if(th != NULL) { //Le thread existe
        sem = new Semaphore("Sem",0);
        th->listSemaphoreJoin->Append(sem); //Ajoute le semaphore à la liste des sem du thread
        semAddrspace->V();
        sem->P(); //Le thread courant attend le thread idT
        delete sem; //Supprime le sémaphore
    }
    else {
        DEBUG('t', "ThreadJoin: le thread %d n'existe pas\n", idT);
    }
}
