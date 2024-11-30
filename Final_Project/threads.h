// threads.h
// Date Created: 2023-07-26
// Date Updated: 2024-11-30
// Threads

#ifndef THREADS_H_
#define THREADS_H_

/************************************Includes***************************************/

#include "./G8RTOS/G8RTOS.h"

/************************************Includes***************************************/

/********************************Public Functions***********************************/

void InitializeGridDisplay();
void PlayNoteAtRow(uint8_t row);

/********************************Public Functions***********************************/

/*************************************Defines***************************************/

#define JOYSTICK_FIFO 0

/*************************************Defines***************************************/

/***********************************Semaphores**************************************/

semaphore_t sem_SPIA;
semaphore_t sem_PCA9555_Debounce;
semaphore_t sem_Joystick_Debounce;
semaphore_t sem_Tiva_Button;

/***********************************Semaphores**************************************/

/***********************************Structures**************************************/
/***********************************Structures**************************************/

/*******************************Background Threads**********************************/

void Idle_Thread(void);
void Speaker_Thread(void);
void Volume_Thread(void);
void Display_Thread(void);
void JoystickPress_Thread(void);
void Navigation_Thread(void);
void NotePlacement_Thread(void);

/*******************************Background Threads**********************************/

/********************************Periodic Threads***********************************/

void Get_Joystick(void);

/********************************Periodic Threads***********************************/

/*******************************Aperiodic Threads***********************************/

void Button_Handler(void);
void Joystick_Button_Handler(void);
void TivaButton_Handler(void);
void DAC_Timer_Handler(void);

/*******************************Aperiodic Threads***********************************/

#endif /* THREADS_H_ */
