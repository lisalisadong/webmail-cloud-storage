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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <set>
#include <vector>
#include <dirent.h>
#include <map>
#include <ctime>
#include <sys/time.h>
#include <algorithm>
#include <grpc++/grpc++.h>

#include "../../backend/storage_query/storage_client.h"
#include "parseURL.h"
#include "constants.h"

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

#include "parseURL.h"
#include "handleEmails.h"
#include "handleFiles.h"
#include "helper.h"

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
		remainingNum--;
	}
	if (remainingChars[lineIndex] == '\n') {
		//remove used chars
		line[lineIndex] = remainingChars[lineIndex++];
		readCnt++;
		remainingNum--;
		int remainingHead = 0;
		while (remainingHead < remainingNum) {
			remainingChars[remainingHead++] = remainingChars[lineIndex];
			remainingChars[lineIndex++] = 0;
		}
		remainingChars[remainingHead] = 0;
		return readCnt;
	}
	bzero(remainingChars, 1000 * LINE_LIMIT);

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
		while (i < readCnt) {
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
			while (i < readCnt) {
				remainingChars[remainingCharsIndex] = buf[i];
				remainingCharsIndex++;
				remainingNum++;
				i++;
			}
			break;
		}
	}
	free(buf);
	return totalCnt;
}

size_t readN(struct Message* pM, char* line, int N) {
	bzero(line, N + 1);
	int lineIndex = 0;
	size_t readCnt = 0;
	// save remaining chars
	while (remainingNum > 0 && readCnt < N) {
		line[lineIndex] = remainingChars[lineIndex++];
		readCnt++;
		remainingNum--;
	}
	if (readCnt == N) {
		//remove used chars
		int remainingHead = 0;
		while (remainingChars[lineIndex] != 0) {
			remainingChars[remainingHead++] = remainingChars[lineIndex];
			remainingChars[lineIndex++] = 0;
		}
		remainingChars[remainingHead] = 0;
		return readCnt;
	}
	bzero(remainingChars, 1000 * LINE_LIMIT);

	char* buf = (char*) malloc(sizeof(char) * (N + 1));
	bzero(buf, N + 1);
	size_t totalCnt = readCnt;
	while (true) {
		if (totalCnt == N) {
			free(buf);
			return totalCnt;
		}
		repeat: if ((readCnt = read(pM->confd, buf, N)) < 0) {
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
		while (i < readCnt) {
			line[lineIndex] = buf[i];
			totalCnt++;
			i++;
			lineIndex++;
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
	close(pM->confd);
	allfd.erase(pM->confd);
}

// function to handle multi-thread
void* threadFun(void* arg) {
	struct Message* pM = (struct Message*) arg;
	char* sender = NULL;
	if (isDebug) {
		fprintf(stderr, "[%d] New connection\n", pM->confd);
	}
	bool isGet = true;
	bool isLastLine = false;
	string url;
	string cookie;
	string referer;
	char* lastLineCharArr = (char*) malloc(sizeof(char) * 2);
	lastLineCharArr[0] = 'a';
	lastLineCharArr[1] = 0;

	int contentLen = 0;
	while (true) {
		if (isLastLine) {
			free(lastLineCharArr);
			lastLineCharArr = (char*) malloc(sizeof(char) * (contentLen + 1));
			readN(pM, lastLineCharArr, contentLen);
			break;
		}
		char* line = (char*) malloc(sizeof(char) * LINE_LIMIT);
		if (getLine(pM, line) <= 0) {
			//close the client
			closeClient(pM);
			return (void*) 0;
		}
		if (isDebug) {
			fprintf(stderr, "[%d] C: ", pM->confd);
			fputs(line, stderr);
		}
		if (!strncmp(line, "\r\n", 2)) {
			if (isGet)
				break;
			else {
				isLastLine = true;
			}
		} else if (!strncmp(line, "GET ", 4)) {
			url = line + 4;
		} else if (!strncmp(line, "POST", 4)) {
			isGet = false;
			url = line + 5;
		} else if (!strncmp(line, CONTENT_LEN, strlen(CONTENT_LEN)))
			contentLen = atoi(line + strlen(CONTENT_LEN));
		else if (!strncmp(line, COOKIE, strlen(COOKIE)))
			cookie = line + strlen(COOKIE);
		else if (!strncmp(line, REFERER, strlen(REFERER))) referer = line + strlen(REFERER);
		free(line);
	}
	cout << "url: " << url << endl;
	cout << "lastLine: " << lastLineCharArr << endl;
	try {
		generateHTML(pM, url.c_str(), lastLineCharArr, cookie, referer, contentLen);
	} catch (int e) {
		if (isDebug) cout << "Error: " << e << endl;
	}
	close(pM->confd);
	free(lastLineCharArr);
	return (void*) 0;
}

int main(int argc, char *argv[]) {
	bzero(remainingChars, (LINE_LIMIT + 1) * 2);
	mqueueInitialize();
//	testInitialize();
//	testInitializezhixu();
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
	cout << "Now Listening to port: " << port << endl;
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
}
