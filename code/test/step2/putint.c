/*
    Test de putint fait en partie 2
*/

#include "syscall.h"




int main() {
    PutInt(1);
    PutChar('\n');

    PutInt(12);
    PutChar('\n');

    // Nombre trop grand
    PutInt(5000000000);
    PutChar('\n');

    

    return 0;
}