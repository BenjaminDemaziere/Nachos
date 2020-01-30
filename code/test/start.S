/* Start.s
 *	Assembly language assist for user programs running on top of Nachos.
 *
 *	Since we don't want to pull in the entire C library, we define
 *	what we need for a user program here, namely Start and the system
 *	calls.
 */

#include "syscall.h"

        .text
        .align  2

/* -------------------------------------------------------------
 * __start
 *	Initialize running a C program, by calling "main".
 *
 * 	NOTE: This has to be first, so that it gets loaded at location 0.
 *	The Nachos kernel always starts a program by jumping to location 0.
 * -------------------------------------------------------------
 */

	.globl __start
	.ent	__start
__start:
	jal	main
	move	$4,$2
	jal	Exit	 /* if we return from main, exit(0) */
	.end __start

/* -------------------------------------------------------------
 * System call stubs:
 *	Assembly language assist to make system calls to the Nachos kernel.
 *	There is one stub per system call, that places the code for the
 *	system call into register r2, and leaves the arguments to the
 *	system call alone (in other words, arg1 is in r4, arg2 is
 *	in r5, arg3 is in r6, arg4 is in r7)
 *
 * 	The return value is in r2. This follows the standard C calling
 * 	convention on the MIPS.
 * -------------------------------------------------------------
 */

	.globl Halt
	.ent	Halt
Halt:
	addiu $2,$0,SC_Halt
	syscall
	j	$31
	.end Halt

	.globl Exit
	.ent	Exit
Exit:
	addiu $2,$0,SC_Exit
	syscall
	j	$31
	.end Exit

	.globl Exec
	.ent	Exec
Exec:
	addiu $2,$0,SC_Exec
	syscall
	j	$31
	.end Exec

	.globl Join
	.ent	Join
Join:
	addiu $2,$0,SC_Join
	syscall
	j	$31
	.end Join

	.globl Create
	.ent	Create
Create:
	addiu $2,$0,SC_Create
	syscall
	j	$31
	.end Create

	.globl Open
	.ent	Open
Open:
	addiu $2,$0,SC_Open
	syscall
	j	$31
	.end Open

	.globl Read
	.ent	Read
Read:
	addiu $2,$0,SC_Read
	syscall
	j	$31
	.end Read

	.globl Write
	.ent	Write
Write:
	addiu $2,$0,SC_Write
	syscall
	j	$31
	.end Write

	.globl Close
	.ent	Close
Close:
	addiu $2,$0,SC_Close
	syscall
	j	$31
	.end Close

	.globl Fork
	.ent	Fork
Fork:
	addiu $2,$0,SC_Fork
	syscall
	j	$31
	.end Fork

	.globl Yield
	.ent	Yield
Yield:
	addiu $2,$0,SC_Yield
	syscall
	j	$31
	.end Yield

	.globl PutChar
	.ent PutChar
PutChar:
	addiu $2,$0,SC_PutChar
	syscall
	j	$31
	.end PutChar

	.globl PutString
	.ent PutString
PutString:
	addiu $2,$0,SC_PutString
	syscall
	j	$31
	.end PutString

	.globl GetChar
	.ent GetChar
GetChar:
	addiu $2,$0,SC_GetChar
	syscall
	j	$31
	.end GetChar

	.globl GetString
	.ent GetString
GetString:
	addiu $2,$0,SC_GetString
	syscall
	j	$31
	.end GetString

	.globl PutInt
	.ent PutInt
PutInt:
	addiu $2,$0,SC_PutInt
	syscall
	j	$31
	.end PutInt

	.globl GetInt
	.ent GetInt
GetInt:
	addiu $2,$0,SC_GetInt
	syscall
	j	$31
	.end GetInt

	.globl UserThreadCreate
	.ent UserThreadCreate
UserThreadCreate:
	la $6, UserThreadExit /*Use to call UserThreadExit at the end of the thread*/
	addiu $2,$0,SC_UserThreadCreate
	syscall
	j	$31
	.end UserThreadCreate

	.globl UserThreadExit
	.ent UserThreadExit
