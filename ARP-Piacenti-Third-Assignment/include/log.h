#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define LOG_FILE_MODE 0644 // owner can r/w, others can only r

// Struct to store log file information (file descriptor and buffer)
typedef struct {
    char buff[255];
    int fd;
} Log;

// Funtion to write to log file: return 0 on success, 1 on failure
int write_log(char* msg, Log log_struct)
{
    // Get current date and time
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // Write to log file
    sprintf(log_struct.buff, "%d-%02d-%02d %02d:%02d:%02d - %s\n",t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, msg);

    if (write(log_struct.fd, log_struct.buff, strlen(log_struct.buff)) == strlen(log_struct.buff))
        return 0;

    return 1;
}

// Function to create a log file: return 0 on success, 1 on failure
int init_log(char* log_file, Log* log_struct, char* process_name)
{
    // Create log file if not existing, otherwise open it in append mode
    log_struct->fd = open(log_file, O_WRONLY | O_APPEND | O_CREAT, LOG_FILE_MODE);

    // Check that the file has been successfully created
    if (log_struct->fd == -1)
    {
        fprintf(stderr, "[%s]: error opening log file\n", process_name);
        return 1;
    }

    // Log init of the process
    if (write_log("[INFO] Process started", *log_struct))
    {
        fprintf(stderr, "[%s]: error writing to log file\n", process_name);
        close(log_struct->fd);
        return 1;
    }
}
