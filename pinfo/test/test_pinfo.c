// COP4610 – Principles of Operating Systems – Summer C 2017
// Prof. Jose F. Osorio
// Student: Alicia Rodriguez – 5162522
//
// Assignment#: 2
// Project: Customizing Linux Kernel
// Specs: Adding Static System Call
// Due Date: 06/12/2017 by 11:55pm
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

void *print_message_function(void *ptr);

int main() {
  struct pinfo *p_info;
  int status;
  char *message1 = "Thread 1";
  char *message2 = "Thread 2";
  char *message3 = "Thread 3";
  int  iret1, iret2, iret3;
  int child1_pid;
  int child2_pid;
  int child3_pid;
  int child1_status = 0;
  int child2_status = 0;
  int child3_status = 0;

  p_info = malloc(sizeof(*p_info));

  // NOTE: The names of the pinfo struct fields have been abbriviated so they can be aligned nicely on the terminal.
  printf("PID\t STATE\t NICE\t P_PID\t NR_C\t NR_T\t YCPID\t YSPID\t OSPID\t STIME\t UTIME\t SYTIM\t CUTIM\t CSTIM\t UID\t COMM\t\n");

  /* create child processes and keep ‘em active */

  child1_pid = fork();
  child2_pid = fork();
  child3_pid = fork();

  if (child1_pid != 0) {
    child1_pid = wait(&child1_status);
  }
  // else {
  //   // printf("I am child1 with pid %d\n", child1_pid);
  //   exit(child1_status);
  // }

  if (child2_pid != 0) {
    child2_pid = wait(&child2_status);
  }
  // else {
  //   // printf("I am child2 with pid %d\n", child2_pid);
  //   exit(child2_status);
  // }

  if (child3_pid != 0) {
    child3_pid = wait(&child3_status);
  }
  // else {
  //   // printf("I am child3 with pid %d\n", child3_pid);
  //   exit(child3_status);
  // }

  /* create child threads and keep’em active */
  pthread_t thread1, thread2, thread3;

  pthread_create(&thread1, NULL, print_message_function, (void*) message1);
  pthread_create(&thread2, NULL, print_message_function, (void*) message2);
  pthread_create(&thread3, NULL, print_message_function, (void*) message3);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  pthread_join(thread3, NULL);

  /* If this is the parent process, call new system call */
  status = syscall(301, p_info);

  if (status < 0) {
    printf("Something went wrong. Status: %d\n", status);
    return 0;
  }

  /* Output to standard output the p_info returned by new system call */
  printf("%d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t %s\t\n",
  p_info->pid, p_info->state, p_info->nice, p_info->parent_pid, p_info->nr_children, p_info->nr_threads, p_info->youngest_child_pid, p_info->younger_sibling_pid, p_info->older_sibling_pid, p_info->start_time, p_info->user_time, p_info->sys_time, p_info->cutime, p_info->cstime, p_info->uid, p_info->comm);

  sleep(10);

  return 0;
}

void *print_message_function( void *ptr ) {
  char *message;
  message = (char *) ptr;
  // printf("%s \n", message);
}
