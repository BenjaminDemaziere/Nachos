#include "syscall.h"
int main(){
  
  ForkExec("./step3/makethreads");
  ForkExec("./step3/makethreads");

  return 0;
}
