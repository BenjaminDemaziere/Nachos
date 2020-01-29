#include "syscall.h"
#define THIS "880 "
#define THAT "881 "
#define THOS "882 "
#define THUS "883 "
#define THES "884 "
const int N = 10; // Choose it large enough!

void f(void *s){
  int i;
  for (i = 0; i < N; i++)
    PutString((char*)s);
}
main(){
  int i;
  UserThreadCreate(f, (void *) THIS);
  UserThreadCreate(f, (void *) THAT);
  UserThreadCreate(f, (void *) THOS);
  UserThreadCreate(f, (void *) THUS);
  UserThreadCreate(f, (void *) THES);
}
