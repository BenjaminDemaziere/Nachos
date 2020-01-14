#include "syscall.h"
#define THIS "440 "
#define THAT "441 "
#define THOS "442 "
#define THUS "443 "
#define THES "444 "
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
