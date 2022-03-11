// Help Program
// Print our information, and about ascii and sort program

#include "syscall.h"

int main()
{
    PrintString("Introduction to Operating System                     \n");
    PrintString("20CLC01 - Ho Chi Minh City University of Science   \n\n");
    PrintString("Theory Instructor: Le Quoc Hoa                       \n");
    PrintString("Lab instructor: Chung Thuy Linh                    \n\n");
    PrintString("---------------------------------------------------\n\n");
    PrintString("Group Information:                                   \n");
    PrintString("19127292   Nguyen Thanh Tinh                         \n");
    PrintString("19127339   Pham Chi Bao                            \n\n");
    PrintString("---------------------------------------------------\n\n");
    PrintString("About programs:                                      \n");
    PrintString("- ascii : print ascii code to console                \n");
    PrintString("- sort  : sort array (n <= 100) by Bubble sort     \n\n");
    PrintString("---------------------------------------------------\n\n");

    Halt();
    /* not reached */
}