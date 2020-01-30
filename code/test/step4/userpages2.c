#include "syscall.h"
#define THIS "220 "
#define THAT "221 "
#define THOS "222 "
#define THUS "223 "
#define THES "224 "
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
