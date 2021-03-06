/* syscalls.h
 * 	Nachos system call interface.  These are Nachos kernel operations
 * 	that can be invoked from user programs, by trapping to the kernel
 *	via the "syscall" instruction.
 *
 *	This file is included by user programs and by the Nachos kernel.
 *
 * Copyright (c) 1992-1993 The Regents of the University of California.
 * All rights reserved.  See copyright.h for copyright notice and limitation
 * of liability and disclaimer of warranty provisions.
 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "copyright.h"

/* system call codes -- used by the stubs to tell the kernel which system call
 * is being asked for
 */
#define SC_Halt		0
#define SC_Exit		1
#define SC_Exec		2
#define SC_Join		3
#define SC_Create	4
#define SC_Open		5
#define SC_Read		6
#define SC_Write	7
#define SC_Close	8
#define SC_Fork		9
#define SC_Yield	10
#define SC_PutChar  11
#define SC_PutString 12
#define SC_GetChar  13
#define SC_GetString 14
#define SC_PutInt  15
#define SC_GetInt 16

#define SC_UserThreadCreate 17
#define SC_UserThreadExit 18
#define SC_UserThreadJoin 19
#define SC_SemaphoreInit 20
#define SC_SemaphoreP 21
#define SC_SemaphoreV 22
#define SC_SemaphoreFree 23

#define SC_UserMkdir 30
#define SC_UserChdir 31
#define SC_UserRmdir 32
#define SC_UserListdir 33
#define SC_UserMkFile 34
#define SC_UserRmFile 35
#define SC_UserOpenFile 36
#define SC_UserCloseFile 37




#define SC_ForkExec 40


#define SC_SocketCreate 50
#define SC_SocketServerCreate 56
#define SC_SocketClose 51
#define SC_SocketSend 52
#define SC_SocketReceive 53
#define SC_SocketAccept 54
#define SC_SocketConnect 55



#ifdef IN_USER_MODE

// LB: This part is read only on compiling the test/*.c files.
// It is *not* read on compiling test/start.S


/* The system call interface.  These are the operations the Nachos
 * kernel needs to support, to be able to run user programs.
 *
 * Each of these is invoked by a user program by simply calling the
 * procedure; an assembly language stub stuffs the system call code
 * into a register, and traps to the kernel.  The kernel procedures
 * are then invoked in the Nachos kernel, after appropriate error checking,
 * from the system call entry point in exception.cc.
 */

/* Stop Nachos, and print out performance stats */
void Halt () __attribute__((noreturn));


/* Address space control operations: Exit, Exec, and Join */

/* This user program is done (status = 0 means exited normally). */
void Exit (int status) __attribute__((noreturn));

/* A unique identifier for an executing user program (address space) */
typedef int SpaceId;

/* Run the executable, stored in the Nachos file "name", and return the
 * address space identifier
 */
SpaceId Exec (char *name);

/* Only return once the the user program "id" has finished.
 * Return the exit status.
 */
int Join (SpaceId id);


/* File system operations: Create, Open, Read, Write, Close
 * These functions are patterned after UNIX -- files represent
 * both files *and* hardware I/O devices.
 *
 * If this assignment is done before doing the file system assignment,
 * note that the Nachos file system has a stub implementation, which
 * will work for the purposes of testing out these routines.
 */

/* A unique identifier for an open Nachos file. */
typedef int OpenFileId;

/* when an address space starts up, it has two open files, representing
 * keyboard input and display output (in UNIX terms, stdin and stdout).
 * Read and Write can be used directly on these, without first opening
 * the console device.
 */

#define ConsoleInput	0
#define ConsoleOutput	1

/* Create a Nachos file, with "name" */
void Create (char *name);

/* Open the Nachos file "name", and return an "OpenFileId" that can
 * be used to read and write to the file.
 */
OpenFileId Open (char *name);

/* Write "size" bytes from "buffer" to the open file. */
void Write (char *buffer, int size, OpenFileId id);

