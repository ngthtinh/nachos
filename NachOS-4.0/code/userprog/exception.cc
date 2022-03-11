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
			character = kernel->synchConsoleIn->GetChar();

			DEBUG(dbgSys, "Read Char Done! Returning with: \"" << character << "\".\n");

			// Prepare result
			kernel->machine->WriteRegister(2, character);

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


			// Prepare result


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
			

			// Prepare result


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
			

			// Prepare result


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
			

			// Prepare result


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
