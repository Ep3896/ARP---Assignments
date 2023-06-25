#include "./../include/processB_utilities.h"
#include "./../include/utility_modules.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Shared memory*/
int shm_fd;

/*Semaphore*/
sem_t *semaphore_one;
sem_t *semaphore_two;

float center[2];
time_t last_execution_time;

struct msg
{
    int buffer[1600][600];
};


void get_center(void *pointer, float *center)
{
    int count = 0;
    int sum_i = 0;
    int sum_j = 0;

    int i_values[WIDTH * HEIGHT]; // Store the i values
    int j_values[WIDTH * HEIGHT]; // Store the j values

    int index = 0;
    int i, j;

    while (index < WIDTH * HEIGHT)
    {
        i = index / HEIGHT;
        j = index % HEIGHT;

        if (((struct msg *)pointer)->buffer[i][j] == 1)
        {
            i_values[count] = i; // Store the i value
            j_values[count] = j; // Store the j value
            count++;
            sum_i += i;
            sum_j += j;
            ((struct msg *)pointer)->buffer[i][j] = 0;
        }

        index++;
    }

    if (count > 0)
    {
        int temp;
        int mid = count / 2;

        // Sort the i values and j values using bubble sort
        for (int k = 0; k < count - 1; k++)
        {
            if (i_values[k] > i_values[k + 1])
            {
                // Swap i values
                temp = i_values[k];
                i_values[k] = i_values[k + 1];
                i_values[k + 1] = temp;

                // Swap j values
                temp = j_values[k];
                j_values[k] = j_values[k + 1];
                j_values[k + 1] = temp;

                // Restart the loop to ensure complete sorting
                k = -1;
            }
        }

        float sum_i_f = (float)sum_i;
        float sum_j_f = (float)sum_j;

        center[0] = sum_i_f / count;
        center[1] = sum_j_f / count;
    }
}



int main(int argc, char const *argv[])
{
    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;

    // Initialize UI
    init_console_ui();

    // Open semaphore
    semaphore_one = sem_open(sem_path_1, 0);
    semaphore_two = sem_open(sem_path_2, 0);

    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);

    ftruncate(shm_fd, sizeof(struct msg));

    void *pointer = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    int old_center[2] = {center[0], center[1]};

    for (int i = 0; i < SM_WIDTH * SM_HEIGHT; ++i)
    {
        int x = i / SM_HEIGHT;
        int y = i % SM_HEIGHT;
        ((struct msg *)pointer)->buffer[x][y] = 0;
    }

    // Infinite loop
    while (TRUE)
    {

        sem_wait(semaphore_two);

        float scaling_factor_x = SM_WIDTH / COLS;
        float scaling_factor_y = SM_HEIGHT / LINES;

        time_t current_time = time(NULL);
        double time_diff = difftime(current_time, last_execution_time);

       if (time_diff >= 0.0001)
        {
            get_center(pointer, center);
            last_execution_time = current_time; 
        }

        mvaddch((center[1] / scaling_factor_x), (center[0] / scaling_factor_y), '0');
        refresh();
 
        sem_post(semaphore_one);

        int cmd = getch();

        if (cmd == KEY_RESIZE)
        {
            if (first_resize)
            {
                first_resize = FALSE;
                reset_console_ui();
            }
            else
            {
                reset_console_ui();
            }
        }
    }

    endwin();
    return 0;
}
