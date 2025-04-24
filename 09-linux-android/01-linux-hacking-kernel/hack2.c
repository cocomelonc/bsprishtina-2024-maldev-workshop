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

static int __init shellcode_init(void) {
  // run shell
  char *argv[] = {"/bin/sh", NULL};
  char *envp[] = {NULL};

  printk(KERN_INFO "starting shellcode...\n");

  // execve from user space
  int ret = call_usermodehelper("/bin/sh", argv, envp, UMH_WAIT_PROC);
  if (ret == 0) {
    printk(KERN_INFO "shellcode launched successfully\n");
  } else {
    printk(KERN_ERR "failed to start shellcode. error code: %d\n", ret);
  }

  return 0;
}

static void __exit shellcode_exit(void) {
  printk(KERN_INFO "shellcode module unloaded\n");
}

module_init(shellcode_init);
module_exit(shellcode_exit);
