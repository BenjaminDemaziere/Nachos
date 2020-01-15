#include "syscall.h"


void helloWorld(){
  int i = 0;
  while(i<10){
    PutChar('a');
    PutChar('\n');
  }
  UserThreadExit();
}

void byeWorld(){
  int i = 0;
  while(i<10){
    PutChar('b');
    PutChar('\n');
  }
  UserThreadExit();
  }

int main() {

  int t = UserThreadCreate(helloWorld,0);
  int y = 0;
  while(y<10){
    PutChar('b');
    PutChar('\n');
  }
  return 0;
}
