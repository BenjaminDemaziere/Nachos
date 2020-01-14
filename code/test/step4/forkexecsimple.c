#include "syscall.h"
//Création de deux processus simples
main(){

  PutString("Création du premier processus. \n");
  int a = ForkExec("./makethreads0");
  PutString("Création du second processus. \n");
  int b = ForkExec("./makethreads1");

  return 0;
}
