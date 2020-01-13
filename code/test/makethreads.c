#include "userthread.h"
#include "syscall.h"
#include "thread.h"

int main() {

  Thread t1 = UserThreadCreate(helloWorld,NULL);
  Thread t2 = UserThreadCreate(byeWorld,NULL);
  t1->UserThreadExit();
  t2->UserThreadExit();

  return 0;
}

void helloWorld(){
  PutString("Hello World");
}

void byeWorld(){
  PutString("Bye World");
}
