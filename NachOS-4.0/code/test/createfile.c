// Create File Program

#include "syscall.h"

int main()
{
    /*
    int result;
    result = Create("test.txt");

    if (result == 0)
        PrintString("Success!\n");
    else
        PrintString("Failed!\n");
    */

    int result;
    result = Open("test.txt");
    
    if (result == -1)
        PrintString("Open Failed!");
    else
    {
        PrintString("Open Successfully! OpenFileID = ");
        PrintNum(result);
        PrintString("\n");
    }
    
    Halt();
    /* not reached */
}