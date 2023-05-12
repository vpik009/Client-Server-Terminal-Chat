#include <arpa/inet.h>

#include <netinet/in.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <sys/socket.h>

#include <unistd.h>

#define SERVER_IP "XXX.XXX.XXX.XXX"  
#define PORT 5080
#define BUFFER_SIZE 1024

int checkBufferExit(char* buffer){
    return strstr(buffer, "exit") != NULL; // return 1 if contains "exit"
}
int main() {
  int client_fd;
  // set the socket ip address
  struct sockaddr_in server_addr;
  
  // create a buffer with a fixed size of 1024 bytes
  char buffer[BUFFER_SIZE];
  // initialize the socket
  client_fd = socket(AF_INET, SOCK_STREAM, 0);
  // check if the socket was created correctly (-1 returned if incorrect initialization)
  if (client_fd == -1) {
    perror("socket");
    // exit the execution with EXIT_FAILURE Message
    exit(EXIT_FAILURE);
  }
  
  server_addr.sin_family = AF_INET;  // set the socket to use IPv4 protocol
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);  // accept connections only from the server IP address
  server_addr.sin_port = htons(PORT);  // set the socket port number
  if (connect(client_fd, (struct sockaddr * ) & server_addr, sizeof(server_addr)) == -1) {
    perror("connect");
    exit(EXIT_FAILURE);
  }
  
  fd_set read_fds, write_fds;  // create variables for file descriptors
  
  while (1) {  // run the following code for an infinite amount of time (1 = true)
    FD_ZERO(&read_fds);  // initialize the file descriptor for read to zero
    FD_ZERO(&write_fds);    // intitialize the file descritor for write to be zero
    FD_SET(client_fd, &read_fds);  // add client_fd file descriptor to what will be used in select
    FD_SET(fileno(stdin), &read_fds);  // add stdin file descriptor to what will be used in select
    char clientName[]="Client: ";
    
    int max_fd;
    if(client_fd > fileno(stdin)){
    	max_fd = client_fd;
    }else{
    	max_fd = fileno(stdin);
    }
    
    // find the number of files that are ready 
    int ready = select(max_fd+1, &read_fds, &write_fds, NULL, NULL);
    if(ready == -1){  // report if the select function executed with errors
    	perror("select error");
    	exit(EXIT_FAILURE);
    }
    if(FD_ISSET(client_fd, &read_fds)){  // if client is ready
		memset(buffer, 0, BUFFER_SIZE);  // set the content of the buffer to 0
		read(client_fd, buffer, BUFFER_SIZE);  // read the server response into the buffer
		printf("\n%s", buffer);  // print the server response in the terminal
		
		printf("\n%s", clientName);  // print "Client: " in the terminal for output formatting purposes
		fflush(stdout);  // make sure the above output is not buffered and is always being printed
		
		if(checkBufferExit(buffer)){  // if server types exit then quit
			perror("exit");
			exit(EXIT_FAILURE);
		}
		
	
    }
    if(FD_ISSET(fileno(stdin), &read_fds)){  // if the stdin (user input) is ready
    	
    	memset(buffer, 0, BUFFER_SIZE);  // set the content of the buffer to 0
        printf("Client: ");
        
        // concatenate stdin with clientName for output formatting purposes
        sprintf(buffer,clientName);
        // get the user input into the buffer with a max BUFFER_SIZE minus what has already been used up
		fgets(buffer + strlen(buffer), BUFFER_SIZE - strlen(buffer), stdin); 
		send(client_fd, buffer, strlen(buffer), 0);  // send the user input to the server
		
		if(checkBufferExit(buffer)){  // if client types exit then quit
				perror("exit");
				exit(EXIT_FAILURE);
			}

    }
    

  }
  close(client_fd);  // close the socket
  return 0;  // close the program with no errors
}
