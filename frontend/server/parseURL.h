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

#include "constants.h"

using namespace std;
#ifndef PARSEURL_H_
#define PARSEURL_H_

#define BUFF_SIZE 8192

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

int readFile(const char* fileName, string& data) {
	ifstream in;
	in.open(fileName, ios::in | ios::binary | ios::ate);
	int size = in.tellg();
	char* dataTmp = (char*) malloc(sizeof(char) * (size + 1));
	in.seekg(ios::beg);
	cout << size << endl;
	dataTmp[size] = 0;
	if (!in.read(dataTmp, size)) {
		cout << "read file error" << endl;
	}
	std::stringstream ss;
	for (int i = 0; i < size; ++i) {
		ss << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) (unsigned char) dataTmp[i];
	}
	data = ss.str();
	in.close();
	free(dataTmp);
	return size;
}

void writeFile(string data, Message* pM) {
	stringstream ss;
	vector<char> hexChars;
	int tmp;
	int index = 0;
	while (index < data.length()) {
		ss.clear();
		ss << std::hex << data.substr(index, 2);
		ss >> tmp;
		hexChars.push_back((char) (unsigned char) (unsigned int) (tmp));
		index += 2;
	}
	write(pM->confd, &hexChars[0], data.length() / 2);
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

void testInitialize() {
	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	string emails = "1\ntest email\n2\ntest email\n4\ntestemail\n";
	string row("Alice");
	string col("emails");
	client.Put(row, col, emails);
	cout << "Put Alice emails: " << endl << emails << endl;

	client.Put(row, "email-1", "aaaaaaaaaaaaaaa");
	client.Put(row, "email-2", "bbbbbbbbbbbbbbb");
	client.Put(row, "email-4", "ddddddddddddddd");

	string files = "1\nhomework1\n2\nhomework1\n5\nhomework1-sol\n";
	string filesCol("files");
	client.Put(row, filesCol, files);

	string file1;
	string file2;
	string file5;
	readFile("a.png", file1);
	readFile("1.txt", file2);
	readFile("2.txt", file5);
	client.Put(row, "file-1", file1);
	client.Put(row, "file-2", file2);
	client.Put(row, "file-5", file5);

	string response = client.Get(row, col);
	cout << "Get Alice emails: " << endl << response << endl;
	string fileResponse = client.Get(row, "file-1");
	cout << "Get file-1: " << endl << fileResponse << endl;
}

string readHTMLFile(const char* fileLoc) {
	string fileStr;
	FILE* f = fopen(fileLoc, "r");
	char fileLine[BUFF_SIZE];
	while (fgets(fileLine, BUFF_SIZE, f) != NULL) {
		fileStr += fileLine;
		bzero(fileLine, BUFF_SIZE);
	}
	return fileStr;
}

string getResponse(const char* fileLoc, const char* headers) {
	string response = HTTP_HEADER;
	string fileStr = readHTMLFile(fileLoc);
	response += to_string(fileStr.length()) + "\n";
	response += headers;
	response += "\n";
	response += fileStr;
	return response;
}

string getEmailResponse(string user, const char* url) {
	string response = HTTP_HEADER;
	string urlStr = url + 1;
	int emailNameI = urlStr.find(' ');
	string emailName = urlStr.substr(0, emailNameI);

	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	string email = client.Get(user, emailName);
	response += to_string(email.length()) + "\n";
	response += "\n";
	response += email;
	return response;
}

void getFileResponse(string user, const char* url, Message* pM) {
	string response = HTTP_HEADER_FILE;
	string urlStr = url + 1;
	int fileNameI = urlStr.find(' ');
	string fileName = urlStr.substr(0, fileNameI);

	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	string email = client.Get(user, fileName);
	response += to_string(email.length()) + "\n";
	response += "\n";
	write(pM->confd, response.c_str(), response.length());
	writeFile(email, pM);
}

string generateLI(string num, string title, string url) {
	string res;
//	<li><a href="emails/0">aaaa</a></li>
	res = "<li><a href=\"" + url + "-" + num + "\">" + title + "</a></li>";
	return res;
}

string getEmailMid(string emails, string url) {
	string email_mid;
	int first = 0;
	while (first != string::npos) {
		int second = emails.find('\n', first);
		if (second == string::npos)
			break;
		else second++;
		string num = emails.substr(first, second - first - 1);
		first = emails.find('\n', second);
		string title;
		if (first == string::npos)
			title = emails.substr(second);
		else {
			first++;
			title = emails.substr(second, first - second - 1);
		}
		email_mid += generateLI(num, title, url);
	}

	return email_mid;
}

string getListResponse(string user, string url, const char* begin, const char* end, string liurl) {
	string response = HTTP_HEADER;

	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	string emails = client.Get(user, url);
	string email_begin = readHTMLFile(begin);
	string email_end = readHTMLFile(end);
//	string email_begin = readHTMLFile("frontend/sites/emails_begin.html");
//	string email_end = readHTMLFile("frontend/sites/emails_end.html");
	string email_mid;
	email_mid = getEmailMid(emails, liurl);
	email_begin += email_mid + email_end;
	response += to_string(email_begin.length()) + "\n";
	response += "\n";
	response += email_begin;
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
		while (!(letter >= 48 && letter <= 57) && !(letter >= 97 && letter <= 122))
			letter = rand() % 123;
		res += char(letter);
	}
	return res;
}

#endif /* PARSEURL_H_ */
