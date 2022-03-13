// Sort Program
// Sort array (within 100 elements) by Bubble Sort Algorithm

#include "syscall.h"

int main()
{
    // Declaire
    int arr[100];
    int n, i, j, temp;

    // Input n
    do
    {
        PrintString("n = ");
        n = ReadNum();
        if (n < 1 || n > 100)
            PrintString("Invalid number! Please try again!\n");
    } while (n < 1 || n > 100);
    
    // Input array
    for (i = 0; i < n; i++)
    {
        PrintString("arr[");
        PrintNum(i);
        PrintString("] = ");
        arr[i] = ReadNum();
    }

    // Bubble sort
    for (i = 0; i < n - 1; i++)
        for (j = i + 1; j < n; j++)
            if (arr[i] > arr[j])
            {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
    
    // Output result
    PrintString("Your array after sorting: ");

    for (i = 0; i < n; i++)
    {
        PrintNum(arr[i]);
        PrintChar(' ');
    }

    PrintChar('\n');

    Halt();
    /* not reached */
}