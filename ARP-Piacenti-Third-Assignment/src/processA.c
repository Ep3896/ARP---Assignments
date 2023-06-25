#include "./../include/processA_utilities.h"
#include "./../include/utility_modules.h"


#include "./../include/log.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <stdbool.h>

#define LOG_FILE "log/processA.log"


bmpfile_t *bmp;

void * data;

rgb_pixel_t pixel = {255, 0, 0, 0}; //Color of the circle
rgb_pixel_t empty_pixel = {255, 255, 255, 0}; //White pixel

int shm_fd;
int print_counter = 0;

sem_t *sem_id1;
sem_t *sem_id2;


// Create a struct to store the position of the circle in the map 1600x600
struct msg
{
    int buf[1600][600];
};




//////////////////////////////////////////////////////////////////////////////////////////////////////




bool isAddressValid(const char* addr) {
    if (addr[0] == '\0') {
        return true;
    }

    int dotCount = 0;
    int digitCount = 0;

    while (*addr) {
        if (*addr == '.') {
            dotCount++;
            digitCount = 0;
        } else if (*addr >= '0' && *addr <= '9') {
            digitCount++;
        } else {
            return true;
        }
        addr++;
    }

    if (dotCount != 3) {
        return true;
    }

    return false;
}





bool readFromClient(int client_fd, int* cmd) {
    char buffer[10];
    int bytesRead = read(client_fd, buffer, sizeof(buffer) - 1);

    if (bytesRead < 0 || bytesRead >= sizeof(buffer)) {
        return false;
    }

    buffer[bytesRead] = '\0';
    *cmd = atoi(buffer);

    return true;
}



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

void load_bmp_to_shm(bmpfile_t *bmp, void* data) 

    {
        sem_wait(sem_id1);
        int pos = 0;
        for (int i = 0; i < SM_WIDTH * SM_HEIGHT; ++i) {
            int x = i / SM_HEIGHT;
            int y = i % SM_HEIGHT;

            rgb_pixel_t *tmp_p = bmp_get_pixel(bmp, x, y);
            if ((tmp_p->blue == 255) && (tmp_p->red == 0) && (tmp_p->green == 0) && (tmp_p->alpha == 0)) {
                ((struct msg *)data)->buf[x][y] = 1;
                  }
        sem_post(sem_id2);
    }
    }


bool isPortValid(int portNumber) {
    if (portNumber < 2500 || portNumber > 65535) {
        return true;
    }
    return false;
}

void reset_bmp(bmpfile_t * bmp) 

    {
        for (int i = 0; i < SM_WIDTH * SM_HEIGHT; ++i) {
            int x = i / SM_HEIGHT;
            int y = i % SM_HEIGHT;
            bmp_set_pixel(bmp, x, y, empty_pixel);

        }
    }

void reset_buffer( void *data)
    {
        for (int i = 0; i < SM_WIDTH * SM_HEIGHT; ++i) {
            int x = i / SM_HEIGHT;
            int y = i % SM_HEIGHT;
            ((struct msg*)data)->buf[x][y] = 0; 

        }

    }



void handle_key_resize(int first_resize)
{
    if (first_resize == TRUE)
    {
        first_resize = FALSE;
    }
    reset_console_ui();

  
}

void handle_key_mouse()
{
    if (getmouse(&event) == OK && check_button_pressed(print_btn, &event))
    {
        mvprintw(LINES - 1, LINES - 1, "Print snaposhot");
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
    float scale_x = SM_WIDTH/(COLS-BTN_SIZE_X);
    float scale_y = SM_HEIGHT/LINES;
    
    reset_buffer(data);
    print_circle(bmp, floor(circle.x * scale_x), floor(circle.y * scale_y), empty_pixel);

    move_circle(cmd);

    print_circle(bmp, floor(circle.x*scale_x), floor(circle.y*scale_y), pixel);
    draw_circle();
    load_bmp_to_shm(bmp, data);


}




bool writeToServer(int server_fd, int cmd) {
    char buffer[10];
    int len = sprintf(buffer, "%d", cmd);

    if (len < 0 || write(server_fd, buffer, len) < 0) {
        return false;
    }

    return true;
}


int main()
{


  Log logFile;

if (init_log(LOG_FILE, &logFile, "Process A"))
  exit(EXIT_FAILURE);

printf("========================================\n");
printf("   Enrico Piacenti - Third Assignment   \n");
printf("                Socket                  \n");
printf("========================================\n");
printf("Select one of the following modes:\n");
printf("1) Normal\n");
printf("2) Server\n");
printf("3) Client\n");



int selectedMode;
// Socket variables
int portNumber = 5555;
char hostAddress[13];
int clientSocket = -1;
int serverSocket = -1;
struct sockaddr_in serverAddress, clientAddress;


do
{
    printf("Select a mode: ");
    scanf("%d", &selectedMode);
} while (selectedMode < 1 || selectedMode > 3);


switch (selectedMode) {
    case 1:
        // Normal mode, no configuration needed
        break;

    case 2: {
        // Server mode
        int validPort = 0;

        do {
            printf("Please enter the port number: ");
            scanf("%d", &portNumber);
            validPort = isPortValid(portNumber);
        } while (validPort);

        // Create a socket
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            write_log("[ERROR] Failed to create socket", logFile);
            close(logFile.fd);
            exit(EXIT_FAILURE);
        }

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(portNumber);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        // Bind socket to address and port
        if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
            write_log("[ERROR] Failed to bind socket", logFile);
            close(logFile.fd);
            close(serverSocket);
            exit(EXIT_FAILURE);
        }

        // Listen for incoming connections
        if (listen(serverSocket, 1) == -1) {
            write_log("[ERROR] Failed to listen on socket", logFile);
            close(logFile.fd);
            close(serverSocket);
            exit(EXIT_FAILURE);
        }

        printf("Waiting for client on port %d...\n", portNumber);

        // Accept incoming connection
        int addressLength = sizeof(clientAddress);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, (socklen_t *)&addressLength);
        if (clientSocket == -1) {
            write_log("[ERROR] Failed to accept connection", logFile);
            close(logFile.fd);
            close(serverSocket);
            exit(EXIT_FAILURE);
        }

        break;
    }

    case 3: {
        // Client mode
        int validPort = 0;
        int validAddress = 0;

        do {
            printf("Please enter the port number: ");
            scanf("%d", &portNumber);
            validPort = isPortValid(portNumber);
        } while (validPort);

        do {
            printf("Please enter the host address: ");
            scanf("%s", hostAddress);
            validAddress = isAddressValid(hostAddress);
        } while (validAddress);

        // Create a socket
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            write_log("[ERROR] Failed to create socket", logFile);
            close(logFile.fd);
            exit(EXIT_FAILURE);
        }

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(portNumber);
        serverAddress.sin_addr.s_addr = inet_addr(hostAddress);

        // Connect to the server
        if (connect(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
            write_log("[ERROR] Failed to establish connection", logFile);
            close(logFile.fd);
            close(serverSocket);
            exit(EXIT_FAILURE);
        }

        break;
    }
}





    int first_resize = TRUE;
    sem_id1 = sem_open(sem_path_1, 0);
    sem_id2 = sem_open(sem_path_2, 0);
    init_console_ui();


    bmp = bmp_create(SM_WIDTH, SM_HEIGHT, DEPTH);

    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);

    ftruncate(shm_fd, sizeof(struct msg));

    data = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);


    while (true)
    {
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

    endwin();
    return 0;
}

