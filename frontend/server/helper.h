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
#include "../../backend/storage_query/master_client.h"
#include "../../backend/storage_query/storage_client.h"

using namespace std;

#ifndef HELPER_H_
#define HELPER_H_

string generateSessionID(string userName) {
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

string bytesToHexString(int size, const char* dataTmp) {
	std::stringstream ss;
	for (int i = 0; i < size; ++i) {
		ss << std::hex << std::setw(2) << std::setfill('0') << (unsigned int) ((unsigned char) (dataTmp[i]));
	}
	string data = ss.str();
	return data;
}

int readFile(const char* fileName, string& data) {
	ifstream in;
	in.open(fileName, ios::in | ios::binary | ios::ate);
	int size = in.tellg();
	char* dataTmp = (char*) (malloc(sizeof(char) * (size + 1)));
	in.seekg(ios::beg);
	cout << size << endl;
	dataTmp[size] = 0;
	if (!in.read(dataTmp, size)) {
		cout << "read file error" << endl;
	}
	data = bytesToHexString(size, dataTmp);
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

char hexToChar(string hexStr) {
	stringstream ss;
	int res;
	ss << std::hex << hexStr;
	ss >> res;
	return res;
}

void filterHex(string& str) {
	int perc;
	while ((perc = str.find('+')) != string::npos)
		str.replace(perc, 1, 1, ' ');
	while ((perc = str.find('%')) != string::npos) {
		string hexStr = str.substr(perc + 1, 2);
		char res = hexToChar(hexStr);
		str.replace(perc, 3, 1, res);
	}
}

MasterClient master(grpc::CreateChannel("127.0.0.1:8000", grpc::InsecureChannelCredentials()));
StorageClient getClient(string user) {
	vector<string> strs;
	master.GetNode(user, "", strs);
	while (strs.size() == 0)
		master.GetNode(user, "", strs);
	return StorageClient(grpc::CreateChannel(strs[0], grpc::InsecureChannelCredentials()));
}
#endif /* HELPER_H_ */
