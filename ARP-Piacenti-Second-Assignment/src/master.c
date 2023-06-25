#include "./../include/utility_modules.h"

int spawn(const char *program, char *arg_list[])
{

  pid_t child_pid = fork();

  if (child_pid < 0)
  {
    perror("Error while forking...");
    return 1;
  }

  else if (child_pid != 0)
  {
    return child_pid;
  }

  else
  {
    if (execvp(program, arg_list) == 0)
      ;
    perror("Exec failed");
    return 1;
  }
}

int main()
{

  int shm_fd;
  shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
  if (shm_fd == 1)
  {
    printf("Shared memory segment failed\n");
    exit(1);
  }

  char *arg_list_A[] = {"/usr/bin/konsole", "-e", "./bin/processA", NULL};
  char *arg_list_B[] = {"/usr/bin/konsole", "-e", "./bin/processB", NULL};


  sem_t *semaphore_one = sem_open(sem_path_1, O_CREAT, S_IRUSR | S_IWUSR, 1);
  sem_init(semaphore_one, 1, 1);

  sem_t *semaphore_two = sem_open(sem_path_2, O_CREAT, S_IRUSR | S_IWUSR, 1);
  sem_init(semaphore_two, 1, 0);

  pid_t pid_procA = spawn("/usr/bin/konsole", arg_list_A);
  pid_t pid_procB = spawn("/usr/bin/konsole", arg_list_B);

  int status;
  waitpid(pid_procA, &status, 0);
  waitpid(pid_procB, &status, 0);

  printf("Main program exiting with status %d\n", status);
  return 0;
}
