// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "synchconsole.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------

// Increase Program Counter
void IncreasePC()
{
	/* Set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	/* Set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	/* Set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
}

/*
 * User Space to System Space
 * Input : int virtAddr - User space address; int limit - Limit of buffer
 * Output: char* - Buffer
 * Purpose: Copy buffer from User memory space to System memory space
 */
#define MAX_BUFFER_SIZE 255

char* User2System(int virtAddr, int limit)
{
	int i; // index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; // Need for terminal string
	
	if (kernelBuf == NULL)
		return kernelBuf;

	memset(kernelBuf, 0, limit + 1);
	
	for (i = 0; i < limit; i++)
	{
		kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char) oneChar;
		if (oneChar == 0)
			break;
	}

	return kernelBuf;
}

/*
 * System space to User space
 * Input : int virtArr - User space address; int len - Limit of buffer; char* buffer - Buffer
 * Output: int - Number of bytes copied
 * Purpose: Copy buffer from System memory space to User memory space
 */
int System2User(int virtAddr, int len, char* buffer)
{
	if (len < 0) return -1;
	if (len == 0) return len;

	int i = 0;
	int oneChar = 0 ;
	
	do
	{
		oneChar = (int) buffer[i];
		kernel->machine->WriteMem(virtAddr+i,1,oneChar);
		i++;
	}
	while(i < len && oneChar != 0);

	return i;
}

