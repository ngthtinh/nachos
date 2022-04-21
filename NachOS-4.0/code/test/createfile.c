// Create File Program

#include "syscall.h"

int main()
{
    int result;
    result = Create("test.txt");

    if (result == 0)
        PrintString("Success!\n");
    else
        PrintString("Failed!\n");

    Halt();
    /* not reached */
}