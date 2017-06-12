// COP4610 – Principles of Operating Systems – Summer C 2017
// Prof. Jose F. Osorio
// Student: Alicia Rodriguez – 5162522
//
// Assignment#: 2
// Project: Customizing Linux Kernel
// Specs: Adding Static System Call
// Due Date: 06/12/2017 by 11:55pm
//
// Module Name: PINFO System Call
// I Certify that this program code has been written by me
// and no part of it has been taken from any sources.

#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/pinfo.h>
#include <linux/list.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>

void check_if_pinfo_values(struct pinfo *kernel_pinfo, struct task_struct *task);
long prio_to_nice(long prio);

asmlinkage long sys_pinfo(struct pinfo *info) {

  struct task_struct *task = current;
  struct task_struct *task_ptr = task;
  struct task_struct *youngest_child;
  struct task_struct *younger_sibling;
  struct task_struct *older_sibling;
  struct list_head *list_ptr;
  int nr_children = 0;
  int nr_threads = -1;

  // Kernel-space pinfo
  struct pinfo *kernel_pinfo = kmalloc(sizeof(*kernel_pinfo), GFP_KERNEL);

  printk(KERN_INFO "sys_pinfo: Starting pinfo system call...\n");

  if (kernel_pinfo == NULL) {
    printk(KERN_ERR "sys_pinfo: Failure to kmalloc\n");
    return -1;
  }
  // If the address for the pinfo structure is null
  if (info == NULL) {
    // Invalid argument
    printk(KERN_ERR "sys_pinfo: Invalid argument\n");
    return -22;
  }

  kernel_pinfo->pid = task->pid;
  kernel_pinfo->state = task->state;
  kernel_pinfo->nice = prio_to_nice(task->prio);
  kernel_pinfo->parent_pid = task->parent->pid;

  list_for_each(list_ptr, &(task->children)) {
    nr_children++;
  }
  kernel_pinfo->nr_children = nr_children;

  do {
    nr_threads++;
  }
  while_each_thread(task, task_ptr);

  kernel_pinfo->nr_threads = nr_threads;

  // Last element of children would make it the youngest child
  youngest_child = list_entry(task->children.prev, struct task_struct, children);
  kernel_pinfo->youngest_child_pid = youngest_child->pid;

  // The first element on sibling, makes it the younger sibling
  younger_sibling = list_entry(task->sibling.next, struct task_struct, sibling);
  kernel_pinfo->younger_sibling_pid = younger_sibling->pid;

  // The last element on sibling, makes it the older sibling
  older_sibling = list_entry(task->sibling.prev, struct task_struct, sibling);
  kernel_pinfo->older_sibling_pid = older_sibling->pid;

  kernel_pinfo->start_time = task->start_time.tv_sec;
  kernel_pinfo->user_time = task->utime;
  kernel_pinfo->sys_time = task->stime;
  kernel_pinfo->cutime = task->signal->cutime;
	kernel_pinfo->cstime = task->signal->cstime;
  kernel_pinfo->uid = task->uid;
  strcpy(kernel_pinfo->comm, task->comm);

  check_if_pinfo_values(kernel_pinfo, task);

  // Copy Kernel-space pinfo into user-space pinfo
  if(copy_to_user(info, kernel_pinfo, sizeof(*kernel_pinfo))) {
    return -EFAULT;
  }

  return 0;
}

/**
  Function which applies a formula which converts the priority value to a
  nice value.
*/
long prio_to_nice(long prio) {
  long max_nice = 19;
  long min_nice = -20;
  long nice_width = max_nice - min_nice + 1;
  long max_rt_prio	= 100;
  long default_prio = max_rt_prio + nice_width / 2;

  return prio - default_prio;
}

/**
  Function which checks if a value to be set in pinfo is invalid.
  If so, then set the corresponding field value in pinfo to -1.
*/
void check_if_pinfo_values(struct pinfo *kernel_pinfo, struct task_struct *task) {

  if (kernel_pinfo->pid < 0) {
    kernel_pinfo->pid = -1;
  }

  if (kernel_pinfo->state < -1) {
    // Set it to an out of bounds number
    kernel_pinfo->state = -2;
  }

  // If nice is out of bounds
  if (kernel_pinfo->nice > 19 || kernel_pinfo->nice < -20) {
    // Set it to an out of bounds number
    kernel_pinfo->nice = 100;
  }

  if (kernel_pinfo->parent_pid < 0) {
    kernel_pinfo->parent_pid = -1;
  }

  if (kernel_pinfo->nr_children < 0) {
    kernel_pinfo->nr_children = -1;
  }

  if (kernel_pinfo->nr_threads < 0) {
    kernel_pinfo->nr_threads = -1;
  }

  // There are no children if the youngest_child_pid is the same as the parent pid
  if (kernel_pinfo->youngest_child_pid == task->pid) {
    kernel_pinfo->youngest_child_pid = -1;
  }

  // There is no younger sibling if it's equal to 0
  if (kernel_pinfo->younger_sibling_pid == 0) {
    kernel_pinfo->younger_sibling_pid = -1;
  }

  // There is no older sibling if it's equal to 0
  if (kernel_pinfo->older_sibling_pid == 0) {
    kernel_pinfo->older_sibling_pid = -1;
  }

  if (kernel_pinfo->start_time < 0) {
    kernel_pinfo->start_time = -1;
  }

  if (kernel_pinfo->user_time < 0) {
    kernel_pinfo->user_time = -1;
  }

  if (kernel_pinfo->sys_time < 0) {
    kernel_pinfo->sys_time = -1;
  }

  if (kernel_pinfo->cutime < 0) {
    kernel_pinfo->cutime = -1;
  }

  if (kernel_pinfo->cstime < 0) {
    kernel_pinfo->cstime = -1;
  }

  if (kernel_pinfo->uid < 0) {
    kernel_pinfo->uid = -1;
  }

  if (strlen(kernel_pinfo->comm) == 0) {
    strcpy(kernel_pinfo->comm, "No comm provided");
  }
}