// Exception Handler
void ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which)
	{
	// Everything ok!
	case NoException:
		return;

	// No valid translation found.
	case PageFaultException:
		DEBUG(dbgSys, "No valid translation found.\n");
		printf("No valid translation found.\n");
		SysHalt();
		ASSERTNOTREACHED();
		break;

	// Write attempted to page marked "read-only".
	case ReadOnlyException:
		DEBUG(dbgSys, "Write attempted to page marked \"read-only\".\n");
		printf("Write attempted to page marked \"read-only\".\n");
		SysHalt();
		ASSERTNOTREACHED();
		break;

	// Translation resulted in an invalid physical address.
	case BusErrorException:
		DEBUG(dbgSys, "Translation resulted in an invalid physical address.\n");
		printf("Translation resulted in an invalid physical address.\n");
		SysHalt();
		ASSERTNOTREACHED();
		break;

	// Unaligned reference or one that was beyond the end of the address space.
	case AddressErrorException:
		DEBUG(dbgSys, "Unaligned reference or one that was beyond the end of the address space.\n");
		printf("Unaligned reference or one that was beyond the end of the address space.\n");
		SysHalt();
		ASSERTNOTREACHED();
		break;

	// Integer overflow in add or sub.
	case OverflowException:
		DEBUG(dbgSys, "Integer overflow in add or sub.\n");
		printf("Integer overflow in add or sub.\n");
		SysHalt();
		ASSERTNOTREACHED();
		break;

	// Unimplemented or reserved instr.
	case IllegalInstrException:
		DEBUG(dbgSys, "Unimplemented or reserved instr.\n");
		printf("Unimplemented or reserved instr.\n");
		SysHalt();
		ASSERTNOTREACHED();
		break;

	// Number of Exception Types.
	case NumExceptionTypes:
		DEBUG(dbgSys, "Number of Exception Types: " << NumExceptionTypes << ".\n");
		printf("Number of Exception Types: %d.\n", NumExceptionTypes);
		SysHalt();
		ASSERTNOTREACHED();
		break;

	// A program executed a system call.
    case SyscallException:
      	switch (type)
		{
		// Default syscall: Halt
      	case SC_Halt:
		{
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
			SysHalt();
			ASSERTNOTREACHED();
			break;
		}

		// Default syscall: Add
    	case SC_Add:
		{
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	
			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");

			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);
	
			// Increase Program Counter for the next instructor
			IncreasePC();

			// Finish up
			return;
			ASSERTNOTREACHED();
			break;
		}

		// System call: ReadNum
    	case SC_ReadNum:
		{
			DEBUG(dbgSys, "System call: ReadNum.\n");
			
			// Process system call
			char* buffer = (char*) malloc(MAX_BUFFER_SIZE); // Prepare a buffer for reading
			memset(buffer, 0, MAX_BUFFER_SIZE);

			int index = 0; // Index
			char character; // Character read from console
			do
			{
				character = kernel->synchConsoleIn->GetChar(); // Get a character by SynchConsoleIn
				if (character != '\n') // If it is not Enter key
					buffer[index++] = character; // Save it to buffer
			} while (character != '\n' && index < MAX_BUFFER_SIZE); // Stop reading if Enter key is pressed
	
			int result = 0; // ReadNum result
			
			if (index <= 10) // It's store-able number
			{
				int start_index = 0;  // Where to start calculating the number. Default is 0.
				if (buffer[0] == '-') // If it's negative
					start_index = 1;  // Start from 1
				
				for (int i = start_index; i < index; i++)
				{
					if ('0' <= buffer[i] && buffer[i] <= '9') // Valid number
						result = result * 10 + buffer[i] - 48;
					else
					{
						result = 0; // Not a valid number, result must be 0
						break;
					}
				}

				if (buffer[0] == '-') // If it's negative number
					result = -result;
			}

			free(buffer); // Free the buffer

			DEBUG(dbgSys, "Read Number Done! Returning with: \"" << result << "\".\n");

			// Prepare result
			kernel->machine->WriteRegister(2, result); // Save result to register $2

			// Increase Program Counter for the next instructor
			IncreasePC();

			// Finish up
			return;
			ASSERTNOTREACHED();
			break;
		}

		// System call: PrintNum
    	case SC_PrintNum:
		{
			DEBUG(dbgSys, "System call: PrintNum.\n");
	
			// Process system call
			int number = kernel->machine->ReadRegister(4); // Load integer to print from register $4

			if (number == 0) // If number = 0
				kernel->synchConsoleOut->PutChar('0'); // Print 0 by SynchConsoleOut
			else
			{
				if (number < 0) // If number < 0
				{
					kernel->synchConsoleOut->PutChar('-'); // Print - by SynchConsoleOut
					number = -number; // Make number positive
				}

				int exp = 1;
				while (exp * 10 < number) exp *= 10;

				// Print each digit of number from left to right
				while (exp != 0)
				{
					kernel->synchConsoleOut->PutChar(number / exp % 10 + 48);
					exp /= 10;
				}
			}

			DEBUG(dbgSys, "Print Number Done!\n");

			// Increase Program Counter for the next instructor
			IncreasePC();

			// Finish up
			return;
			ASSERTNOTREACHED();
			break;
		}

		// System call: ReadChar
    	case SC_ReadChar:
		{
			DEBUG(dbgSys, "System call: ReadChar.\n");
	
			// Process system call
			char character = kernel->synchConsoleIn->GetChar(); // Get char from SynchConsoleIn

			DEBUG(dbgSys, "Read Char Done! Returning with: \"" << character << "\".\n");

			// Prepare result
			kernel->machine->WriteRegister(2, character); // Save result to register $2

			// Increase Program Counter for the next instructor
			IncreasePC();

			// Finish up
			return;
			ASSERTNOTREACHED();
			break;
		}

		// System call: PrintChar
    	case SC_PrintChar:
		{
			DEBUG(dbgSys, "System call: PrintChar.\n");
	
			// Process system call
			char character = kernel->machine->ReadRegister(4); // Load character to print from register $4

			kernel->synchConsoleOut->PutChar(character); // Print character to console by SynchConsoleOut
			
			DEBUG(dbgSys, "Print Char Done!\n");

			// Increase Program Counter for the next instructor
			IncreasePC();

			// Finish up
			return;
			ASSERTNOTREACHED();
			break;
		}

		// System call: RandomNum
    	case SC_RandomNum:
		{
			DEBUG(dbgSys, "System call: RandomNum.\n");
	
			// Process system call
			srand(time(NULL));

			int result = rand(); // Get a random integer

			DEBUG(dbgSys, "Random Number Done! Returning with: \"" << result << "\".\n");

			// Prepare result
			kernel->machine->WriteRegister(2, result); // Save result to register $2

			// Increase Program Counter for the next instructor
			IncreasePC();

			// Finish up
			return;
			ASSERTNOTREACHED();
			break;
		}

		// System call: ReadString
    	case SC_ReadString:
		{
			DEBUG(dbgSys, "System call: ReadString.\n");
	
			// Process system call
			int virtAddr = kernel->machine->ReadRegister(4); // Read address of string

			int length = kernel->machine->ReadRegister(5); // Get the maximum length of string

			char* buffer = (char*) malloc(length); // Prepare a buffer for reading
			memset(buffer, 0, length);

			int index = 0; // Index
			char character; // Character read from console
			do
			{
				character = kernel->synchConsoleIn->GetChar(); // Get a character by SynchConsoleIn
				if (character != '\n') // If it is not Enter key
					buffer[index++] = character; // Save it to buffer
			} while (character != '\n' && index < length); // Stop reading if Enter key is pressed

			System2User(virtAddr, length, buffer); // Copy string from System space to User space

			free(buffer); // Free the buffer

			DEBUG(dbgSys, "Read String Done!\n");

			// Increase Program Counter for the next instructor
			IncreasePC();

			// Finish up
			return;
			ASSERTNOTREACHED();
			break;
		}

		// System call: PrintString
    	case SC_PrintString:
		{
			DEBUG(dbgSys, "System call: PrintString.\n");
	
			// Process system call
			int virtAddr = kernel->machine->ReadRegister(4); // Read address of string

			char* buffer = User2System(virtAddr, MAX_BUFFER_SIZE); // Copy string from User space to System space

			int index = 0; // Index of the character to print from string
			while (buffer[index] != 0) // Print character if it is not '\0'
				kernel->synchConsoleOut->PutChar(buffer[index++]); // Print character to console by SynchConsoleOut

			free(buffer); // Free the buffer

			DEBUG(dbgSys, "Print String Done!\n");

			// Increase Program Counter for the next instructor
			IncreasePC();

			// Finish up
			return;
			ASSERTNOTREACHED();
			break;
		}

		// System call: Create
    	case SC_Create:
		{
			DEBUG(dbgSys, "System call: Create.\n");
	
			// Process system call
			int virtAddr = kernel->machine->ReadRegister(4); // Read address of filename

			char* filename = User2System(virtAddr, MAX_BUFFER_SIZE); // Copy filename from User space to Kernel space

			if (filename == NULL) // User2System failed! (Reason: User2System cannot allocate filename buffer)
			{
				// Announce Error
				DEBUG(dbgSys, "Create Failed: Cannot allocate filename buffer.\n");

				// Return -1 to Create syscall for failure (Write -1 to register $2)
				kernel->machine->WriteRegister(2, -1);

				// Finish up
				IncreasePC();

				return;
				ASSERTNOTREACHED();
				break;
			}
			
			if (strlen(filename) == 0)  // Invalid filename!
			{
				// Announce Error
				DEBUG(dbgSys, "Create Failed: Invalid filename.\n");

				// Return -1 to Create syscall for failure (Write -1 to register $2)
				kernel->machine->WriteRegister(2, -1);

				// Finish up
				IncreasePC();

				return;
				ASSERTNOTREACHED();
				break;
			}

			// Create file using fileSystem object
			if (kernel->fileSystem->Create(filename, 0) == FALSE) // Create file failed!
			{
				// Announce Error
				DEBUG(dbgSys, "Create Failed!\n");

				// Return -1 to Create syscall for failure (Write -1 to register $2)
				kernel->machine->WriteRegister(2, -1);

				// Finish up
				delete[] filename;

				IncreasePC();

				return;
				ASSERTNOTREACHED();
				break;
			}

			// Create file successfully, return 0 to Create syscall for success (Write 0 to register $2)
			kernel->machine->WriteRegister(2, 0);

			delete[] filename; // Delete filename buffer

			DEBUG(dbgSys, "Create Done!\n");

			// Increase Program Counter for the next instructor
			IncreasePC();

			// Finish up
			return;
			ASSERTNOTREACHED();
			break;
		}

		// System call: Open
		// Input: Filename
		// Output: OpenFileID
    	case SC_Open:
		{
			DEBUG(dbgSys, "System call: Open.\n");
	
			// Process system call
			int virtAddr = kernel->machine->ReadRegister(4); // Read address of filename

			char* filename = User2System(virtAddr, MAX_BUFFER_SIZE); // Copy filename from User space to Kernel space

			int freeSlot = kernel->fileSystem->FindFreeSlot();
			
			if (freeSlot == -1) // Enough opened file
			{
				// Announce Error
				DEBUG(dbgSys, "Open Failed: Enough opened file!\n");

				// Return -1 to Open syscall for failure (Write -1 to register $2)
				kernel->machine->WriteRegister(2, -1);

				// Finish up
				delete[] filename;

				IncreasePC();

				return;
				ASSERTNOTREACHED();
				break;
			}

			// Open file
			kernel->fileSystem->openFile[freeSlot] = kernel->fileSystem->Open(filename, 0);
			kernel->machine->WriteRegister(2, freeSlot); // Return OpenFileID (freeSlot)

			delete[] filename;

			DEBUG(dbgSys, "Open Done!\n");

			// Increase Program Counter for the next instructor
			IncreasePC();

			// Finish up
			return;
			ASSERTNOTREACHED();
			break;
		}
	
		// System call: Close
		// Input: OpenFileId - Open File ID
		// Output: int - 0 for succes, -1 for failure
    	case SC_Close:
		{
			DEBUG(dbgSys, "System call: Close.\n");
	
			// Process system call
			OpenFileId openFileId = kernel->machine->ReadRegister(4); // Get OpenFileID to close

			if (openFileId < 0 || openFileId >= MAX_FILES || kernel->fileSystem->openFile[openFileId] == NULL)
			{
				// Announce Error
				DEBUG(dbgSys, "Close Failed!\n");

				// Return -1 to Close syscall for failure (Write -1 to register $2)
				kernel->machine->WriteRegister(2, -1);

				// Finish up
				IncreasePC();

				return;
				ASSERTNOTREACHED();
				break;
			}

			delete kernel->fileSystem->openFile[openFileId];
			kernel->fileSystem->openFile[openFileId] = NULL;

			kernel->machine->WriteRegister(2, 0); // Return 0 for success

			DEBUG(dbgSys, "Close Done!\n");

			// Increase Program Counter for the next instructor
			IncreasePC();

			// Finish up
			return;
			ASSERTNOTREACHED();
			break;
		}

		// System call: Read
    	case SC_Read:
		{
			DEBUG(dbgSys, "System call: Read.\n");
	
			// Process system call
			// Code here

			// Prepare result (if necessary)
			// Code here

			DEBUG(dbgSys, "Read Done!\n");

			// Increase Program Counter for the next instructor
			IncreasePC();

			// Finish up
			return;
			ASSERTNOTREACHED();
			break;
		}

		// System call: Write
    	case SC_Write:
		{
			DEBUG(dbgSys, "System call: Write.\n");
	
			// Process system call
			// Code here

			// Prepare result (if necessary)
			// Code here

			DEBUG(dbgSys, "Write Done!\n");

			// Increase Program Counter for the next instructor
			IncreasePC();

			// Finish up
			return;
			ASSERTNOTREACHED();
			break;
		}

		// System call: Seek
    	case SC_Seek:
		{
			DEBUG(dbgSys, "System call: Seek.\n");
	
			// Process system call
			// Code here

			// Prepare result (if necessary)
			// Code here

			DEBUG(dbgSys, "Seek Done!\n");

			// Increase Program Counter for the next instructor
			IncreasePC();

			// Finish up
			return;
			ASSERTNOTREACHED();
			break;
		}

		// System call: Remove
    	case SC_Remove:
		{
			DEBUG(dbgSys, "System call: Remove.\n");
	
			// Process system call
			// Code here

			// Prepare result (if necessary)
			// Code here

			DEBUG(dbgSys, "Remove Done!\n");

			// Increase Program Counter for the next instructor
			IncreasePC();

			// Finish up
			return;
			ASSERTNOTREACHED();
			break;
		}

		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
    	}
      
		break;

	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
    
	ASSERTNOTREACHED();
}
