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

    int result, count;

    result = Open("test.txt");
    
    if (result == -1)
        PrintString("Open Failed!\n");
    else
    {
        PrintString("Open Successfully! OpenFileID = ");
        PrintNum(result);
        PrintString("\n");
    }

    count = Write("Hello!", 100, result);
    if (count == -1)
        PrintString("Write Failed!\n");
    else
        PrintString("Write Successful!\n");

    PrintString("Count = ");
    PrintNum(count);
    PrintString("\n");

    result = Close(result);

    if (result == -1)
        PrintString("Close Failed!\n");
    else
        PrintString("Close Successful!\n");
    
    Halt();
    /* not reached */
}