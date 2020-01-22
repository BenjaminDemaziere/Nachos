# include "syscall.h"

int main()
{
    PutChar(GetChar());
    PutChar('\n');

    return 12;
}