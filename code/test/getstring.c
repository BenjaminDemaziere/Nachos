# include "syscall.h"

# define STRING_SIZE 64

int main()
{
    char string[STRING_SIZE];

    GetString (string, STRING_SIZE);
    
    if (string > 0)
        PutString (string);

    return 11;
}