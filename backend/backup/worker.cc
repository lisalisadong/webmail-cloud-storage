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
class Worker {
public:
	int fd;

	Worker(int &fd) {
		this->fd = fd;
	}

	void start() {

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
}