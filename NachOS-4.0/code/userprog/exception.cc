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


			// Prepare result


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
			

			// Prepare result


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
			char character;
			character = kernel->synchConsoleIn->GetChar(); // Get char from SynchConsoleIn

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
			char character;
			character = kernel->machine->ReadRegister(4); // Load character to print from register $4

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

			int result;
			result = rand(); // Get a random integer

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
			int virtAddr;
			virtAddr = kernel->machine->ReadRegister(4); // Read address of string

			int length;
			length = kernel->machine->ReadRegister(5); // Get the maximum length of string

			char* buffer = (char*) malloc(length); // Prepare a buffer for reading
			memset(buffer, 0, length);

			int index; // Index
			index = 0;

			char character; // Character read from console

			do
			{
				character = kernel->synchConsoleIn->GetChar(); // Get a character by SynchConsoleIn
				if (character != '\n') // If it is not Enter key
					buffer[index++] = character; // Save it to buffer
			} while (character != '\n' && index < length); // Stop reading if Enter key is pressed

			System2User(virtAddr, length, buffer); // Copy string from System space to User space

			delete buffer;

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
			int virtAddr;
			virtAddr = kernel->machine->ReadRegister(4); // Read address of string

			char* buffer;
			buffer = User2System(virtAddr, 255); // Copy string from User space to System space

			int index;
			index = 0; // Index of the character to print from string

			while (buffer[index] != 0) // Print character if it is not '\0'
				kernel->synchConsoleOut->PutChar(buffer[index++]); // Print character to console by SynchConsoleOut

			delete buffer;

			DEBUG(dbgSys, "Print String Done!\n");

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
