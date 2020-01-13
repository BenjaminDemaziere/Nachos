#include "syscall.h"


void helloWorld(){
  PutString("Hello World");
  UserThreadExit();
}

void byeWorld(){
  PutString("Bye World");
  UserThreadExit();
}

int main() {

  int i = UserThreadCreate(helloWorld,0);
  int d = UserThreadCreate(byeWorld,0);


  return 0;
}
