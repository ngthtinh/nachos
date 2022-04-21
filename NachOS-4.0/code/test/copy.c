// Copy File Program

#include "syscall.h"

#define MAX_LENGTH 255

int main()
{
    // Declaire
    int i, size, openFileId_1, openFileId_2;
    char c, filename_1[MAX_LENGTH], filename_2[MAX_LENGTH];

    // Read filename 1
    PrintString("Enter source file name: ");
    ReadString(filename_1, MAX_LENGTH);

    // Open file 1
    openFileId_1 = Open(filename_1);

    // Copy content
    if (openFileId_1 >= 0)
    {
        // Read filename 2
        PrintString("Enter destination file name: ");
        ReadString(filename_2, MAX_LENGTH);

        // Create file 2
        Create(filename_2);

        // Open file 2
        openFileId_2 = Open(filename_2);

        // Get file size
        size = Seek(-1, openFileId_1);
        Seek(0, openFileId_1);

        // Copy each byte
        for (i = 0; i < size; i++)
        {
            Read(&c, 1, openFileId_1);
            Write(&c, 1, openFileId_2);
        }

        // Close file
        Close(openFileId_1);
        Close(openFileId_2);

        PrintString("Copy completed!\n");
    }
    else
    {
        PrintString("Cannot open source file!\n");
    }

    Halt();
    /* not reached */
}