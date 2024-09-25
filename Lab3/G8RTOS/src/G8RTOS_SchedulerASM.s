; G8RTOS_SchedulerASM.s
; Created: 2022-07-26
; Updated: 2022-07-26
; Contains assembly functions for scheduler.

	; Functions Defined
	.def G8RTOS_Start, PendSV_Handler

	; Dependencies
	.ref CurrentlyRunningThread, G8RTOS_Scheduler

	.thumb		; Set to thumb mode
	.align 2	; Align by 2 bytes (thumb mode uses allignment by 2 or 4)
	.text		; Text section

; Need to have the address defined in file
; (label needs to be close enough to asm code to be reached with PC relative addressing)
RunningPtr: .field CurrentlyRunningThread, 32

; G8RTOS_Start
;	Sets the first thread to be the currently running thread
;	Starts the currently running thread by setting Link Register to tcb's Program Counter
G8RTOS_Start:

	.asmfunc

    ; Disable interrupts to prevent context switching during this process
    CPSID I

	; Load the address of RunningPtr
    LDR R0, RunningPtr

	; Load the address of the thread control block of the currently running pointer
    LDR R1, [R0]

	; Load the first thread's stack pointer
    LDR SP, [R1]

    ; Restore the context of the first thread
    POP {R4-R11}
    POP {R0-R3}
    POP {R12}

	; Load LR with the first thread's PC
    POP {LR}        ; Pop past LR
    POP {LR}        ; Pop PC into LR

    ; Enable interrupts
	CPSIE I

    BX LR		    ; Branches to the first thread

	.endasmfunc

; PendSV_Handler
; - Performs a context switch in G8RTOS
; 	- Saves remaining registers into thread stack
;	- Saves current stack pointer to tcb
;	- Calls G8RTOS_Scheduler to get new tcb
;	- Set stack pointer to new stack pointer from new tcb
;	- Pops registers from thread stack
PendSV_Handler:

	.asmfunc

    ; Disable interrupts to prevent context switching during this process
    CPSID I

    ; Step 1: Save the callee-saved registers (R4-R11) of the current thread onto the stack
    PUSH {R4-R11}

    ; Step 2: Save the current stack pointer to the current thread's TCB
    LDR R0, RunningPtr      ; Load the address of the currently running thread
    LDR R1, [R0]            ; Get the current thread's TCB
    STR SP, [R1]            ; Save the current stack pointer into the TCB

    ; Step 3: Call the scheduler to select the next thread to run
    PUSH{LR}                ; Save LR before calling the scheduler
    BL  G8RTOS_Scheduler    ; Call the scheduler
    POP {LR}                ; Restore LR after scheduler call

    ; Step 4: Load the stack pointer of the new thread from the new TCB
    LDR R0, RunningPtr      ; Load the updated currently running thread (next thread)
    LDR R1, [R0]            ; Get the new thread's TCB
    LDR SP, [R1]            ; Load the new thread's stack pointer

    ; Step 5: Restore the saved registers (R4-R11) from the new thread's stack
    POP {R4-R11}            ; Restore R4-R11 for the new thread

    ; Enable interrupts
    CPSIE I

    BX LR               ; Return from PendSV_Handler

	.endasmfunc

	; end of the asm file
	.align
	.end
