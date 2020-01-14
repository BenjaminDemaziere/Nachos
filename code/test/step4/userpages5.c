#include "syscall.h"
#define THIS "550 "
#define THAT "551 "
#define THOS "552 "
#define THUS "553 "
#define THES "554 "
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
