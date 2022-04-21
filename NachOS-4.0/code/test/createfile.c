// Create File Program

#include "syscall.h"

int main()
{
    int result;
    result = Create("test.txt");

    if (result == 0)
        PrintString("Success!");
    else
        PrintString("Failed!");

    Halt();
    /* not reached */
}