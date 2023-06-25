#include "./../include/processA_utilities.h"
#include "./../include/utility_modules.h"

bmpfile_t *bmp;

void *pointer;

rgb_pixel_t pixel = {255, 0, 0, 0}; 
rgb_pixel_t empty_pixel = {255, 255, 255, 0}; 

int shm_fd;
int print_counter = 0;

sem_t *semaphore_one;
sem_t *semaphore_two;

struct msg
{
    int buffer[1600][600];
};

bool take_snapshot()

{
    char path[20];
    snprintf(path, 20, "prints/%d.bmp", print_counter);
    print_counter += 1;
    bmp_save(bmp, path);
}

void print_circle(bmpfile_t *bmp, int xc, int yc, rgb_pixel_t color)

{
    int total_pixels = (2 * RADIUS + 1) * (2 * RADIUS + 1);
    for (int i = 0; i < total_pixels; i++)
    {
        int x = i % (2 * RADIUS + 1) - RADIUS;
        int y = i / (2 * RADIUS + 1) - RADIUS;

        if (sqrt(x * x + y * y) < RADIUS)
        {
            bmp_set_pixel(bmp, xc + x, yc + y, color);
        }
    }
}

void load_bmp_to_shm(bmpfile_t *bmp, void *pointer)

{
    sem_wait(semaphore_one);
    int pos = 0;
    for (int i = 0; i < SM_WIDTH * SM_HEIGHT; ++i)
    {
        int x = i / SM_HEIGHT;
        int y = i % SM_HEIGHT;

        rgb_pixel_t *tmp_p = bmp_get_pixel(bmp, x, y);
        if ((tmp_p->blue == 255) && (tmp_p->red == 0) && (tmp_p->green == 0) && (tmp_p->alpha == 0))
        {
            ((struct msg *)pointer)->buffer[x][y] = 1;
        }
        sem_post(semaphore_two);
    }
}

void reset_bmp(bmpfile_t *bmp)

{
    for (int i = 0; i < SM_WIDTH * SM_HEIGHT; ++i)
    {
        int x = i / SM_HEIGHT;
        int y = i % SM_HEIGHT;
        bmp_set_pixel(bmp, x, y, empty_pixel);
    }
}

void reset_buffer(void *pointer)
{
    for (int i = 0; i < SM_WIDTH * SM_HEIGHT; ++i)
    {
        int x = i / SM_HEIGHT;
        int y = i % SM_HEIGHT;
        ((struct msg *)pointer)->buffer[x][y] = 0;
    }
}

void handle_key_resize(int first_resize)
{
    if (first_resize == TRUE)
    {
        first_resize = FALSE;
    }
    reset_console_ui();
    reset_bmp(bmp);
    reset_buffer(pointer);
    float scale_x = SM_WIDTH / (COLS - BTN_SIZE_X);
    float scale_y = SM_HEIGHT / LINES;
}

void handle_key_mouse()
{
    if (getmouse(&event) == OK && check_button_pressed(print_btn, &event))
    {
        mvprintw(LINES - 1, LINES - 1, "Print done! ");
        take_snapshot();
        refresh();
        sleep(1);
        for (int j = 0; j < COLS - BTN_SIZE_X - 2; j++)
            mvaddch(LINES - 1, j, ' ');
    }
}

void handle_key_movement(int cmd)
{
    reset_bmp(bmp);
    float scale_x = SM_WIDTH / (COLS - BTN_SIZE_X);
    float scale_y = SM_HEIGHT / LINES;

    reset_buffer(pointer);
    print_circle(bmp, floor(circle.x * scale_x), floor(circle.y * scale_y), empty_pixel);

    move_circle(cmd);

    print_circle(bmp, floor(circle.x * scale_x), floor(circle.y * scale_y), pixel);
    draw_circle();
    load_bmp_to_shm(bmp, pointer);
}

int main()
{
    int first_resize = TRUE;
    semaphore_one = sem_open(sem_path_1, 0);
    semaphore_two = sem_open(sem_path_2, 0);
    init_console_ui();

    bmp = bmp_create(SM_WIDTH, SM_HEIGHT, DEPTH);

    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);

    ftruncate(shm_fd, sizeof(struct msg));

    pointer = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    while (true)
    {
        sleep(0.5);
        int cmd = getch();

        switch (cmd)
        {
        case KEY_RESIZE:
            handle_key_resize(first_resize);
            break;

        case KEY_MOUSE:
            handle_key_mouse();
            break;

        case KEY_LEFT:
        case KEY_DOWN:
        case KEY_UP:
        case KEY_RIGHT:
            handle_key_movement(cmd);
            break;
        }
    }

    bmp_destroy(bmp);

    munmap(pointer, sizeof(struct msg));

    close(shm_fd);
    sem_close(semaphore_one);
    sem_close(semaphore_two);

    endwin();
    return 0;
}
