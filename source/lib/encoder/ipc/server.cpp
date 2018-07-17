//
//  server.cpp
//  server for tcp socket connection
//
//  Created by Thorsten Laude on 08.01.16.
//

#include <iostream>
//#include <iosfwd>
//#include <memory>
//#include <string>
//#include <utility>
//#include <vector>
//#include <iomanip>
#include <exception>
#include <signal.h>

// includes for socket connection
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// defining sockets globally to make them available in signal handler
int sockfd, sockfdLoop;

// used to display error messages when connections fail
void errorFct(const char *msg) {
  perror(msg);
  throw std::exception();
}

// capture SIGINT, SIGTERM and SIGQUIT to make sure that the socket(s) is closed when the program is terminated
void handler(int signum) {
  throw std::exception();
}

int main(int argc, const char * argv[]) {

  // parameters
  int portNo = 12345;
  int backlog = 20; // TODO: what should be used as backlog size?
  const int dummyDataSize = 10;
  
  // use handler for SIGINT, SIGTERM, SIGQUIT
  if ( signal(SIGINT, handler) == SIG_ERR ) {
    return EXIT_FAILURE;
  }
  if ( signal(SIGTERM, handler) == SIG_ERR ) {
    return EXIT_FAILURE;
  }
  if ( signal(SIGQUIT, handler) == SIG_ERR ) {
    return EXIT_FAILURE;
  }
  
  try {
    
    // create socket
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      errorFct("ERROR opening socket");
    }
    // is this something that should be removed?
    int soReuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &soReuse, sizeof(int)) < 0) {
      errorFct("ERROR in setsockopt");
    }
    
    // bind socket
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portNo);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      errorFct("ERROR on binding");
    }
    // listen on socket
    listen(sockfd, backlog);
    clilen = sizeof(cli_addr);
    
    // endless loop waiting for clients to connect
    while (1) {
      
      // some dummy integers are transmitted between server and client
      int dummyDataArray[dummyDataSize] = {0};
      
      std::cout << "[server] waiting for client connection" << std::endl;
      sockfdLoop = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      if (sockfdLoop < 0) {
        errorFct("ERROR on accept");
      }
      std::cout << "[server] client has connected" << std::endl;
      
      long n = 0;
      
      
      n = read(sockfdLoop ,dummyDataArray, dummyDataSize*sizeof(int));
      if (n < 0) {
        errorFct("ERROR reading from socket");
      }
      
      // do some dummy stuff to dummy data
      for (std::size_t counter= 0; counter < dummyDataSize; counter++) {
        dummyDataArray[counter] *= 2;
      }
      
      // sending modified data back to client
      n = write(sockfdLoop, dummyDataArray, dummyDataSize*sizeof(int));
      if (n < 0) {
        errorFct("ERROR writing to socket");
      }
      
      // closing socket
      close(sockfdLoop);
    }
  } catch (std::exception &e) {
    std::cout << "exception: " << e.what()  << std::endl;
    close(sockfd);
    close(sockfdLoop);
    std::cout << std::endl << "=========================="
              << std::endl << "Closed sockets, terminate program" << std::endl;
    return EXIT_SUCCESS;
  }
  
  // we should never get here since the program is terminated using the signal handler
  return EXIT_SUCCESS;
}
