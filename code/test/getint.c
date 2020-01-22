# include "syscall.h"

int main()
{
    int res;

    GetInt (&res);
    PutInt (res);
    PutChar ('\n');

    return res;
}