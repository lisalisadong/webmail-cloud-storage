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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string>
#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <set>
#include <errno.h>
#include <vector>
#include <dirent.h>
#include <map>
#include <ctime>
#include <sys/time.h>
#include <sstream>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <set>
#include <errno.h>
#include <vector>
#include <dirent.h>
#include <map>
#include <ctime>

#include <grpc++/grpc++.h>
#include "../../backend/storage_query/storage_client.h"
#include "../../backend/storage_query/storage_query.grpc.pb.h"

#define LINE_LIMIT 1000
#define MAX_CON 128
#define DEFAULT_PORT 8000
#define BUFF_SIZE 8192

using grpc::Channel;
using namespace std;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using storagequery::StorageQuery;
using storagequery::GetRequest;
using storagequery::GetResponse;
using storagequery::PutRequest;
using storagequery::PutResponse;
using storagequery::CPutRequest;
using storagequery::CPutResponse;
using storagequery::DeleteRequest;
using storagequery::DeleteResponse;

const static char* HTTP_HEADER = "HTTP/1.0 200 OK\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: text/html\nContent-Length: ";

class Message {
public:
	struct sockaddr_in clientAddr;
	unsigned int addrLen;
	int confd;

	Message() {

	}

	Message(struct sockaddr_in addr, unsigned int len, int fd) {
		clientAddr = addr;
		addrLen = len;
		confd = fd;
	}
};

#include "parseURL.h"

/* threads & fds */
pthread_t threads[1000];
int fd;
int threadNum = -1;
bool isDebug = false;
static char* remainingChars = (char*) malloc(sizeof(char) * (LINE_LIMIT + 1) * 2);
static int sockfd;
static set<int> allfd;

// encapsulate the socket creation process
void openSocket(int& sockfd, struct sockaddr_in serverAddr, int port) {
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "Cannot open a socket. \n");
		exit(5);
	}
	bzero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(port);
	if (bind(sockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0) {
		fprintf(stderr, "Cannot bind the socket. \n");
		exit(6);
	}
	if (listen(sockfd, MAX_CON) < 0) {
		fprintf(stderr, "Cannot listen to the socket. \n");
		exit(7);
	}
}

// read a line from socket
size_t getLine(struct Message* pM, char* line) {
	bzero(line, LINE_LIMIT);
	int lineIndex = 0;
	size_t readCnt = 0;
	// save remaining chars
	while (remainingChars[lineIndex] != 0 && remainingChars[lineIndex] != '\n') {
		line[lineIndex] = remainingChars[lineIndex++];
		readCnt++;
	}
	if (remainingChars[lineIndex] == '\n') {
		//remove used chars
		line[lineIndex] = remainingChars[lineIndex++];
		readCnt++;
		int remainingHead = 0;
		while (remainingChars[lineIndex] != 0) {
			remainingChars[remainingHead++] = remainingChars[lineIndex];
			remainingChars[lineIndex++] = 0;
		}
		remainingChars[remainingHead] = 0;
		return readCnt;
	}
	bzero(remainingChars, 2 * (LINE_LIMIT + 1));

	char* buf = (char*) malloc(sizeof(char) * (LINE_LIMIT + 1));
	bzero(buf, LINE_LIMIT + 1);
	size_t totalCnt = readCnt;
	while (true) {
		repeat: if ((readCnt = read(pM->confd, buf, LINE_LIMIT + 1)) < 0) {
			//error
			if (errno == EINTR)
				goto repeat;
			else {
				free(buf);
				return readCnt;
			}
		}
		if (readCnt == 0) {
			free(buf);
			return readCnt;
		}
		int i = 0;
		while (buf[i] != 0) {
			line[lineIndex] = buf[i];
			totalCnt++;
			if (buf[i] == '\n') break;
			i++;
			lineIndex++;
		}
		if (buf[i] == '\n') {
			// handle joined package
			i++;
			int remainingCharsIndex = 0;
			while (buf[i] != 0) {
				remainingChars[remainingCharsIndex] = buf[i];
				remainingCharsIndex++;
				i++;
			}
			break;
		}
	}
	free(buf);
	return totalCnt;
}

