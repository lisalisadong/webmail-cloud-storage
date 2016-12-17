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
#include "handleEmails.h"
#include "handleFiles.h"

using namespace std;
#ifndef PARSEURL_H_
#define PARSEURL_H_

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

	client.Put(row, "files", "folder-aaa\nfolder-bbb\nfile-1.txt\n");
	client.Put(row, "folder-aaa", "folder-aaa/..\nfolder-aaa/homework3\nfolder-aaa/solution\nfile-aaa/a.png\n");
	client.Put(row, "folder-aaa/solution", "folder-aaa/solution/..\nfile-aaa/solution/2.txt\n");
	client.Put(row, "folder-aaa/homework3", "folder-aaa/homework3/..\n");
	client.Put(row, "folder-bbb", "folder-bbb/..\n");

	/*
	 *	files
	 *	|--aaa
	 *	|	|--homework3
	 *	|	|--solution
	 *	|	|	|--2.txt
	 *	|	|--a.png
	 *	|--bbb
	 *	|--1.txt
	 */
	string apng;
	string txt1;
	string txt2;
	readFile("a.png", apng);
	readFile("1.txt", txt1);
	readFile("2.txt", txt2);
	client.Put(row, "file-aaa/a.png", apng);
	client.Put(row, "file-1.txt", txt1);
	client.Put(row, "file-aaa/solution/2.txt", txt2);

	string response = client.Get(row, col);
	cout << "Get Alice emails: " << endl << response << endl;
	string fileResponse = client.Get(row, "file-aaa/solution/2.txt");
	cout << "Get file-1: " << endl << fileResponse << endl;
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

string getEmail(string& urlStr) {
	string email;
	int emailIndex = urlStr.find("email");
	email = urlStr.substr(emailIndex + 6, urlStr.find('&', emailIndex) - emailIndex - 6);
	return email;
}

bool checkPWD(const char* lastLine, string& email) {
	string urlStr = lastLine;
	email = getEmail(urlStr);
	int passwordIndex = urlStr.find("password");
	string password = urlStr.substr(passwordIndex + 9);
	if (isDebug) cout << "Current login user is: " << email << " with password: " << password << endl;

	return true;
}

bool checkCookie(string cookie, string& email) {
	if (cookie.length() <= 0) return false;
	int newLineI = cookie.find_first_of("\r\n");
	if (newLineI != string::npos) cookie.erase(newLineI);

	int sessionidI = cookie.find("sessionid=");
	int userI = cookie.find("user=");
	string sessionid = cookie.substr(sessionidI + 10, userI - sessionidI - 12);
	email = cookie.substr(userI + 5);
	if (email.compare("Alice") == 0)
		return true;
	else return false;
}

int generateHTML(struct Message* pM, const char* url, const char* lastLine, string cookie, string referer, int contentLen) {
//	get();
	string urlStr = url;
	string response = "";
	if (!strncmp(url, SIGNUP, strlen(SIGNUP)))
		response = getResponse("frontend/sites/signup.html", "");
	else {
		string user;
		if (checkCookie(cookie, user)) {
			if (!strncmp(url + 1, " ", 1)) {
				response = getResponse("frontend/sites/dashboard.html", "");
			} else if (!strncmp(url, EMAILS, strlen(EMAILS))) {
				response = getListResponse(user, string("emails"), "frontend/sites/emails_begin.html", "frontend/sites/emails_end.html", string("email"));
			} else if (!strncmp(url, FILES, strlen(FILES)) || !strncmp(url, FOLDER_, strlen(FOLDER_))) {
//				response = getListResponse(user, string("files"), "frontend/sites/files_begin.html", "frontend/sites/files_end.html", string("file"));
				string dir = url + 1;
				dir = dir.substr(0, dir.find(' '));
				response = getFileListResponse(user, "frontend/sites/files_begin.html", "frontend/sites/files_end.html", dir);
			} else if (!strncmp(url, EMAIL_, strlen(EMAIL_))) {
				response = getEmailResponse(user, url);
			} else if (!strncmp(url, FILE_, strlen(FILE_))) {
				getFileResponse(user, url, pM);
				return 0;
			} else if (urlStr.find(NEW_FOLDER) != string::npos) {
				createNewFolder(user, referer, lastLine);
				response = getResponse("frontend/sites/getBack.html", "");
			} else if (!strncmp(url, UPLOAD_FILE, strlen(UPLOAD_FILE))) {
				createNewFile(user, referer, lastLine, contentLen);
				response = getResponse("frontend/sites/getBack.html", "");
			} else if (!strncmp(url, WRITE_EMAIL, strlen(WRITE_EMAIL))) {
				response = getResponse("frontend/sites/writeEmail.html", "");
			} else if (!strncmp(url, SEND_EMAIL, strlen(SEND_EMAIL))) {

			} else response = getResponse("frontend/sites/notfound.html", "");
		} else {
			if (!strncmp(url, LOGINSUBMIT, strlen(LOGINSUBMIT))) {
				if (checkPWD(lastLine, user)) {
					string headers = "Set-Cookie: sessionid=" + generateSessionID(user) + "\nSet-Cookie: user=" + user + "\n";
					response = getResponse("frontend/sites/dashboard.html", headers.c_str());
				} else response = getResponse("frontend/sites/loginError.html", "");
			} else if (!strncmp(url + 1, " ", 1))
				response = getResponse("frontend/sites/login.html", "");
			else response = getResponse("frontend/sites/notfound.html", "");
		}
	}
	write(pM->confd, response.c_str(), response.length());
	return 0;
}

#endif /* PARSEURL_H_ */
