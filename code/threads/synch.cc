// synch.cc 
//      Routines for synchronizing threads.  Three kinds of
//      synchronization routines are defined here: semaphores, locks 
//      and condition variables (the implementation of the last two
//      are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
//      Initialize a semaphore, so that it can be used for synchronization.
//
//      "debugName" is an arbitrary name, useful for debugging.
//      "initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore (const char *debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
//      De-allocate semaphore, when no longer needed.  Assume no one
//      is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore ()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
//      Wait until semaphore value > 0, then decrement.  Checking the
//      value and decrementing must be done atomically, so we
//      need to disable interrupts before checking the value.
//
//      Note that Thread::Sleep assumes that interrupts are disabled
//      when it is called.
//----------------------------------------------------------------------

void
Semaphore::P ()
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);	// disable interrupts

    while (value == 0)
      {				// semaphore not available
	  queue->Append ((void *) currentThread);	// so go to sleep
	  currentThread->Sleep ();
      }
    value--;			// semaphore available, 
    // consume its value

    (void) interrupt->SetLevel (oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
//      Increment semaphore value, waking up a waiter if necessary.
//      As with P(), this operation must be atomic, so we need to disable
//      interrupts.  Scheduler::ReadyToRun() assumes that threads
//      are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V ()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel (IntOff);

    thread = (Thread *) queue->Remove ();
    if (thread != NULL)		// make thread ready, consuming the V immediately
	scheduler->ReadyToRun (thread);
    value++;
    (void) interrupt->SetLevel (oldLevel);
}

int Semaphore::getValue() {
    return value;
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock (const char *debugName)
{
    name = debugName;
    state = FREE;
    listWaitingThreads = new List;
    threadOwner = NULL;
    numberLock = 0;
}

Lock::~Lock ()
{
    delete listWaitingThreads;
}
void
Lock::Acquire ()
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);	// disable interrupts

    if(currentThread == threadOwner) { //Si le thread qui a le verrou veut le reprendre
        numberLock ++;
    }
    else {
        while (state == BUSY) { 
            listWaitingThreads->Append((void *) currentThread);	// so go to sleep
            currentThread->Sleep ();
        }
        //Verrou disponible
        numberLock = 1;
        state = BUSY;
        threadOwner = currentThread;
    }

    (void) interrupt->SetLevel (oldLevel);	// re-enable interrupts
}
void
Lock::Release ()
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);	// disable interrupts

    if(currentThread == threadOwner) { //Si le thread qui a le verrou veut le libérer
        numberLock --;
        if(numberLock==0) { //Le thread a fait autant de fois release que lock
            state = FREE;
            threadOwner = NULL;
            Thread * thread = (Thread *) listWaitingThreads->Remove(); //Récupère un thread en attente
            if (thread != NULL) { //S'il y en a un, on lance le thread
                scheduler->ReadyToRun(thread);
            }
        }
    }

    (void) interrupt->SetLevel (oldLevel);	// re-enable interrupts
}

bool Lock::isHeldByCurrentThread () {
    return currentThread == threadOwner;
}

Condition::Condition (const char *debugName)
{
    name = debugName;
    listWaitingThreads = new List;

}

Condition::~Condition ()
{
    delete listWaitingThreads;
}
void
Condition::Wait (Lock * conditionLock)
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);	// disable interrupts

    if(conditionLock->isHeldByCurrentThread()) {
        listWaitingThreads->Append((void *) currentThread); //On fait attendre le thread
        conditionLock->Release(); //Relache le verrou
        currentThread->Sleep ();
        //Quand le thread est de nouveau actif il reprend le verrou
        conditionLock->Acquire();
    }

    (void) interrupt->SetLevel (oldLevel);	// re-enable interrupts
}

void
Condition::Signal (Lock * conditionLock)
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);	// disable interrupts

    if(conditionLock->isHeldByCurrentThread()) {
        Thread * thread = (Thread *) listWaitingThreads->Remove (); //Récupère un thread en attente
        if (thread != NULL) { //S'il y en a un, on lance le thread
            scheduler->ReadyToRun(thread);
        } 
    }
    (void) interrupt->SetLevel (oldLevel);	// re-enable interrupts

}
void
Condition::Broadcast (Lock * conditionLock)
{
    IntStatus oldLevel = interrupt->SetLevel (IntOff);	// disable interrupts

    if(conditionLock->isHeldByCurrentThread()) {
        //Réveille tous les thread en attente
        Thread * thread = (Thread *) listWaitingThreads->Remove ();
        while(thread != NULL) {
            scheduler->ReadyToRun(thread);
            thread = (Thread *) listWaitingThreads->Remove ();
        }
    }
    (void) interrupt->SetLevel (oldLevel);	// re-enable interrupts

}
