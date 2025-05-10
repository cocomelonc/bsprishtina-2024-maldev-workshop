#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> // for inet_addr
#include <unistd.h>    // for close()

int main() {
  int socket_desc, client_sock, c, read_size;
  struct sockaddr_in server, client;
  char client_message[2048];

  // step 1: create socket
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1) {
      printf("could not create socket\n");
      return 1;
  }
  puts("socket created");

  // step 2: prepare sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(8888);

  // step 3: bind socket to IP/port
  if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("bind failed");
    return 1;
  }
  puts("bind successful");

  // step 4: start listening for incoming connections
  listen(socket_desc, 3);
  puts("waiting for incoming connections...");

  // step 5: accept incoming connection
  c = sizeof(struct sockaddr_in);
  client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
  if (client_sock < 0) {
    perror("accept failed");
    return 1;
  }
  puts("connection accepted");

  // step 6: echo back received messages
  while ((read_size = recv(client_sock, client_message, 2000, 0)) > 0) {
    write(client_sock, client_message, strlen(client_message));
  }

  // handle disconnection or errors
  if (read_size == 0) {
    puts("client disconnected");
  } else if (read_size == -1) {
    perror("recv failed");
  }

  return 0;
}