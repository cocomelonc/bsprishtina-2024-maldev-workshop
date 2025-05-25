#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
  int sock;
  struct sockaddr_in server;
  char message[1000], server_reply[2000];

  // Step 1: Create socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    printf("Could not create socket\n");
    return 1;
  }
  puts("Socket created");

  // step 2: configure server IP and port
  server.sin_addr.s_addr = inet_addr("192.168.0.22"); // replace with your 
  
  // iPhone's IP
  server.sin_family = AF_INET;
  server.sin_port = htons(8888);

  // step 3: connect to server
  if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("Connection failed");
    return 1;
  }
  puts("Connected to server");

  // step 4: interactive message loop
  while (1) {
    printf("Enter message: ");
    scanf("%s", message);

    // Send message to server
    if (send(sock, message, strlen(message), 0) < 0) {
      puts("Send failed");
      return 1;
    }

    // Receive and print server reply
    if (recv(sock, server_reply, 2000, 0) < 0) {
      puts("Receive failed");
      break;
    }

    puts("Server reply:");
    puts(server_reply);
  }

  close(sock);
  return 0;
}
