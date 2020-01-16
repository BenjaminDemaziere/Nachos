#include "syscall.h"
main(){
  int a = ForkExec("../test/makethreads");
  
  return a;
}
