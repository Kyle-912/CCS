// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for thread functions.

/************************************Includes***************************************/

#include "./threads.h"

#include "./MultimodDrivers/multimod.h"
#include "./MiscFunctions/Shapes/inc/cube.h"
#include "./MiscFunctions/LinAlg/inc/linalg.h"
#include "./MiscFunctions/LinAlg/inc/quaternions.h"
#include "./MiscFunctions/LinAlg/inc/vect3d.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Change this to change the number of points that make up each line of a cube.
// Note that if you set this too high you will have a stack overflow!
#define Num_Interpolated_Points 10

// sizeof(float) * num_lines * (Num_Interpolated_Points + 2) = ?

#define MAX_NUM_CUBES (MAX_THREADS - 3)

/*********************************Global Variables**********************************/
Quat_t world_camera_pos = {0, 0, 0, 50};
Quat_t world_camera_frame_offset = {0, 0, 0, 50};
Quat_t world_camera_frame_rot_offset;
Quat_t world_view_rot = {1, 0, 0, 0};
Quat_t world_view_rot_inverse = {1, 0, 0, 0};

// How many cubes?
uint8_t num_cubes = 0;

// y-axis controls z or y
uint8_t joystick_y = 1;

// Kill a cube?
uint8_t kill_cube = 0;

/*********************************Global Variables**********************************/

/*************************************Threads***************************************/

void Idle_Thread(void)
{
    time_t t;
    srand((unsigned)time(&t));
    while (1)
    {
    }
}

void CamMove_Thread(void)
{
    // Initialize / declare any variables here
    int16_t x_raw, y_raw;
    float x_norm, y_norm;
    const float JOYSTICK_MAX = 32767.0;

    while (1)
    {
        // Get result from joystick
        x_val = JOYSTICK_GetX();
        y_val = JOYSTICK_GetY();

        // If joystick axis within deadzone, set to 0. Otherwise normalize it.
        if (x_val > DEADZONE || x_val < -DEADZONE)
        {
            world_camera_pos.x += x_val;
        }

        // Update world camera position. Update y/z coordinates depending on the joystick toggle.
        if (joystick_y)
        {
            if (y_val > DEADZONE || y_val < -DEADZONE)
            {
                world_camera_pos.z += y_val; // Control Z-axis
            }
        }
        else
        {
            if (y_val > DEADZONE || y_val < -DEADZONE)
            {
                world_camera_pos.y += y_val; // Control Y-axis
            }
        }

        // sleep
        sleep(10);
    }
}

void Cube_Thread(void)
{
    cube_t cube;

    /*************YOUR CODE HERE*************/
    // Get spawn coordinates from FIFO, set cube.x, cube.y, cube.z
    uint32_t coordinates = G8RTOS_ReadFIFO(SPAWNCOOR_FIFO);
    cube.x_pos = (coordinates >> 16) & 0xFFFF; // Extract x coordinate
    cube.y_pos = coordinates & 0xFFFF;         // Extract y coordinate
    cube.z_pos = 50;

    cube.width = 50;
    cube.height = 50;
    cube.length = 50;

    Quat_t v[8];
    Quat_t v_relative[8];

    Cube_Generate(v, &cube);

    uint32_t m = Num_Interpolated_Points + 1;
    Vect3D_t interpolated_points[12][Num_Interpolated_Points + 2];
    Vect3D_t projected_point;

    Quat_t camera_pos;
    Quat_t camera_frame_offset;
    Quat_t view_rot_inverse;

    uint8_t kill = 0;

    while (1)
    {
        /*************YOUR CODE HERE*************/
        // Check if kill ball flag is set.
        if (kill_cube)
        {
            G8RTOS_KillSelf();
        }

        camera_pos.x = world_camera_pos.x;
        camera_pos.y = world_camera_pos.y;
        camera_pos.z = world_camera_pos.z;

        camera_frame_offset.x = world_camera_frame_offset.x;
        camera_frame_offset.y = world_camera_frame_offset.y;
        camera_frame_offset.z = world_camera_frame_offset.z;

        view_rot_inverse.w = world_view_rot_inverse.w;
        view_rot_inverse.x = world_view_rot_inverse.x;
        view_rot_inverse.y = world_view_rot_inverse.y;
        view_rot_inverse.z = world_view_rot_inverse.z;

        // Clears cube from screen
        for (int i = 0; i < 12; i++)
        {
            for (int j = 0; j < m + 1; j++)
            {
                getViewOnScreen(&projected_point, &camera_frame_offset, &(interpolated_points[i][j]));
                /*************YOUR CODE HERE*************/
                // Wait on SPI bus
                G8RTOS_WaitSemaphore(&sem_SPIA);

                ST7789_DrawPixel(projected_point.x, projected_point.y, ST7789_BLACK);

                /*************YOUR CODE HERE*************/
                // Signal that SPI bus is available
                G8RTOS_SignalSemaphore(&sem_SPIA);
            }
        }

        /*************YOUR CODE HERE*************/
        // If ball marked for termination, kill the thread.

        // Calculates view relative to camera position / orientation
        for (int i = 0; i < 8; i++)
        {
            getViewRelative(&(v_relative[i]), &camera_pos, &(v[i]), &view_rot_inverse);
        }

        // Interpolates points between vertices
        interpolatePoints(interpolated_points[0], &v_relative[0], &v_relative[1], m);
        interpolatePoints(interpolated_points[1], &v_relative[1], &v_relative[2], m);
        interpolatePoints(interpolated_points[2], &v_relative[2], &v_relative[3], m);
        interpolatePoints(interpolated_points[3], &v_relative[3], &v_relative[0], m);
        interpolatePoints(interpolated_points[4], &v_relative[0], &v_relative[4], m);
        interpolatePoints(interpolated_points[5], &v_relative[1], &v_relative[5], m);
        interpolatePoints(interpolated_points[6], &v_relative[2], &v_relative[6], m);
        interpolatePoints(interpolated_points[7], &v_relative[3], &v_relative[7], m);
        interpolatePoints(interpolated_points[8], &v_relative[4], &v_relative[5], m);
        interpolatePoints(interpolated_points[9], &v_relative[5], &v_relative[6], m);
        interpolatePoints(interpolated_points[10], &v_relative[6], &v_relative[7], m);
        interpolatePoints(interpolated_points[11], &v_relative[7], &v_relative[4], m);

        for (int i = 0; i < 12; i++)
        {
            for (int j = 0; j < m + 1; j++)
            {
                getViewOnScreen(&projected_point, &camera_frame_offset, &(interpolated_points[i][j]));

                if (interpolated_points[i][j].z < 0)
                {
                    /*************YOUR CODE HERE*************/
                    // Wait on SPI bus
                    G8RTOS_WaitSemaphore(&sem_SPIA);

                    ST7789_DrawPixel(projected_point.x, projected_point.y, ST7789_BLUE);

                    /*************YOUR CODE HERE*************/
                    // Signal that SPI bus is available
                    G8RTOS_SignalSemaphore(&sem_SPIA);
                }
            }
        }

        /*************YOUR CODE HERE*************/
        // Sleep
        sleep(10);
    }
}