/* Read "size" bytes from the open file into "buffer".
 * Return the number of bytes actually read -- if the open file isn't
 * long enough, or if it is an I/O device, and there aren't enough
 * characters to read, return whatever is available (for I/O devices,
 * you should always wait until you can return at least one character).
 */
int Read (char *buffer, int size, OpenFileId id);

/* Close the file, we're done reading and writing to it. */
void Close (OpenFileId id);



/* User-level thread operations: Fork and Yield.  To allow multiple
 * threads to run within a user program.
 */

/* Fork a thread to run a procedure ("func") in the *same* address space
 * as the current thread.
 */
void Fork (void (*func) ());

/* Yield the CPU to another runnable thread, whether in this address space
 * or not.
 */
void Yield ();

/* (Etape2) affichage d'un caractère
*/
void PutChar(char c);

/* (Etape2) affichage d'une chaine de caractère
*/
void PutString(char * c);

char GetChar();

char * GetString();

void PutInt(int n);

int GetInt();


/* (étape 3) création de thread, lance la fonction f avec les arguments args
*/
int UserThreadCreate(void f(void *arg), void *arg);


void UserThreadExit();

/*prend en paramètre le numéro du thread retourné par  UserThreadCreate*/
void UserThreadJoin(int idT);


/*(étape 3) Sémaphore*/
typedef struct sem_t {
    int sem; //The system semaphor
}sem_t;

void SemaphoreInit(sem_t * sem, int val);

void SemaphoreP(sem_t * sem);

void SemaphoreV(sem_t * sem);

void SemaphoreFree(sem_t * sem);



/*
étape 4, création d'un nouveau processus, exécutant le programme s
renvoie 0 si tout se passe bien
*/
int ForkExec(char *s);


/*
étape 5, gestion des fichiers et répertoires
*/
//Crée un nouveau répertoire de nom dirName s'il n'existe pas
//déja dans le répertoire courant
//Renvoie 0 en cas de succès , 1 sinon
int UserMkdir(char *dirName);

//Déplace la répertoire courant vers le répertoire de nom dirName,
//si il existe dans le répertoire courant
//Renvoie 0 en cas de succès , 1 sinon
int UserChdir(char *dirName);

//Supprime le répertoire de nom dirName
//si il existe dans le répertoire courant
//Renvoie 0 en cas de succès , 1 sinon
int UserRmdir(char *dirName);

//Affiche le contenu du répertoire courant
void UserListdir();

//Crée un fichier de nom fileName
//fileName ne dois pas être utilisé dans le répertoire courant
//Renvoie 0 en cas de succès , 1 sinon
int UserMkFile(char *fileName, int size);

//Détruit un fichier de nom fileName
//fileName dois être dans le répertoire courant
//Renvoie 0 en cas de succès , 1 sinon
int UserRmFile(char *fileName);

//Ouvre un fichier de nom fileName
//fileName dois être dans le répertoire courant
//Il dois rester de la place dans la table de fichiers du système
//Renvoie le file descriptor du fichier ouvert en cas de succès
//-1 sinon
int UserOpenFile(char *fileName);

//Ferme un fichier de nom fileName
void UserCloseFile(int fd);


/*
étape 6, réseau
*/
typedef struct socket_t {
    int socket; //La socket 
    char type; // 1=Client, 2=Server
}socket_t;

//Créer une socket
void SocketCreate(socket_t * socket);
//Se connecte au serveur à l'adresse adr, et au port
int SocketConnect(socket_t * socket,int adr,int port); 


//Créer une socket serveur sur le port donné 
int SocketServerCreate(socket_t * socket,int port);

//Accepte une connection sur le port donné plus haut
// clientResponse est la socket qui permettra de communiquer avec le client
int SocketAccept(socket_t * socket,socket_t * clientResponse); 


//Ferme la socket
void SocketClose(socket_t * socket); 

//Envoie et réception de données sur la socket
int SocketSend(socket_t * socket,char * data,int size); 
int SocketReceive(socket_t * socket,char * data, int size); 



#endif // IN_USER_MODE

#endif /* SYSCALL_H */
