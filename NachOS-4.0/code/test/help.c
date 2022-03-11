// Help Program
// Print our information, and about ascii and sort program

#include "syscall.h"

int main()
{
    int n;
    n = RandomNum();
    while (n > 0)
    {
        PrintChar(n % 10 + 48);
        n /= 10;
    }

    Halt();
    /* not reached */
}