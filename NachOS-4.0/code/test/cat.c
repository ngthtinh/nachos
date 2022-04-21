// Display File Content Program

#include "syscall.h"

#define MAX_LENGTH 255

int main()
{
    // Declaire
    int i, size, openFileId;
    char c, filename[MAX_LENGTH];

    // Read filename
    PrintString("Enter file name to display: ");
    ReadString(filename, MAX_LENGTH);

    // Open file
    openFileId = Open(filename);

    // Display content
    if (openFileId >= 0)
    {
        PrintString("File content:\n");

        // Get file size
        size = Seek(-1, openFileId);
        Seek(0, openFileId);

        // Print each byte to console
        for (i = 0; i < size; i++)
        {
            Read(&c, 1, openFileId);
            PrintChar(c);
        }

        PrintString("\n");

        // Close file
        Close(openFileId);
    }
    else
    {
        PrintString("Cannot open file!\n");
    }

    Halt();
    /* not reached */
}