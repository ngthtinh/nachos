// Concatenate Files Program

#include "syscall.h"

#define MAX_LENGTH 255

int main()
{
    // Declaire
    int i, size, openFileId_1, openFileId_2;
    char c, filename_1[MAX_LENGTH], filename_2[MAX_LENGTH];

    // Read filename 1
    PrintString("Enter file name 1: ");
    ReadString(filename_1, MAX_LENGTH);

    // Open file 1
    openFileId_1 = Open(filename_1);

    if (openFileId_1 == -1)
    {
        PrintString("Cannot open file!\n");
        Halt();
        return 0;
    }

    // Read filename 2
    PrintString("Enter file name 2: ");
    ReadString(filename_2, MAX_LENGTH);

    // Open file 2
    openFileId_2 = Open(filename_2);

    if (openFileId_2 == -1)
    {
        PrintString("Cannot open file!\n");
        Halt();
        return 0;
    }

    // Get file size
    size = Seek(-1, openFileId_1);
    Seek(0, openFileId_1);

    Seek(-1, openFileId_2);

    // Copy each byte
    for (i = 0; i < size; i++)
    {
        Read(&c, 1, openFileId_1);
        Write(&c, 1, openFileId_2);
    }

    // Close file
    Close(openFileId_1);
    Close(openFileId_2);

    PrintString("Concatenate Done! Check file ");
    PrintString(filename_2);
    PrintString("\n");

    Halt();
    /* not reached */
}