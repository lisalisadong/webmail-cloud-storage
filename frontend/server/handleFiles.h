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
#include "helper.h"

using namespace std;
#ifndef HANDLEFOLDER_H_
#define HANDLEFOLDER_H_

string getParentDir(string dir) {
	int slash = dir.find_last_of('/');
	int preSlash = dir.find_last_of('/', slash - 1);
	string res;
	if (preSlash == string::npos)
		res = "files";
	else res = dir.substr(0, preSlash);
	return res;
}

void getFileResponse(string user, const char* url, Message* pM) {
	string urlStr = url + 1;
	int fileNameI = urlStr.find(' ');
	string fileName = urlStr.substr(0, fileNameI);

	string response = HTTP_HEADER_FILE;
	response += fileName.substr(fileName.find_last_of("/-") + 1);
	response += HTTP_HEADER_FILE2;

	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	string email = client.Get(user, fileName);
	response += to_string(email.length()) + "\n";
	response += "\n";
	write(pM->confd, response.c_str(), response.length());
	writeFile(email, pM);
}
string generateFilesLI(string url, string title, bool isFolder) {
	string res;
	if (isFolder) {
		res =
				"<li><a href=\"" + url
						+ "\"> <img src=\"http://icons.iconarchive.com/icons/hopstarter/sleek-xp-basic/16/Folder-icon.png\" alt=\"Folder\" style=\"width:15px;height:15px;\">"
						+ title + "</a></li>";
	} else {
		res = "<li><a href=\"" + url + "\">" + title + "</a></li>";
	}
	return res;
}

string getFileMid(string files) {
	string email_mid;
	int pre = 0;
	int first = files.find('\n', pre);
	while (first != string::npos) {
		string url = files.substr(pre, first - pre);
		bool isFolder = !url.compare(0, strlen(FOLDER_PRE), FOLDER_PRE);
		int titleIndex = url.find_last_of('/');
		if (titleIndex == string::npos) titleIndex = url.find_last_of('-');
		string title = url.substr(titleIndex + 1);
		if (!title.compare("..")) url = getParentDir(url);
		url = "/" + url;
		email_mid += generateFilesLI(url, title, isFolder);
		pre = first + 1;
		first = files.find('\n', pre);
	}
	return email_mid;
}

string getFileListResponse(string user, const char* begin, const char* end, string url) {
	string response = HTTP_HEADER;

	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	string files = client.Get(user, url);
	string email_begin = readHTMLFile(begin);
	string email_end = readHTMLFile(end);
	string email_mid = getFileMid(files);
	email_begin += email_mid + email_end;
	response += to_string(email_begin.length()) + "\n";
	response += "\n";
	response += email_begin;
	return response;
}

void createNewFolder(string user, string referer, string lastLine) {
	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	string folderName = lastLine.substr(lastLine.find('=') + 1); // aaa
	string dir = referer.substr(referer.find('/', 7) + 1); // files / folder-aaa/solution
	if (dir[dir.length() - 2] == '\r') dir.erase(dir.length() - 2, 2);
	if (!dir.compare("files"))
		folderName = "folder-" + folderName; // folder-aaa
	else folderName = dir + "/" + folderName; // folder-aaa/solution/aaa
	string preFiles = client.Get(user, dir);
	preFiles += folderName + "\n";
	client.Put(user, dir, preFiles);
	client.Put(user, folderName, folderName + "/..\n");
}

string getFileName(string lastLine) {
	int start = lastLine.find(FILE_NAME_PRE) + strlen(FILE_NAME_PRE);
	int end = lastLine.find('"', start);
	return lastLine.substr(start, end - start);
}

char* getRawData(const char* lastLine, int& contentLen) {
	int start = 0;
	while (strncmp(lastLine + start, "\r\n\r\n", 4))
		start++;
	start += 4;
	int end = contentLen - 4;
	while (strncmp(lastLine + end, "\r\n", 2))
		end--;
	char* res = (char*) malloc(sizeof(char) * (end - start + 1));
	bzero(res, end - start + 1);
//	strncpy(res, lastLine + start, end - start);
	const char* head = lastLine + start;
	for (int i = 0; i < end - start; i++)
		res[i] = *(head + i);
	contentLen = end - start;
	return res;
}

void createNewFile(string user, string referer, const char* lastLine, int contentLen) {
	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	string fileName = getFileName(lastLine);
	string folder = referer.substr(referer.find('/', 7) + 1); // files / folder-aaa/solution
	if (folder[folder.length() - 2] == '\r') folder.erase(folder.length() - 2, 2);
	string dir = folder;
	if (!dir.compare("files"))
		fileName = "file-" + fileName; // folder-aaa
	else {
		dir.replace(0, 6, "file");
		fileName = dir + "/" + fileName; // folder-aaa/solution/aaa
	}
	string preFiles = client.Get(user, folder);
	if (preFiles.find(fileName) == string::npos) {
		preFiles += fileName + "\n";
		client.Put(user, folder, preFiles);
	}
	char* rawData = getRawData(lastLine, contentLen);
	string data = bytesToHexString(contentLen, rawData);
	free(rawData);
//	string data = bytesToHexString(contentLen - strlen(UPLOADED_FILE) - 1, lastLine.c_str() + strlen(UPLOADED_FILE) + 1);
	client.Put(user, fileName, data);
}

#endif /* HANDLEFOLDER_H_ */
