// COP4610 – Principles of Operating Systems – Summer C 2017
// Prof. Jose F. Osorio
// Student: Alicia Rodriguez – 5162522
//
// Assignment#: 2
// Project: Customizing Linux Kernel
// Specs: Adding Static System Call
// Due Date: 06/12/2 017 by 11:55pm
//
// Module Name: Testing PINFO System Call
// I Certify that this program code has been written by me
// and no part of it has been taken from any sources.

#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <pinfo.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 3
#define NUM_PROCESSES 3

void *thread_func(void *thread_id);

int main() {
  struct pinfo *p_info;
  int status, pid;
  long process, thread;

  // NOTE: The names of the pinfo struct fields have been abbreviated so they can be aligned nicely on the terminal.
  printf("\nPID\t STATE\t NICE\t P_PID\t NR_C\t NR_T\t YCPID\t YSPID\t OSPID\t STIME\t UTIME\t SYTIM\t CUTIM\t CSTIM\t UID\t COMM\t\n");

  /* create child processes and keep ‘em active */
  for (process = 0; process < NUM_PROCESSES; process++) {
    pid = fork();

    if (pid < 0) {
      exit(1);
    } else if (pid == 0) {  /* Child Process */
      sleep(process);
      exit(0);
    } else {  /* Parent Process */

      /* Create child threads and keep’em active */
      pthread_t threads[NUM_THREADS];

      for (thread = 0; thread < NUM_THREADS; thread++) {
        pthread_create(&threads[thread], NULL, thread_func, (void *)thread);
      }

      /* Call new system call */
      p_info = malloc(sizeof(*p_info));
      status = syscall(301, p_info);

      // Join Threads
      for (thread = 0; thread < NUM_THREADS; thread++) {
        pthread_join(threads[thread], NULL);
      }

      /* Output to standard output the p_info returned by new system call */
      printf("%d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %s\t\n",
      p_info->pid, p_info->state, p_info->nice, p_info->parent_pid, p_info->nr_children, p_info->nr_threads, p_info->youngest_child_pid, p_info->younger_sibling_pid, p_info->older_sibling_pid, p_info->start_time, p_info->user_time, p_info->sys_time, p_info->cutime, p_info->cstime, p_info->uid, p_info->comm);
    }
  }

  for (process = 0; process < NUM_PROCESSES; process++) {
    // Wait for child processes.
    wait(NULL);
  }

  if (status < 0) {
    printf("Something went wrong. Status: %d\n", status);
    return 0;
  }

  return 0;
}

void *thread_func(void *thread_id) {
  // Do nothing.
}
