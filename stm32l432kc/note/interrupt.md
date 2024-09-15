# Exceptions and Interrupts In ARM

## Context

- Let say CPU is supposed to handle multiple tasks.
- One task is to process a large array of data units (e.g, sort the elements in 
the array).
- In addition, some data may receive via a serial port. The CPU does not know 
***the exact time***, and the CPU needs to respond to this event ***immediately***.
- Then, how this program should structure in this situation?

### Solution 1: Polling

- Put the serial port handling process in a subroutine.
- Put the sorting algorithm in the main program.
- After each instruction in the main program, check the serial port.

```
Loop sorting
    check serial port: if data arrive, "Branch serial_handling_rountine"
    BAL Loop
```

### Solution 2: Exception and Interrupts

- Put the sorting algorithm in the main program.
- Put the serial port handling process in another routine.
- When the CPU is executing an instruction of the sorting algorithm in the main 
program, if data receive at the serial port, then the serial port sends a 
*notification* to the CPU, and an ***exception*** happens.
- Upon the exception, the CPU ***pauses***  the sorting algorithm, and is 
***diverted*** to execute the `serial_handling_routine`.
- After the handling routine is completed, the CPU ***returns*** to the main 
program, to ***continues*** the sorting algorithm.

## CPU Processing States

- In **normal** processing, the next instruction executed is indicated by the *PC*.
- In addition to the normal operational state, there are:
    - ***Exception state*** - Processing of exceptions
    - ***Halted state*** - To prevent unpredictable behavior when a serious 
    system failure occurs

## Exceptions

- Exception is an event that **alters** the normal program execution.
- A deviation from normal processing which can be triggered externally by
    - system error
    - request from I/O device
    - or internally by execution of a trap instruction
- Exceptions provide a way of **diverting the CPU** from the normal processing 
state to *handle special situations*.

## Modes of Operation in ARM

- Why talking about modes of operation in ARM? For each ARM exception, it will 
(run?) in different modes of operation in ARM.
- Here's a following list of exceptions (Exception - Mode)
    - Fast Interrupt Request - FIQ
    - Interrupt Request - IRQ
    - SWI and REST - SVC
    - Instruction fetch or memory access failure - ABT
    - Undefined Instruction - UND
- The ARM has seven operating modes.
    - User (**usr**), or normal program execution mode,
    - FIQ (**fiq**), or fast data processing mode,
    - IRQ (**irq**), a mode for general purpose interrupts,
    - Supervisor (**svc**), a protected mode for the operating system,
    - Abort (**abt**), a mode for data or instruction fetch is aborted,
    - Undefined (**und**), a mode for undefined instructions,
    - System (**sys**), priviledge mode for OS tasks.
- Switching above operating modes requires saving / loading register values.

### Program Status Register (CPSR and SPSR)

- A 32 bit register.
- Bit 31 to bit 28: Condition code flags
    - N = Negative result from ALU flag
    - Z = Zero result from ALU flag
    - C = ALU operation Carried out
    - V = ALU operation Overflow
- Bit 8 to bit 4 - Interrupt Disable bits
    - I = 1, disables the IRQ
    - F = 1, disables the FIQ
    - T bits, (Architecture v4T only)
        - 0, CPU in ARM state
        - 1, CPU in Thumb state
- Mode bits (bit 4 to bit 0) defines the process mode.


### Mode Bits

- User (USR) 10000 - Unprivileged mode in which most applications run
- FIQ 10001 - Entered on an FIQ interrupt exception
- IRQ 10010 - Entered on an IRQ interrupt exception
- Supervisor 10011 - Entered on reset or when a *supervisor call instruction* is 
executed
- Abort (ABT) 10111 - Entered on a memory access exception
- Undef (UND) 11011 - Entered when an undefined instruction executed
- System (SYS) 111111 - Privileged mode, sharing register view with User mode

### Register Organization in different modes

![Register Organization Summary](./) 

## Exception Flow

- One might ask, upon exception, how is the CPU diverted to the exception 
handling routine?

### Typical Jump

- Typically, when calling a subroutine, we **exactly know** when and where to jump
since jump is initiated explicitly with `BL [name_of_subroutine]`.
- `BL` will save returning address on the link register, and jump to the location 
with the label `[name_of_subroutine]`.
- When the subroutine is finished, `MOV PC, LR` will pop the returning address 
from the link register to PC, and thus the CPU **diverts back** back to the main 
program.

### Exception Jump

- An exception can happen anytime. Explicitly using an instruction for a typical '
jump is not reliable and feasible.
- There's a different mechanism to let CPU find where to jump, i.e., find where 
the *exception* handling routine is located.

## Exception Vector and Exception Vector Table

- An ***exception vector*** contains the **address** of an exception handling 
routine.
- ST MCU store 256 exception vectors (256 long word addresses) together in an 
***exception vector table***.
- The table occupies the first *1024* (256 x 4) bytes in the memory starting 
at address pointed to by the contents of the ***"vector register (VR)"*** 
- VR holds the base address of exception vector in memory. The **displacement** of an 
exception vector is added to the value in this register to access the vector table.
- The RESET exception vector has two vectors (two addresses):
    - The value of the ***initial*** stack pointer.
    - The address of the ***RESET*** handling routine.
```
AREA RESET,DATA,READONLY
STACK DCD 0X20001000 ;stack pointer value
RESET_HANDLER DCD 0x08000010 ;reset vector
```
- On system reset, the vector table is fixed at address 0x00000000. 
- Privileged software can write to the vector table start address to a different 
memory location.

## Exception Priority

- Since exceptions can occur simultaneously so more than one exception can arise 
at the same the time.
- The processor has to have a priority for each exception so it can decide which of 
the currently raised exceptions is more important.
```
Exception             |Priority|I bit and F bit
Reset                 |1       |11
Data Abort            |2       |1-
FIQ                   |3       |11
IRQ                   |4       |1-
Pre-fetch abort       |5       |1-
SWI                   |6       |1-
Undefined instruction |6       |1-

1 is highest, 6 is lowest
```

## Exception Processing

1. Recognition of exception
2. Preserve the address of the next instruction
3. Copy CPSR to SPSR
4. Force the CPSR mode bits to an exception value
5. Force PC to fetch the next instruction from the exception vector table
6. Execute the handler
7. Restore SPSR to CPSR
8. Return to normal operations


