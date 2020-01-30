#include "syscall.h"
#define THIS "110 "
#define THAT "111 "
#define THOS "112 "
#define THUS "113 "
#define THES "114 "
const int N = 10; // Choose it large enough!

void f(void *s){
  int i;
  for (i = 0; i < N; i++)
    PutString((char*)s);

}

int main()
{
  UserThreadCreate(f, (void *) THIS);
  UserThreadCreate(f, (void *) THAT);
  UserThreadCreate(f, (void *) THOS);
  UserThreadCreate(f, (void *) THUS);
  UserThreadCreate(f, (void *) THES);
}