// close one client with the client messages pM
void closeClient(struct Message*& pM) {
	if (isDebug) {
		fprintf(stderr, "[%d] S: ", pM->confd);
		fputs("QUIT\n", stderr);
		fprintf(stderr, "[%d] Connection closed\n", pM->confd);
	}
//	write(pM->confd, QUIT_S, strlen(QUIT_S));
	close(pM->confd);
	allfd.erase(pM->confd);
}

void get() {
	//TODO!!!
	StorageClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
	std::string row("lisa");
	std::string col("emails");
	std::string val("from 1 to 2:xxx");
	client.Put(row, col, val);
	std::cout << "putting lisa||emails||from 1 to 2:xxx" << std::endl;

	std::string response = client.Get(row, col);
	std::cout << "getting lisa||emails: " << response << std::endl;
}

string getResponse(const char* fileLoc) {
	string response = HTTP_HEADER;
	string fileStr = "";
	FILE* f = fopen(fileLoc, "r");
	char fileLine[BUFF_SIZE];
	while (fgets(fileLine, BUFF_SIZE, f) != NULL) {
		fileStr += fileLine;
		bzero(fileLine, BUFF_SIZE);
	}
	response += to_string(fileStr.length());
	response += "\n\n";
	response += fileStr;
	return response;
}

bool checkPWD(char* url) {
	string urlStr = url;
	int emailIndex = urlStr.find("email");
	string email = urlStr.substr(emailIndex + 6, urlStr.find('&', emailIndex) - emailIndex - 6);
	int passwordIndex = urlStr.find("password");
	string password = urlStr.substr(passwordIndex + 9);
	if (isDebug) cout << "Current login user is: " << email << " with password: " << password << endl;

	return true;
}

int generateHTML(char* line, struct Message* pM, char* url) {
//	get();
	string response = "";
	if (!strncmp(url, " ", 1))
		response = getResponse("frontend/sites/login.html");
	else if (!strncmp(url, "signup ", 7))
		response = getResponse("frontend/sites/signup.html");
	else if (!strncmp(url, "loginsubmit ", 12))
		if (checkPWD(url))
			response = getResponse("frontend/sites/dashboard.html");
		else response = getResponse("frontend/sites/loginError.html");
	else if (!strncmp(url, "emails ", 7))
		response = getResponse("frontend/sites/emails.html");
	else if (!strncmp(url, "files ", 6))
		response = getResponse("frontend/sites/files.html");
	else response = getResponse("frontend/sites/notfound.html");
	write(pM->confd, response.c_str(), response.length());
	return 0;
}

// function to handle multi-thread
void* threadFun(void* arg) {
	struct Message* pM = (struct Message*) arg;
	char* sender = NULL;
	if (isDebug) {
		fprintf(stderr, "[%d] New connection\n", pM->confd);
	}
	bool isContent = false;
	char* url;
	while (true) {
		char* line = (char*) malloc(sizeof(char) * LINE_LIMIT);
		if (getLine(pM, line) <= 0) {
			//close the client
			closeClient(pM);
			return (void*) 0;
		}
		if (!strncmp(line, "\r\n", 2)) isContent = true;
		if (!strncmp(line, "GET ", 4)) url = line + 5;
		if (!strncmp(line, "POST", 4)) url = line + 6;
		if (isDebug) {
			fprintf(stderr, "[%d] C: ", pM->confd);
			fputs(line, stderr);
		}
		int checkStatus = 0;
		if (isContent) checkStatus = generateHTML(line, pM, url);
		if (checkStatus == -1) // QUIT
			return (void*) 0;
	}
	close(pM->confd);
	return (void*) 0;
}

