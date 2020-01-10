/*
    Test de getstring fait en partie 2
*/

#include "syscall.h"


int main() {
    //Test chaine de base
    char str [10];

    GetString(str,10);

    PutString(str);

    return 0;
}
