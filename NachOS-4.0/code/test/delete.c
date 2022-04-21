// Delete File Program

#include "syscall.h"

#define MAX_LENGTH 255

int main()
{
    // Declaire
    char filename[MAX_LENGTH];

    // Read filename
    PrintString("Enter file name to delete: ");
    ReadString(filename, MAX_LENGTH);

    if (Remove(filename) == 0)
    {
        PrintString("Delete file ");
        PrintString(filename);
        PrintString(" successfully!\n");
    }
    else
    {
        PrintString("Failed to delete file!\n");
    }
    
    Halt();
    /* not reached */
}