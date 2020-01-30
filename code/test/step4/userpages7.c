#include "syscall.h"
#define THIS "770 "
#define THAT "771 "
#define THOS "772 "
#define THUS "773 "
#define THES "774 "
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
