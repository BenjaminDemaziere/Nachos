#include "syscall.h"
#define THIS "330 "
#define THAT "331 "
#define THOS "332 "
#define THUS "333 "
#define THES "334 "
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
