#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>
#include <sstream>
#include <signal.h>
#include <fcntl.h>

using namespace std;

/* threads & fds */
pthread_t threads[1000];
int fd;
int threadNum = -1;


int main(int argc, char *argv[])
{
  int c = 0;
  int PORT = 8000;

  	// 1.open new socket
	int listen_fd = socket(PF_INET, SOCK_STREAM, 0);  
	if (listen_fd < 0) {
    	fprintf(stderr, "Cannot open socket (%s)\n", strerror(errno));    
    	exit(1);
  	}  


	// configure the server
  	struct sockaddr_in servaddr;
  	bzero(&servaddr, sizeof(servaddr));
  	servaddr.sin_family = AF_INET;
  	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
  	servaddr.sin_port = htons(PORT);

  	// 2. bind: use the socket and associate it with the port number
  	if(bind(listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
  		fprintf(stderr, "Unable to bind!!!\n");    
    	exit(1);
  	}


  	// 3. listen: indicates that we want to listn to the port to which we bound; second arg is number of allowed connections
    if (listen(listen_fd, 100) == -1) {
        cerr<<"Listen failed!"<<endl;
        exit(1);
    }


    // once get here, the server is set up and about to start listening
    cerr<<"\nServer configured to listen on port "<<PORT<<"!!!\n\n"<<endl;


    while(1) {
    	// 4. accept: wait here until we get a connection on that port
      struct sockaddr_in clientaddr;
    	socklen_t clientaddrlen = sizeof(clientaddr);

		  fd = accept(listen_fd, (struct sockaddr *)&clientaddr, &clientaddrlen);

      cerr<<"Server got a connection from: ("<<inet_ntoa(clientaddr.sin_addr)<<", "<<ntohs(clientaddr.sin_port)<<")"<<endl;
      
      // pthread_create(&threads[threadNum], NULL, worker, &arg);

      while(1) {
        char line[1024];
        int length = read(fd, line, 1024);

        printf("%s/n", line);

        if(length == 0) {
          // client lost connection
          cerr<<" Client lost connection!!\n"<<endl;
          close(fd);
          break;
        }

        if(length != -1) {
          break;
        }
        
      }

    }
    return 0;
}



























