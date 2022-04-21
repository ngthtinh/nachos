// Create File Program

#include "syscall.h"

#define MAX_LENGTH 255

int main()
{
    char filename[MAX_LENGTH];

    PrintString("Enter file name to create: ");
    ReadString(filename, MAX_LENGTH);

    if (Create(filename) == 0)
    {
        PrintString("Create file ");
        PrintString(filename);
        PrintString(" successfully!\n");
    }
    else
    {
        PrintString("Failed to create file: Invalid file name!\n");
    }
    
    Halt();
    /* not reached */
}