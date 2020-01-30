/*
    Test de putint fait en partie 2
*/

#include "syscall.h"




int main() {
    PutInt(1);
    PutChar('\n');

    PutInt(12);
    PutChar('\n');

    // Nombre grand
    PutInt(2147483647);
    PutChar('\n');

    PutInt(-4);
    PutChar('\n');

    

    return 0;
}