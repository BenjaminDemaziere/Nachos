#include "syscall.h"
main(){
  int a = ForkExec("./makethreads");
  int b = ForkExec("./makethreads");

  return 0;
}
