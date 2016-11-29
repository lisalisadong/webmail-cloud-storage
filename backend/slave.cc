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
#include <vector>
#include "Cache.h"


using namespace std;

/* threads & fds */
int fd;

Cache cache;

bool do_write(int &fd, const char *buf, int len) {  
  int sent = 0;  
  while (sent < len) {
    int n = write(fd, &buf[sent],len-sent);
    if (n<0)
      return false;    
    sent += n;  
  }  
  return true;
}

vector<string> getParams(string s, int paramNum) {
    vector<string> res;
    int prev = 0;
    for(int i = 0; i < s.length(); i++) {
      if(s[i] == ' ') {
        res.push_back(s.substr(prev, i - prev)); 
        prev = i + 1;
        if(res.size() == paramNum - 1) break;
      }
    }
    res.push_back(s.substr(prev));
    return res;
  }

/* execute single command */
bool execute(int &fd, string command) {
  // get rid of trailing carriage return \r
  while(command[command.length() - 1] == '\r') command.erase(command.length() - 1);

  cerr<<" C: "<<command<<endl;

  vector<string> params;

  string response;

  if(command.substr(0, 3) == "get") {

    params = getParams(command, 2);
    response = cache.get(params[1]) + "\n";
  } else if(command.substr(0, 3) == "set") {

    params = getParams(command, 3);
    cache.set(params[1], params[2]);
    response = "+OK \n";
  }

  do_write(fd, response.c_str(), response.length());
}

/* execute all valid commands in buffer and clear the executed commands */
void executeCommands(int &fd, string &buffer) {
  // cout<<"Buffer length is: " << buffer.length()<<endl;
  int start = 0;
  int end = 0;
  for(int i = 0; i < buffer.length(); i++) {
    if(buffer[i] == '\n') {
      end = i;
      execute(fd, buffer.substr(start, end - start));
      start = end + 1;
      end = start;
    }
  }

  buffer = buffer.substr(end);
}

/* Thread worker */
void *worker(void *arg) 
{ 
  int fd = *(int*)arg;
  cerr<<"New connection!"<<endl;
  
  string buffer;
  while(1) {
    char line[1024];
    int length = read(fd, line, 1024);
    if(length == 0) {
      // client lost connection
      cerr<<" Client lost connection!!\n"<<endl;
      close(fd);
      return NULL;
    }

    if(length != -1) {
      string temp(line);
  
      temp = temp.substr(0, length);
      cout<<"Read "<<length<<": "<<temp<<endl;
      buffer.append(temp);
      executeCommands(fd, buffer);
    }
  }

  close(fd);
  cerr<<"Connection closed!"<<endl;
}


int main(int argc, char *argv[])
{
  int c = 0;
  int PORT = 9876;

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
      
      pthread_t thread;

      pthread_create(&thread, NULL, worker, &fd);
    
    }
    

    return 0;
}




























