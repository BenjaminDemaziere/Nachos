/*
    Test de getchar fait en partie 2
*/

#include "syscall.h"




int main() {
    char a = GetChar();


    char b = GetChar();

    PutChar(a);
    PutChar('\n');

    PutChar(b);
    PutChar('\n');



    return 0;
}