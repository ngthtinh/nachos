// Help Program
// Print our information, and about ascii and sort program

#include "syscall.h"

int main()
{
    int n = ReadNum();
    PrintNum(n);

    Halt();
    /* not reached */
}