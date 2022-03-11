// Help Program
// Print our information, and about ascii and sort program

#include "syscall.h"

int main()
{
    char buffer[10];
    ReadString(buffer, 10);

    PrintString(buffer);

    Halt();
    /* not reached */
}