# include "syscall.h"

int main()
{
    int res = 15;

    PutInt (res);
    PutChar ('\n');

    return res;
}