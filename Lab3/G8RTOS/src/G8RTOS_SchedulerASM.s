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

	; Load the address of RunningPtr
	; Load the address of the thread control block of the currently running pointer
	; Load the first thread's stack pointer
	; Load LR with the first thread's PC

	; Load the stack pointer of the currently running thread
    LDR     R0, RunningPtr                  ; Load address of RunningPtr
    LDR     R1, [R0]                        ; Load the current thread's TCB
    LDR     R2, [R1]                        ; Load the stack pointer of the current thread (PSP)
    MSR     PSP, R2                         ; Set the Process Stack Pointer (PSP) to the current thread's stack

    ; Restore the context of the first thread (R4-R11)
    LDMIA   R2!, {R4-R11}                   ; Load R4-R11 from the thread's stack

    ; Return to thread mode using PSP
    ORR     LR, LR, #0x04                   ; Ensure the exception returns using PSP (Thread mode)
    BX      LR                              ; Branch back to the thread (switch context)

	BX LR				;Branches to the first thread

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
	; put your assembly code here!
    ; Step 1: Save the remaining registers (R4-R11) of the current thread
    MRS     R0, PSP                         ; Get current Process Stack Pointer (PSP)
    STMDB   R0!, {R4-R11}                   ; Store R4-R11 onto the current thread's stack

    ; Step 2: Save the current stack pointer to the current thread's TCB
    LDR     R1, RunningPtr                  ; Load the address of the currently running thread
    LDR     R2, [R1]                        ; Get the current thread's TCB
    STR     R0, [R2]                        ; Save the PSP (R0) into the TCB's stack pointer

    ; Step 3: Call the scheduler to get the new thread to run
    BL      G8RTOS_Scheduler                ; Call the scheduler to switch to the next thread

    ; Step 4: Load the stack pointer of the new thread from the new TCB
    LDR     R2, [R1]                        ; Load the new thread's TCB
    LDR     R0, [R2]                        ; Load the PSP (stack pointer) of the new thread

    ; Step 5: Restore the saved registers (R4-R11) from the new thread's stack
    LDMIA   R0!, {R4-R11}                   ; Load R4-R11 from the new thread's stack

    ; Update the PSP to point to the new stack pointer
    MSR     PSP, R0                         ; Update PSP to the new thread's stack pointer

    ; Return from the exception
    ORR     LR, LR, #0x04                   ; Ensure the exception returns to thread mode using PSP
    BX      LR                              ; Return from PendSV_Handler

	.endasmfunc

	; end of the asm file
	.align
	.end