UserThreadExit:
	addiu $2,$0,SC_UserThreadExit
	syscall
	j	$31
	.end UserThreadExit

	.globl UserThreadJoin
	.ent UserThreadJoin
UserThreadJoin:
	addiu $2,$0,SC_UserThreadJoin
	syscall
	j	$31
	.end UserThreadJoin

	.globl SemaphoreInit
	.ent SemaphoreInit
SemaphoreInit:
	addiu $2,$0,SC_SemaphoreInit
	syscall
	j	$31
	.end SemaphoreInit

	.globl SemaphoreP
	.ent SemaphoreP
SemaphoreP:
	addiu $2,$0,SC_SemaphoreP
	syscall
	j	$31
	.end SemaphoreP

	.globl SemaphoreV
	.ent SemaphoreV
SemaphoreV:
	addiu $2,$0,SC_SemaphoreV
	syscall
	j	$31
	.end SemaphoreV

	.globl SemaphoreFree
	.ent SemaphoreFree
SemaphoreFree:
	addiu $2,$0,SC_SemaphoreFree
	syscall
	j	$31
	.end SemaphoreFree

  .globl ForkExec
  .ent ForkExec
ForkExec:
  addiu $2,$0,SC_ForkExec
  syscall
  j	$31
  .end ForkExec

  .globl UserMkdir
  .ent UserMkdir
UserMkdir:
  addiu $2,$0,SC_UserMkdir
  syscall
  j	$31
  .end UserMkdir

  .globl UserChdir
  .ent UserChdir
UserChdir:
  addiu $2,$0,SC_UserChdir
  syscall
  j	$31
  .end UserChdir

  .globl UserRmdir
  .ent UserRmdir
UserRmdir:
  addiu $2,$0,SC_UserRmdir
  syscall
  j	$31
  .end UserRmdir

  .globl UserListdir
  .ent UserListdir
UserListdir:
  addiu $2,$0,SC_UserListdir
  syscall
  j	$31
  .end UserListdir

  .globl UserMkFile
  .ent UserMkFile
UserMkFile:
  addiu $2,$0,SC_UserMkFile
  syscall
  j	$31
  .end UserMkFile

  .globl UserRmFile
  .ent UserRmFile
UserRmFile:
  addiu $2,$0,SC_UserRmFile
  syscall
  j	$31
  .end UserRmFile

  .globl UserOpenFile
  .ent UserOpenFile
UserOpenFile:
  addiu $2,$0,SC_UserOpenFile
  syscall
  j	$31
  .end UserOpenFile

  .globl UserCloseFile
  .ent UserCloseFile
UserCloseFile:
  addiu $2,$0,SC_UserCloseFile
  syscall
  j	$31
  .end UserCloseFile


	.globl SocketCreate
	.ent SocketCreate
SocketCreate:
	addiu $2,$0,SC_SocketCreate
	syscall
	j	$31
	.end SocketCreate

	.globl SocketServerCreate
	.ent SocketServerCreate
SocketServerCreate:
	addiu $2,$0,SC_SocketServerCreate
	syscall
	j	$31
	.end SocketServerCreate

	.globl SocketClose
	.ent SocketClose
SocketClose:
	addiu $2,$0,SC_SocketClose
	syscall
	j	$31
	.end SocketClose

	.globl SocketSend
	.ent SocketSend
SocketSend:
	addiu $2,$0,SC_SocketSend
	syscall
	j	$31
	.end SocketSend

	.globl SocketReceive
	.ent SocketReceive
SocketReceive:
	addiu $2,$0,SC_SocketReceive
	syscall
	j	$31
	.end SocketReceive

	.globl SocketAccept
	.ent SocketAccept
SocketAccept:
	addiu $2,$0,SC_SocketAccept
	syscall
	j	$31
	.end SocketAccept

	.globl SocketConnect
	.ent SocketConnect
SocketConnect:
	addiu $2,$0,SC_SocketConnect
	syscall
	j	$31
	.end SocketConnect

/* dummy function to keep gcc happy */
        .globl  __main
        .ent    __main
__main:
        j       $31
        .end    __main
