#include "syscall.h"
//Création de deux processus simples
int main(){

  PutString("Création du premier processus. \n");
  ForkExec("./makethreads0");
  PutString("Création du second processus. \n");
  ForkExec("./makethreads1");

  return 0;
}
