#include "syscall.h"
#define THIS "660 "
#define THAT "661 "
#define THOS "662 "
#define THUS "663 "
#define THES "664 "
const int N = 10; // Choose it large enough!

void f(void *s){
  int i;
  for (i = 0; i < N; i++)
    PutString((char*)s);
}
int main(){
  UserThreadCreate(f, (void *) THIS);
  UserThreadCreate(f, (void *) THAT);
  UserThreadCreate(f, (void *) THOS);
  UserThreadCreate(f, (void *) THUS);
  UserThreadCreate(f, (void *) THES);
}
