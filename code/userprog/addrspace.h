// addrspace.h 
//      Data structures to keep track of executing user programs 
//      (address spaces).
//
//      For now, we don't keep any information about address spaces.
//      The user level CPU state is saved and restored in the thread
//      executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "bitmap.h"
#include "list.h"


#define UserStackSize		2048	// increase this as necessary!



/*Indique que ces classes existent car on ne peut pas importer leur .h*/
class Semaphore;
class Thread;


class AddrSpace
{
  public:
    AddrSpace (OpenFile * executable);	// Create an address space,
    // initializing it with the program
    // stored in the file "executable"
    ~AddrSpace ();		// De-allocate an address space

    void InitRegisters ();	// Initialize user-level CPU registers,
    // before jumping to user code

    void SaveState ();		// Save/restore address space-specific
    void RestoreState ();	// info on a context switch 

  //Retourne l'adresse du pointeur de pile pour le nouveau thread
  //Retourne -1 si aucun n'espace est disponible pour la pile 
  //Effet collatéral: Alloue 3 pages pour la pile, incrémente le nombre de threads, et ajout le thread à la liste
    int BeginningStackThread(Thread * t);

  //Désalloue la place sur la pile et enlève de la liste le thread idT, décrémente le nombre de threads
  //Renvoie 1 si la désallocation s'est bien passé, 0 sinon
    int ClearThread(Thread * t);

  //Return the number of user threads in this addrspace
    int getNumberThreads();

    //Return a new id for the thread
    int NewIdThread();


  private:
      TranslationEntry * pageTable;	// Assume linear page table translation
    // for now!
    unsigned int numPages;	// Number of pages in the virtual 
    // address space

      int nbThreads; //Nombre de threads utilisateurs créés dans le processus
    
      BitMap  * usedPageTable; //BitMap indiquant qu'elle page est occupée

      List * listThreads; //Liste des threads créés dans le processus

      unsigned int uniqueIdT; //Nombre pour créer des id uniques pour les threads

      //On protège méthodes qui sont peuvent être accédés par plusieurs threads
      Semaphore * semAddrspace;

};

#endif // ADDRSPACE_H
