//
//  client.cpp
//  client for tcp socket connection
//
//  Created by Thorsten Laude on 08.01.16.
//  Copyright © 2016 Thorsten Laude. All rights reserved.
//

#include <iostream>
//#include <algorithm>
//#include <iosfwd>
//#include <memory>
//#include <string>
//#include <utility>
//#include <vector>
#include <iomanip>
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
#include <netdb.h> // gethostbyname

// defining socket globally to make them available in signal handler
int sockfd;

// used to display error messages when connections fail
void errorFct(const char *msg) {
  perror(msg);
  throw std::exception();
}

// capture SIGINT, SIGTERM and SIGQUIT to make sure that the socket is closed when the program is terminated
void handler(int signum) {

  throw std::exception();
}

int main(int argc, const char * argv[]) {
  
  // parameters
  int portNo = 12345;
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
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      errorFct("ERROR opening socket");
    }
    
    int soReuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &soReuse, sizeof(int)) < 0) {
      errorFct("ERROR in setsockopt");
    }
    
    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portNo);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
      errorFct("ERROR connecting");
    }
    
    int dummyDataArray[dummyDataSize] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    
    std::cout << "[client] sending data: ";
    for (std::size_t counter = 0; counter < dummyDataSize; counter++) {
      std::cout << std::setw(3) << dummyDataArray[counter];
    }
    std::cout << std::endl;
    
    long n;
    n = write(sockfd, dummyDataArray, dummyDataSize * sizeof(int));
    if (n < 0) {
      errorFct("ERROR writing to socket");
    }
    
    n = read(sockfd, dummyDataArray, dummyDataSize * sizeof(int));
    if (n < 0) {
      errorFct("ERROR reading from socket");
    }
    
    std::cout << "[client] received data: ";
    for (std::size_t counter = 0; counter < dummyDataSize; counter++) {
      std::cout << std::setw(3) << dummyDataArray[counter];
    }
    std::cout << std::endl;
    close(sockfd);
    
  } catch (std::exception & e) {
    std::cout << "Exception: " << e.what() << std::endl;
    close(sockfd);
    std::cout << std::endl << "=========================="
              << std::endl << " Closed socket, terminate program" << std::endl;
  }
  
  
  
  
  
  
  
}
