// threads.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Threads

#ifndef THREADS_H_
#define THREADS_H_

/************************************Includes***************************************/

#include "./G8RTOS/G8RTOS.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/

#define JOYSTICK_FIFO       0
#define FREQ1_FIFO          1
#define FREQ2_FIFO          2
#define DISPLAY_FIFO        3
#define OUTPUT_FIFO         4

/*************************************Defines***************************************/

/***********************************Semaphores**************************************/

semaphore_t sem_I2CA;
semaphore_t sem_SPIA;
semaphore_t sem_PCA9555_Debounce;
semaphore_t sem_Joystick_Debounce;

/***********************************Semaphores**************************************/

/***********************************Structures**************************************/
/***********************************Structures**************************************/

/*******************************Background Threads**********************************/

void Idle_Thread(void);
void Mic_Thread(void);
void Speaker_Thread(void);
void Volume_Thread(void);
void Display_Thread(void);

/*******************************Background Threads**********************************/

/********************************Periodic Threads***********************************/

void Update_Volume(void);

/********************************Periodic Threads***********************************/

/*******************************Aperiodic Threads***********************************/

void Mic_Handler(void);
void Button_Handler(void);
void DAC_Timer_Handler(void);

/*******************************Aperiodic Threads***********************************/

#endif /* THREADS_H_ */
