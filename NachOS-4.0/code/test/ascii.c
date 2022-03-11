// Ascii Program
// Print ascii code

#include "syscall.h"

int main()
{
    int character;

    PrintString("Ascii Program (Visible ascii character)\n");

    for (character = 33; character < 127; character++)
    {
        PrintNum(character);
        PrintString(" : ");
        PrintChar((char) character);
        PrintChar('\n');
    }

    Halt();
    /* not reached */
}