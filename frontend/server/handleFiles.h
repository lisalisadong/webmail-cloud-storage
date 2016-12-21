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

//	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	StorageClient client = getClient(user);
	string email;
	client.Get(user, fileName, email);
	response += to_string(email.length()) + "\n";
	response += "\n";
	write(pM->confd, response.c_str(), response.length());
	writeFile(email, pM);
}
string generateFilesLI(string url, string title, bool isFolder) {
	string res;
	if (isFolder) {
		res =
				"<li class=\"list-group-item\"><a href=\"" + url
						+ "\"> <img src=\"http://icons.iconarchive.com/icons/hopstarter/sleek-xp-basic/16/Folder-icon.png\" alt=\"Folder\" style=\"width:15px;height:15px;\">"
						+ title + "</a></li>";
	} else {
		res = "<li class=\"list-group-item\"><a href=\"" + url + "\">" + title + "</a></li>";
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

//	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	StorageClient client = getClient(user);
	string files;
	client.Get(user, url, files);
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
//	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	StorageClient client = getClient(user);
	string folderName = lastLine.substr(lastLine.find('=') + 1); // aaa
	string dir = referer.substr(referer.find('/', 7) + 1); // files / folder-aaa/solution
	if (dir[dir.length() - 2] == '\r') dir.erase(dir.length() - 2, 2);
	if (!dir.compare("files"))
		folderName = "folder-" + folderName; // folder-aaa
	else folderName = dir + "/" + folderName; // folder-aaa/solution/aaa
	string preFiles;
	client.Get(user, dir, preFiles);
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
//	StorageClient client(grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials()));
	StorageClient client = getClient(user);
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
	string preFiles;
	client.Get(user, folder, preFiles);
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

void updateFileName(StorageClient& client, string& user, string& preFullName, string& newFullName) {
	// delete pre file
	string preHex;
	client.Get(user, preFullName, preHex); // file-aaa/a.png
	client.Delete(user, preFullName);
	// add new file
	client.Put(user, newFullName, preHex); // file-bbb/a.png
}

void moveFile(string user, string lastLine) {
	StorageClient client = getClient(user);
	filterHex(lastLine);
	int srcL = lastLine.find("src=") + 4;
	int srcR = lastLine.find('&', srcL);
	string src = lastLine.substr(srcL, srcR - srcL); // aaa/a.png
	string srcFileName = src.substr(src.find_last_of("/-") + 1); // a.png

	int desL = srcR + 5;
	string des = lastLine.substr(desL); //bbb

	int srcPreSlash = src.find_last_of('/');
	string srcPreFolder; // files / folder-aaa
	if (srcPreSlash == string::npos)
		srcPreFolder = "files";
	else {
		srcPreFolder = "folder-" + src.substr(0, srcPreSlash);
	}

	// change src folder
	string srcFiles; //folder-aaa/..\nfolder-aaa/homework3\nfolder-aaa/solution\nfile-aaa/a.png\n
	client.Get(user, srcPreFolder, srcFiles);
	int srcIndex = srcFiles.find(src);
	int srcStart = srcFiles.find_last_of('\n', srcIndex) + 1;
	int srcEnd = srcFiles.find('\n', srcIndex);
	srcFiles.replace(srcStart, srcEnd - srcStart + 1, "");
	client.Put(user, srcPreFolder, srcFiles);

	// change des folder
	string desPreFolder = des.length() == 0 ? "files" : "folder-" + des; // folder-bbb / files
	string desFiles; // folder-bbb/..\n
	client.Get(user, desPreFolder, desFiles);
	string desFile; // file-bbb/a.png / file-a.png
	if (des.length() == 0) {
		desFile = "file-" + srcFileName;
	} else {
		desFile = desPreFolder + "/" + srcFileName; // folder-bbb/a.png
		desFile.replace(0, 6, "file");
	}
	desFiles += desFile + "\n";
	client.Put(user, desPreFolder, desFiles);

	string srcFullName = "file-" + src;
	// delete pre file
	updateFileName(client, user, srcFullName, desFile);
}

void renameFile(string user, string lastLine, string referer) {
	StorageClient client = getClient(user);
	filterHex(lastLine);
	int preNameL = lastLine.find("preName=") + 8;
	int preNameR = lastLine.find('&', preNameL);
	string preName = lastLine.substr(preNameL, preNameR - preNameL); // a.png

	int newNameL = preNameR + 9;
	string newName = lastLine.substr(newNameL); //ccc.png

	string dir = referer.substr(referer.find('/', 7) + 1); // files / folder-aaa/solution
	if (dir[dir.length() - 2] == '\r') dir.erase(dir.length() - 2, 2);
	string files;
	client.Get(user, dir, files);
	int srcIndex = files.find(preName);
	files.replace(srcIndex, preName.length(), newName);
	client.Put(user, dir, files);

	string preFullName = dir + "/" + preName;
	preFullName.replace(0, 6, "file");
	string newFullName = dir + "/" + newName;
	newFullName.replace(0, 6, "file");
	updateFileName(client, user, preFullName, newFullName);
}

vector<string> getFileNames(string subFiles) {
	vector<string> res;
	int first = subFiles.find('\n') + 1;
	int second = first;
	while ((second = subFiles.find('\n', first)) != string::npos) {
		res.push_back(subFiles.substr(first, second - first));
		first = second + 1;
	}
	return res;
}

void deleteHelper(string& user, StorageClient& client, string& subFileName) {
	if (!strncmp(subFileName.c_str(), "file-", 5))
		client.Delete(user, subFileName);
	else {
		string subFiles;
		client.Get(user, subFileName, subFiles);
		client.Delete(user, subFileName);
		vector<string> subFileNames = getFileNames(subFiles);
		for (int i = 0; i < subFileNames.size(); i++) {
			deleteHelper(user, client, subFileNames[i]);
		}
	}
}

void deleteFileOrFolder(string user, string lastLine, string referer) {
	StorageClient client = getClient(user);
	filterHex(lastLine);
	string name = lastLine.substr(5); // a.png

	string dir = referer.substr(referer.find('/', 7) + 1); // files / folder-aaa/solution
	if (dir[dir.length() - 2] == '\r') dir.erase(dir.length() - 2, 2);

	string files;
	client.Get(user, dir, files);
	int nameIndex = files.find(name);
	int namePre = files.find_last_of('\n', nameIndex) + 1;
	int namePost = files.find('\n', nameIndex);
	string fullName = files.substr(namePre, namePost - namePre); // file-aaa/a.png
	files.replace(namePre, namePost - namePre + 1, "");
	client.Put(user, dir, files);
	deleteHelper(user, client, fullName);
//	if (!strncmp(fullName.c_str(), "file-", 5))
//		client.Delete(user, fullName);
//	else {
//		string subFiles;
//		client.Get(user, fullName, subFiles);
//		client.Delete(user, fullName);
//		vector<string> subFileNames = getFileNames(subFiles);
//		for (int i = 0; i < subFileNames.size(); i++) {
//			deleteHelper(user, client, subFileNames[i]);
//		}
//	}
}

#endif /* HANDLEFOLDER_H_ */