void Read_Buttons()
{
    // Initialize / declare any variables here
    uint8_t button_state;
    int16_t x, y, z;

    while (1)
    {
        // Wait for a signal to read the buttons on the Multimod board.
        G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);

        // Sleep to debounce
        sleep(10);

        // Read the buttons status on the Multimod board.
        button_state = MultimodButtons_Get();

        // Process the buttons and determine what actions need to be performed.
        if (button_state & SW1)
        {
            // Generate random coordinates for the cube
            x = (rand() % 201) - 100; // Random number between [-100, 100]
            y = (rand() % 201) - 100; // Random number between [-100, 100]
            z = (rand() % 101) - 120; // Random number between [-120, -20]

            // Send coordinates to SPAWNCOOR_FIFO
            uint32_t spawn_coords = ((uint32_t)x << 16) | ((uint32_t)y << 8) | (uint32_t)z;
            G8RTOS_WriteFIFO(SPAWNCOOR_FIFO, spawn_coords);
        }

        if (button_state & SW2) // SW2 Pressed
        {
            // Signal to terminate a random cube
            G8RTOS_SignalSemaphore(&sem_KillCube);
        }

        // Clear the interrupt
        GPIOIntClear(GPIO_PORTE_BASE, BUTTONS_INT_PIN);

        // Re-enable the interrupt so it can occur again.
        GPIOIntEnable(GPIO_PORTE_BASE, BUTTONS_INT_PIN);
    }
}

void Read_JoystickPress()
{
    // Initialize / declare any variables here

    while (1)
    {
        // Wait for a signal to read the joystick press
        G8RTOS_WaitSemaphore(&sem_Joystick_Debounce);

        // Sleep to debounce
        sleep(10);

        // Read the joystick switch status on the Multimod board.
        if (JOYSTICK_GetPress())
        {
            joystick_y = !joystick_y; // Toggle the joystick_y flag.
        }

        // Clear the interrupt
        GPIOIntClear(GPIO_PORTD_BASE, JOYSTICK_INT_PIN);

        // Re-enable the interrupt so it can occur again.
        GPIOIntEnable(GPIO_PORTD_BASE, JOYSTICK_INT_PIN);
    }
}

/********************************Periodic Threads***********************************/

void Print_WorldCoords(void)
{
    int32_t x = (int32_t)(world_camera_pos.x * 100);
    int32_t y = (int32_t)(world_camera_pos.y * 100);
    int32_t z = (int32_t)(world_camera_pos.z * 100);
    // Print the camera position through UART to display on console.
    G8RTOS_WaitSemaphore(&sem_I2CA);
    UARTprintf("Camera Pos: X=%d.%02d, Y=%d.%02d, Z=%d.%02d\n",
               x / 100, abs(x % 100),
               y / 100, abs(y % 100),
               z / 100, abs(z % 100));
    G8RTOS_SignalSemaphore(&sem_I2CA);
}

void Get_Joystick(void)
{
    // Read the joystick
    uint16_t x = JOYSTICK_GetX();
    uint16_t y = JOYSTICK_GetY();

    // Send through FIFO.
    G8RTOS_WriteFIFO(JOYSTICK_FIFO, (x << 16) | y);
}

/*******************************Aperiodic Threads***********************************/

void GPIOE_Handler()
{
    // Disable interrupt
    GPIOIntDisable(GPIO_PORTE_BASE, BUTTONS_INT_PIN);

    // Signal relevant semaphore
    G8RTOS_SignalSemaphore(&sem_PCA9555_Debounce);
}

void GPIOD_Handler()
{
    // Disable interrupt
    GPIOIntDisable(GPIO_PORTD_BASE, JOYSTICK_INT_PIN);

    // Signal relevant semaphore
    G8RTOS_SignalSemaphore(&sem_Joystick_Debounce);
}