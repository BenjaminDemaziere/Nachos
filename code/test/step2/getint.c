/*
    Test de getint fait en partie 2
*/

#include "syscall.h"




int main() {
    int a = GetInt();

    PutInt(a);
    PutChar('\n');

    return 0;
}