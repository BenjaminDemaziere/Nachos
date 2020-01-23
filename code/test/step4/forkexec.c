#include "syscall.h"
int main(){
  
  ForkExec("./makethreads");
  ForkExec("./makethreads");

  return 0;
}
