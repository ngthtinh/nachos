// Help Program
// Print our information, and about ascii and sort program

#include "syscall.h"

int main()
{
    char character;
    character = ReadChar();
    PrintChar(character);

    Halt();
    /* not reached */
}