int main(int argc, char *argv[]) {
	bzero(remainingChars, (LINE_LIMIT + 1) * 2);
// parse arguments
	char arg_char;
	int port = DEFAULT_PORT;
	while ((arg_char = getopt(argc, argv, "vp:")) != -1)
		switch (arg_char) {
		case 'v':
			isDebug = true;
			break;
		case 'p':
			port = atoi(optarg);
			if (port == 0) {
				fprintf(stderr, "Wrong option for -n.");
				exit(2);
			}
			break;
		case '?':
			fprintf(stderr, "Not supported option.\n");
			exit(3);
		}

	struct sockaddr_in serverAddr;
	char* ipStr = (char*) malloc(sizeof(char) * 16);
	openSocket(sockfd, serverAddr, port);
	while (1) {
		int confd = 0;
		struct sockaddr_in clientAddr;
		unsigned int addrLen = sizeof(clientAddr);
		confd = accept(sockfd, (struct sockaddr*) &clientAddr, &addrLen);
		if (confd < 0) {
			fprintf(stderr, "Cannot create the connection to the client");
			exit(4);
		}
		allfd.insert(confd);
		Message* m = new Message(clientAddr, addrLen, confd);
		pthread_t tid;
		pthread_create(&tid, NULL, threadFun, m);
	}
	return 0;

//	int c = 0;
//	int port = DEFAULT_PORT;
//	char arg_char;
//
//	while ((arg_char = getopt(argc, argv, "vp:")) != -1)
//		switch (arg_char) {
//		case 'v':
//			isDebug = true;
//			break;
//		case 'p':
//			port = atoi(optarg);
//			if (port == 0) {
//				fprintf(stderr, "Wrong option for -p.\n");
//				exit(2);
//			}
//			break;
//		case '?':
//			fprintf(stderr, "Not supported option.\n");
//			exit(3);
//		}
//	// 1.open new socket
//	int listen_fd = socket(PF_INET, SOCK_STREAM, 0);
//	if (listen_fd < 0) {
//		fprintf(stderr, "Cannot open socket (%s)\n", strerror(errno));
//		exit(1);
//	}
//
//	// configure the server
//	struct sockaddr_in servaddr;
//	bzero(&servaddr, sizeof(servaddr));
//	servaddr.sin_family = AF_INET;
//	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
//	servaddr.sin_port = htons(port);
//
//	// 2. bind: use the socket and associate it with the port number
//	if (bind(listen_fd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == -1) {
//		fprintf(stderr, "Unable to bind!!!\n");
//		exit(1);
//	}
//
//	// 3. listen: indicates that we want to listn to the port to which we bound; second arg is number of allowed connections
//	if (listen(listen_fd, 100) == -1) {
//		cerr << "Listen failed!" << endl;
//		exit(1);
//	}
//
//	// once get here, the server is set up and about to start listening
//	cerr << "\nServer configured to listen on port " << PORT << "!!!\n\n"
//			<< endl;
//
//	while (1) {
//		// 4. accept: wait here until we get a connection on that port
//		struct sockaddr_in clientaddr;
//		socklen_t clientaddrlen = sizeof(clientaddr);
//
//		fd = accept(listen_fd, (struct sockaddr *) &clientaddr, &clientaddrlen);
//
//		cerr << "Server got a connection from: ("
//				<< inet_ntoa(clientaddr.sin_addr) << ", "
//				<< ntohs(clientaddr.sin_port) << ")" << endl;
//
//		// pthread_create(&threads[threadNum], NULL, worker, &arg);
//
//		while (1) {
//			char line[1024];
//			int length = read(fd, line, 1024);
//
//			printf("%s/n", line);
//
//			if (length == 0) {
//				// client lost connection
//				cerr << " Client lost connection!!\n" << endl;
//				close(fd);
//				break;
//			}
//
//			if (length != -1) {
//				break;
//			}
//
//		}
//
//	}
//	return 0;
}

