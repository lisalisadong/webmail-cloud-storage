#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "adminConsole.h"

#define PORT_NUM 10000
#define DISPLAY_ITEM 10

using namespace std;

/*
 * four commond:
 * /: admin console
 * /nodedata: display node information
 * /nodedata/prev
 * /nodedata/next
 *
 */
void* adminConsole(void* arg) {
	int fd = *((int*) arg);
	char buf[1024];
	int page = 0, node = 0;
	vector<string> upBackendServer;

	while (read(fd, buf, sizeof(buf)) > 0) {
		int start = -1, end = -1, count = 0;

		for (int i = 0; i < 1024; i++) {
			if (buf[i] != ' ') continue;

			if (count == 0) {
				start = i;
				count++;
			} else {
				end = i;
				break;
			}
		}
		string request = string(buf).substr(start + 1, end - start - 1);
		cout << "new request: " << request << endl;

		if (request.compare("/") == 0) {
			renderAdminConsoleHomepage(fd, upBackendServer);
			page = 0;
			node = 0;
		} else if (request.compare("/data") == 0) {
			renderDataStoragePage(fd, upBackendServer, 0, 0);
		} else if (request.compare("/prev") == 0) {
			page--;
			if (page < 0) page = 0;
			renderDataStoragePage(fd, upBackendServer, node, page);
		} else if (request.compare("/next") == 0) {
			page++;
			renderDataStoragePage(fd, upBackendServer, node, page);
		} else {
			continue;
		}
	}

	close(fd);
	return NULL;
}

/* 
 * Main
 */
int main() {
	// create socket for listening
	int listen_fd = socket(PF_INET, SOCK_STREAM, 0);

	cout << "listen_fd: " +  to_string(listen_fd) << endl;

	// config server socket
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(PORT_NUM);

	//bind
	int ret_val = ::bind(listen_fd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	cout << "bind return value: " + to_string(ret_val) << endl;
	
	ret_val = listen(listen_fd, 100);
	if (ret_val >= 0 ) cout << "listenint at " + to_string(PORT_NUM) << endl;


	while (true) {
		// accept a connection from client
		struct sockaddr_in clientaddr;
		socklen_t clientaddrlen = sizeof(clientaddr);
		int* fd = (int*) malloc(sizeof(int));
		*fd = accept(listen_fd, (struct sockaddr*) &clientaddr, &clientaddrlen);
		cout << "accept a new connection" + to_string(*fd) << endl;
		if (fd < 0) continue;

		pthread_t thread;
		pthread_create(&thread, NULL, adminConsole, fd);
	}

	close(listen_fd);

	return 0;
}