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

    CPSID I

	; Load the address of RunningPtr
    LDR     R0, RunningPtr

	; Load the address of the thread control block of the currently running pointer
    LDR     R1, [R0]

	; Load the first thread's stack pointer
    LDR     SP, [R1]

    ; Restore the context of the first thread
    POP     {R4-R11}                        ; Load R4-R11 from the thread's stack
    POP     {R0-R3}                         ; Load R0-R3 from the thread's stack
    POP     {R12}                           ; Load R12 from the thread's stack

	; Load LR with the first thread's PC
    POP     {LR}                            ; Pop past LR
    POP     {LR}                            ; Pop PC into LR

    ;MOV     R0,SP
    ;ADD     R0, R0, #4
    ;MOV     SP,R0

	CPSIE I

    BX      LR				                ; Branches to the first thread

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
    CPSID   I

    ; Step 1: Save the remaining registers (R4-R11) of the current thread
    PUSH    {R4-R11}                        ; Store R4-R11 onto the current thread's stack

    ; Step 2: Save the current stack pointer to the current thread's TCB
    LDR     R1, RunningPtr                  ; Load the address of the currently running thread
    LDR     R2, [R1]                        ; Get the current thread's TCB
    STR     SP, [R2]                        ; Save the current stack pointer (SP) into the TCB

    ; Step 3: Call the scheduler to select the next thread to run
    PUSH    {LR}                            ; Save LR before calling the scheduler
    BL      G8RTOS_Scheduler                ; Call the scheduler
    POP     {LR}                            ; Restore LR after scheduler call

    ; Step 4: Load the stack pointer of the new thread from the new TCB
    LDR     R1, RunningPtr                  ; Load the updated currently running thread (next thread)
    LDR     R2, [R1]                        ; Get the new thread's TCB
    LDR     SP, [R2]                        ; Load the new thread's stack pointer (SP)

    ; Step 5: Restore the saved registers (R4-R11) from the new thread's stack
    POP     {R4-R11}                        ; Restore R4-R11 for the new thread

    ; Enable interrupts
    CPSIE   I

    ; Return from the exception
    BX      LR                              ; Return from PendSV_Handler

	.endasmfunc

	; end of the asm file
	.align
	.end
