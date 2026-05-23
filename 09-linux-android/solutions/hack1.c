/*
 * exercise 1 solution
 * linux kernel module: run a non-interactive command to prove execution
 * hack1.c
 * author: @cocomelonc
 *
 * base: 01-linux-hacking-kernel/hack2.c
 *   original: call_usermodehelper("/bin/sh", {"/bin/sh", NULL}, ...)
 *   problem:  an interactive shell spawned from kernel space has no
 *             controlling terminal and exits immediately - not useful.
 *
 * change: run "/bin/sh -c 'id > /tmp/pwned.txt'" instead.
 *   this writes the effective UID/GID line to a file, giving concrete
 *   proof that the command ran with kernel-delegated privileges.
 *
 * key API:
 *   call_usermodehelper(path, argv, envp, wait_flag)
 *     path      - absolute path of the executable
 *     argv      - NULL-terminated argument array (argv[0] = path)
 *     envp      - NULL-terminated environment array
 *     wait_flag - UMH_WAIT_PROC: block until the process exits
 *
 * argv for "-c" usage:
 *   argv[0] = "/bin/sh"          (the interpreter)
 *   argv[1] = "-c"               (run the next arg as a command string)
 *   argv[2] = "id > /tmp/pwned.txt"   (the shell command)
 *   argv[3] = NULL               (terminator - always required)
 *
 * build:
 *   # place this file as hack1.c, update Makefile: obj-m += hack1.o
 *   make
 *   sudo insmod hack1.ko
 *   cat /tmp/pwned.txt          # should show uid=0(root) ...
 *   dmesg | tail                # check printk messages
 *   sudo rmmod hack1
 *
 * Makefile (copy from 01-linux-hacking-kernel, change hack.o -> hack1.o):
 *   obj-m += hack1.o
 *   all:
 *       make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
 *   clean:
 *       make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cocomelonc");
MODULE_DESCRIPTION("kernel exec: write id output to /tmp/pwned.txt");
MODULE_VERSION("0.001");

static int __init hack_init(void) {
  int ret;

  /*
   * exercise 1 change:
   * original argv: { "/bin/sh", NULL }
   *   - opens an interactive shell that has no terminal → exits at once
   *
   * updated argv: { "/bin/sh", "-c", "<command>", NULL }
   *   - "-c" tells /bin/sh to execute the argument as a shell command
   *   - "id > /tmp/pwned.txt" redirects `id` output to a file
   *   - result is verifiable: cat /tmp/pwned.txt after insmod
   */
  char *argv[] = {
    "/bin/sh",
    "-c",
    "id > /tmp/pwned.txt",
    NULL           /* argv must be NULL-terminated */
  };
  char *envp[] = {
    "HOME=/",
    "PATH=/sbin:/bin:/usr/sbin:/usr/bin",
    NULL
  };

  printk(KERN_INFO "[hack1] launching command...\n");

  ret = call_usermodehelper("/bin/sh", argv, envp, UMH_WAIT_PROC);
  if (ret == 0) {
    printk(KERN_INFO "[hack1] command completed successfully\n");
    printk(KERN_INFO "[hack1] check /tmp/pwned.txt\n");
  } else {
    printk(KERN_ERR "[hack1] command failed, error: %d\n", ret);
  }

  return 0;
}

static void __exit hack_exit(void) {
  printk(KERN_INFO "[hack1] module unloaded\n");
}

module_init(hack_init);
module_exit(hack_exit);
