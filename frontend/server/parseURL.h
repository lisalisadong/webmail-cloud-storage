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
using namespace std;
#ifndef PARSEURL_H_
#define PARSEURL_H_

#define BUFF_SIZE 8192

const static char* HTTP_HEADER = "HTTP/1.0 200 OK\nDate: Fri, 31 Dec 1999 23:59:59 GMT\nContent-Type: text/html\nContent-Length: ";

string getResponse(const char* fileLoc, const char* headers) {
	string response = HTTP_HEADER;
	string fileStr = "";
	FILE* f = fopen(fileLoc, "r");
	char fileLine[BUFF_SIZE];
	while (fgets(fileLine, BUFF_SIZE, f) != NULL) {
		fileStr += fileLine;
		bzero(fileLine, BUFF_SIZE);
	}
	response += to_string(fileStr.length()) + "\n";
	response += headers;
	response += "\n";
	response += fileStr;
	return response;
}


string getSessionID(string userName) {
	int userNameInt = 0;
	for (int i = 0; i < userName.length(); i++)
		userNameInt += userName[i];
	srand(time(NULL) + userNameInt + rand());
	string res;
	for (int i = 0; i < 32; i++) {
		int letter = rand() % 123;
		while (!(letter >= 48 && letter <= 57)
				&& !(letter >= 97 && letter <= 122))
			letter = rand() % 123;
		res += char(letter);
	}
	return res;
}

#endif /* PARSEURL_H_ */
