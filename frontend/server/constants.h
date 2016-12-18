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
#include <fstream>
#include <grpc++/grpc++.h>
#include <iomanip>

#include "../../backend/storage_query/master_client.h"

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define LINE_LIMIT 1000
#define MAX_CON 128
#define DEFAULT_PORT 8080
#define BUFF_SIZE 8192

const static char* HTTP_HEADER = "HTTP/1.0 200 OK\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: text/html\nContent-Length: ";
const static char* HTTP_HEADER_FILE =
		"HTTP/1.0 200 OK\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: multipart/form-data\nContent-Disposition: attachment; filename=\"";
const static char* HTTP_HEADER_FILE2 = "\"\nContent-Length: ";

const static char* CONTENT_LEN = "Content-Length: ";
const static char* COOKIE = "Cookie: ";
const static char* REFERER = "Referer: ";

const static char* DOMAIN_NAME = "localhost";

const static char* WRITE_EMAIL = "/writeEmail";
const static char* SEND_EMAIL = "/sendEmail";
const static char* EMAILS = "/emails ";
const static char* FILES = "/files ";
const static char* EMAIL_ = "/email-";
const static char* FILE_ = "/file-";
const static char* FOLDER_ = "/folder-";
const static char* SIGNUP = "/signup";
const static char* LOGINSUBMIT = "/loginsubmit";
const static char* NEW_FOLDER = "/newFolder";
const static char* UPLOAD_FILE = "/uploadFile";

const static char* FOLDER_NAME = "folderName";
const static char* FILE_PRE = "file-";
const static char* FOLDER_PRE = "folder-";

const static char* UPLOADED_FILE = "uploadedFile";
const static char* FILE_NAME_PRE = "Content-Disposition: form-data; name=\"uploadedFile\"; filename=\"";

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

/* threads & fds */
pthread_t threads[1000];
int fd;
int threadNum = -1;
bool isDebug = false;
static char* remainingChars = (char*) malloc(sizeof(char) * LINE_LIMIT * 1000);
static int remainingNum = 0;
static int sockfd;
static std::set<int> allfd;

#endif /* CONSTANTS_H_ */
