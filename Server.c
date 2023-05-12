#include <arpa/inet.h>

#include <netinet/in.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <sys/socket.h>

#include <unistd.h>

#include <sys/select.h>

#include <sys/types.h>

#define PORT 5080
#define BUFFER_SIZE 1024

int checkBufferExit(char* buffer){
    return strstr(buffer, "exit") != NULL; // return 1 if equals to exit
}

int main() {
  int server_fd, client_fd, addr_len;
  struct sockaddr_in server_addr, client_addr;
  //  initialize the buffer with BUFFER_SIZE 1024
  char buffer[BUFFER_SIZE];
  // initialize the server socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  // check if the socket was created successfully (-1 if not)
  if (server_fd == -1) {
    perror("socket");
    // exit the code execution with error EXIT_FAILURE
    exit(EXIT_FAILURE);
  }
  server_addr.sin_family = AF_INET;  // set the socket to use IPv4 protocol
  server_addr.sin_addr.s_addr = INADDR_ANY;  // accept connections from any ip address
  server_addr.sin_port = htons(PORT);  // set the socket port number
  
  // bind the socket server_fd with the server address (if successful returns 0)
  if (bind(server_fd, (struct sockaddr * ) & server_addr, sizeof(server_addr)) == -1) {
    perror("bind");
    // exit the code execution upon error
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 1) == -1) {  // attempt to listen for incoming connection requests
    perror("listen");
    // exit the code execution upon error
    exit(EXIT_FAILURE);
  }
  addr_len = sizeof(client_addr);
  // accept the new client connection and create a socket for it
  client_fd = accept(server_fd, (struct sockaddr * ) & client_addr, (socklen_t * ) & addr_len);
  if (client_fd == -1) { // check if the socket was created successfully
    perror("accept");
    // exit the code execution upon error
    exit(EXIT_FAILURE);
  }
  
  fd_set read_fds, write_fds;  // create variables for file descriptors

  while (1) {
    FD_ZERO(&read_fds);  // initialize the file descriptor for read to zero
    FD_ZERO(&write_fds);    // intitialize the file descritor for write to be zero
    FD_SET(client_fd, &read_fds);  // add client_fd file descriptor to what will be used in select
    FD_SET(fileno(stdin), &read_fds);  // add stdin file descriptor to what will be used in select
    char serverName[]="Server: ";
    
    int max_fd;  // holds the maximum file descriptor of stdin and client_fd
    if(client_fd > fileno(stdin)){
    
      max_fd = client_fd;
      
    }else{
      max_fd = fileno(stdin);
    }
    
    // find the number of files that are ready 
    int ready = select(max_fd + 1, &read_fds, &write_fds, NULL, NULL);
    if(ready == -1){  // report if the select function executed with errors
      perror("select error");
      exit(EXIT_FAILURE);
    }
    
    if(FD_ISSET(client_fd, &read_fds)){ // if client is ready
      memset(buffer, 0, BUFFER_SIZE);  // set the content of the buffer to 0
      read(client_fd, buffer, BUFFER_SIZE);  // read the client response into the buffer
      printf("\n%s", buffer);  // print the client response in the terminal
      
      // no need to check for exit if the response if from client
      
      printf("\n%s", serverName);  // print "Server: " in the terminal for output formatting purposes
	  fflush(stdout);  // make sure the above output is not buffered and is always being printed
    

    }
    if(FD_ISSET(fileno(stdin), &read_fds)){ // if the stdin (user input) is ready
		memset(buffer, 0, BUFFER_SIZE); // set the content of the buffer to 0
		printf("Server: ");
		// concatenate stdin with serverName for output formatting purposes
      	sprintf(buffer,serverName);
		// get the user input into the buffer with a max BUFFER_SIZE minus what has already been used up
		fgets(buffer + strlen(buffer), BUFFER_SIZE - strlen(buffer), stdin); 
      	send(client_fd, buffer, strlen(buffer), 0);  // send the formatted stdin to client
    
		if(checkBufferExit(buffer)){ // exit if the input contained "exit"
		    perror("exit");
		    exit(EXIT_FAILURE);
		}
		

    }

    
  }
  close(client_fd);  // close the client socket
  close(server_fd);  // close the server socket
  return 0;  // close the program with no errors
}
